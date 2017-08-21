#pragma once

#include "fcl.h"

namespace fbh {

    template <typename t_type>
    class [[deprecated("Use fbh::ConfigItem instead")]] config_item_t {
        cfg_int_t<t_type> m_value;
    public:

        void reset() { set(get_default_value()); }
        void set(t_type p_val) { m_value = p_val; on_change(); }
        t_type get() const { return m_value; };

        virtual t_type get_default_value() = 0;
        virtual void on_change() = 0;
        virtual const GUID & get_guid() = 0;
        config_item_t(const GUID & p_guid, t_type p_value) : m_value(p_guid, p_value) {}
    };

    template<>
    class [[deprecated("Use fbh::ConfigItem instead")]] config_item_t<pfc::string8> {
        cfg_string m_value;
    public:
        void reset() { set(get_default_value()); }
        void set(const char * p_val) { m_value = p_val; on_change(); }
        const char * get() const { return m_value; };

        virtual const char * get_default_value() = 0;
        virtual void on_change() = 0;
        virtual const GUID & get_guid() = 0;
        config_item_t(const GUID & p_guid, const char * p_value) : m_value(p_guid, p_value) {}
    };

    template <typename ValueType, typename ImplType = cfg_int_t<ValueType>>
    class ConfigItem {
    public:
        using Type = ConfigItem<ValueType, ImplType>;

        void reset() 
        {
            set(m_default_value);
        }
        
        void set(ValueType newValue) 
        {
            m_value = newValue;
            if (m_on_change)
                m_on_change(m_value);
        }

        ValueType get() const 
        {
            return m_value;
        }

        ValueType get_default_value() const
        {
            return m_default_value;
        }

        operator ValueType () const
        {
            return m_value;
        }

        Type & operator = (const ValueType & newValue)
        {
            set(newValue); 
            return *this;
        }

        ConfigItem(const GUID & guid, ValueType defaultValue, std::function<void(const ValueType&)> on_change = nullptr)
            : m_value(guid, defaultValue), m_default_value{defaultValue}, m_on_change{on_change}
        {}

        virtual ~ConfigItem() {}

    private:
        ImplType m_value;
        ValueType m_default_value;
        std::function<void(const ValueType&)> m_on_change;
    };

    using ConfigUint32 = ConfigItem<uint32_t>;
    using ConfigInt32 = ConfigItem<int32_t>;
    using ConfigBool = ConfigItem<bool>;
    using ConfigString = ConfigItem<const char*, cfg_string>;

    template<typename TInteger>
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
        Type& operator =(TInteger value) { set(value);  return *this; }
        Type& operator =(uih::IntegerAndDpi<TInteger> value) { set(value);  return *this; }

        operator TInteger () const { return m_value.get_scaled_value(); }
        const ValueType & get_raw_value() const { return m_value; };

        TInteger get_scaled_value() const { return m_value.get_scaled_value(); };

        virtual void on_change() {};
        ConfigIntegerDpiAware(const GUID & guid, TInteger value) : cfg_var(guid), m_value(ValueType(value))
        {};
    protected:
        void get_data_raw(stream_writer * p_stream, abort_callback & p_abort) override
        {
            p_stream->write_lendian_t(m_value.value, p_abort);
            p_stream->write_lendian_t(m_value.dpi, p_abort);
        }
        void set_data_raw(stream_reader * p_stream, t_size p_sizehint, abort_callback & p_abort) override
        {
            p_stream->read_lendian_t(m_value.value, p_abort);
            // Allow migration from older config variables
            if (p_sizehint > sizeof(TInteger))
                p_stream->read_lendian_t(m_value.dpi, p_abort);
            else
                m_value.dpi = uih::get_system_dpi_cached().cx; //If migrating from an older config var, assume it was set using the current system DPI.
        }
    private:
        uih::IntegerAndDpi<TInteger> m_value;
    };

    using ConfigUint32DpiAware = ConfigIntegerDpiAware<uint32_t>;
    using ConfigInt32DpiAware = ConfigIntegerDpiAware<int32_t>;

    namespace fcl {
        template<typename t_int>
        void fcl_read_item(Reader& reader, fbh::config_item_t<t_int>& item)
        {
            item.set(reader.read_raw_item<t_int>());
        }
    }
}
