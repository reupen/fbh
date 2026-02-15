#pragma once

#include "fcl.h"

namespace fbh {

template <typename Value, typename Impl = cfg_int_t<Value>>
class ConfigItem {
public:
    using ChangeEventHandler = std::conditional_t<std::is_arithmetic_v<Value>,
        std::function<void(Value new_value, Value old_value, std::optional<uint32_t> source_id)>,
        std::function<void(const Value& new_value, std::optional<uint32_t> source_id)>>;

    void reset() { set(m_default_value); }

    void set(Value new_value, std::optional<uint32_t> source_id = {})
    {
        if constexpr (std::is_arithmetic_v<Value>) {
            Value old_value{};
            old_value = m_value.get_value();
            m_value = new_value;

            for (auto& callback : m_callbacks)
                (*callback)(m_value, old_value, source_id);
        } else {
            m_value = new_value;

            for (auto& callback : m_callbacks)
                (*callback)(m_value, source_id);
        }
    }

    Value get() const { return m_value; }

    Value get_default_value() const { return m_default_value; }

    operator Value() const { return m_value; }

    ConfigItem& operator=(const Value& new_value)
    {
        set(new_value);
        return *this;
    }

    mmh::EventToken::Ptr on_change(ChangeEventHandler handler)
    {
        return mmh::make_event_token(m_callbacks, std::move(handler));
    }

    ConfigItem(const GUID& id, Value default_value, ChangeEventHandler on_change = nullptr)
        : m_value(id, default_value)
        , m_default_value{default_value}

    {
        if (on_change)
            m_callbacks.emplace_back(std::make_shared<ChangeEventHandler>(std::move(on_change)));
    }

    virtual ~ConfigItem() {}

private:
    Impl m_value;
    Value m_default_value;
    std::vector<std::shared_ptr<ChangeEventHandler>> m_callbacks;
};

using ConfigUint32 = ConfigItem<uint32_t>;
using ConfigInt32 = ConfigItem<int32_t>;
using ConfigBool = ConfigItem<bool>;
using ConfigString = ConfigItem<const char*, cfg_string>;

template <typename TInteger>
class ConfigIntegerDpiAware : public cfg_var {
public:
    using ValueType = uih::IntegerAndDpi<TInteger>;
    using Type = ConfigIntegerDpiAware<TInteger>;

    // X and Y DPIs are always the same for 'Windows apps', according to MSDN.
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn312083%28v=vs.85%29.aspx
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn280510%28v=vs.85%29.aspx
    void set(TInteger value, uint32_t dpi = uih::get_system_dpi_cached().cx)
    {
        m_value.set(value, dpi);
        on_change();
    }
    void set(uih::IntegerAndDpi<TInteger> value)
    {
        m_value = value;
        on_change();
    }
    Type& operator=(TInteger value)
    {
        set(value);
        return *this;
    }
    Type& operator=(uih::IntegerAndDpi<TInteger> value)
    {
        set(value);
        return *this;
    }

    operator TInteger() const { return m_value.get_scaled_value(); }
    const ValueType& get_raw_value() const { return m_value; }

    TInteger get_scaled_value() const { return m_value.get_scaled_value(); }

    virtual void on_change() {}
    ConfigIntegerDpiAware(const GUID& guid, TInteger value) : cfg_var(guid), m_value(ValueType(value)) {}

protected:
    void get_data_raw(stream_writer* p_stream, abort_callback& p_abort) override
    {
        p_stream->write_lendian_t(m_value.value, p_abort);
        p_stream->write_lendian_t(m_value.dpi, p_abort);
    }
    void set_data_raw(stream_reader* p_stream, t_size p_sizehint, abort_callback& p_abort) override
    {
        p_stream->read_lendian_t(m_value.value, p_abort);
        // Allow migration from older config variables
        if (p_sizehint > sizeof(TInteger))
            p_stream->read_lendian_t(m_value.dpi, p_abort);
        else
            m_value.dpi
                = uih::get_system_dpi_cached()
                      .cx; // If migrating from an older config var, assume it was set using the current system DPI.
    }

private:
    uih::IntegerAndDpi<TInteger> m_value;
};

using ConfigUint32DpiAware = ConfigIntegerDpiAware<uint32_t>;
using ConfigInt32DpiAware = ConfigIntegerDpiAware<int32_t>;

namespace fcl {
/**
 * \brief Writes an integer held in a ConfigItem<> object to an FCL writer.
 *
 * \tparam t_int    Integral or floating-point type
 * \param writer    FCL writer
 * \param id        Item ID
 * \param item      Value to write
 */
template <typename t_int>
void fcl_write_item(Writer& writer, unsigned id, const ConfigItem<t_int>& item)
requires std::integral<t_int> || std::floating_point<t_int>
{
    writer.write_item(id, static_cast<t_int>(item));
}

/**
 * \brief Reads an integer from an FCL reader to a ConfigItem<> object.
 *
 * \tparam t_int    Integral or floating-point type
 * \param reader    FCL reader
 * \param item      Output object
 */
template <typename t_int>
void fcl_read_item(Reader& reader, ConfigItem<t_int>& item)
requires std::integral<t_int> || std::floating_point<t_int>
{
    item = reader.read_raw_item<t_int>();
}

/**
 * \brief Writes an integer and DPI held in a ConfigIntegerDpiAware<> object to an FCL writer.
 *
 * \tparam t_int    Integral or floating-point type
 * \param writer    FCL writer
 * \param id        Item ID
 * \param item      Value to write
 */
template <typename t_int>
void fcl_write_item(Writer& writer, unsigned id, const ConfigIntegerDpiAware<t_int>& item)
{
    const auto raw_value = item.get_raw_value();
    static_assert(sizeof(raw_value.dpi) == sizeof(uint32_t));
    writer.write_raw(id);
    constexpr uint32_t size{sizeof(t_int) + sizeof(uint32_t)};
    writer.write_raw(size);
    writer.write_raw(raw_value.value);
    writer.write_raw(raw_value.dpi);
}

/**
 * \brief Reads an integer and DPI from an FCL reader to a ConfigIntegerDpiAware<> object.
 *
 * \tparam t_int    Integral or floating-point type
 * \param reader    FCL reader
 * \param item      Output object
 */
template <typename t_int>
void fcl_read_item(Reader& reader, ConfigIntegerDpiAware<t_int>& item)
{
    const auto value = reader.read_raw_item<t_int>();
    const auto dpi = reader.read_raw_item<uint32_t>();
    item.set(value, dpi);
}
} // namespace fcl
} // namespace fbh
