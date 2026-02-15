#pragma once

namespace fbh {
/**
 * \brief Implementation of config_object for integers using std::atomic<> to store values in memory.
 * \tparam Value         Integer or floating-point type of the value to store
 */
template <typename Value>
requires std::is_arithmetic_v<Value>
class ConfigObjectArithmetic
    : public config_object
    , cfg_var {
public:
    using ChangeEventHandler = std::function<void(Value new_value, std::optional<uint32_t> source_id)>;

    ConfigObjectArithmetic(const GUID& guid, bool default_value) : cfg_var(guid), m_value(default_value) {}

    operator Value() const { return get_value(); }

    ConfigObjectArithmetic& operator=(Value new_value)
    {
        set_value(new_value);
        return *this;
    }

    Value get_value() const { return m_value.load(std::memory_order_acquire); }

    void set_value(Value new_value, std::optional<uint32_t> source_id = {})
    {
        m_value.store(new_value, std::memory_order_release);

        fb2k::inMainThread2([this, source_id] {
            const auto value = get_value();

            for (auto& callback : m_callbacks)
                (*callback)(value, source_id);
        });
    }

    mmh::EventToken::Ptr on_change(ChangeEventHandler handler)
    {
        core_api::assert_main_thread();
        return mmh::make_event_token(m_callbacks, std::move(handler));
    }

private:
    GUID get_guid() const override { return cfg_var::get_guid(); }

    void get_data(stream_writer* p_stream, abort_callback& p_abort) const override
    {
        p_stream->write_lendian_t(get_value(), p_abort);
    }

    void set_data(stream_reader* p_stream, abort_callback& p_abort, bool p_sendnotify = true) override
    {
        Value new_value;
        p_stream->read_lendian_t(new_value, p_abort);
        set_value(new_value);
        if (p_sendnotify)
            config_object_notify_manager::g_on_changed(this);
    }

    void get_data_raw(stream_writer* p_stream, abort_callback& p_abort) override { get_data(p_stream, p_abort); }

    void set_data_raw(stream_reader* p_stream, t_size p_sizehint, abort_callback& p_abort) override
    {
        set_data(p_stream, p_abort, false);
    }

    std::atomic<Value> m_value;
    std::vector<std::shared_ptr<ChangeEventHandler>> m_callbacks;
};

/**
 * \brief Factory for ConfigObjectIntegral services.
 * \tparam Value         Integer or floating-point of the value to store
 */
template <typename Value>
class ConfigObjectArithmeticFactory : public service_factory_single_transparent_t<ConfigObjectArithmetic<Value>> {
public:
    using service_factory_single_transparent_t<ConfigObjectArithmetic<Value>>::service_factory_single_transparent_t;
    using ConfigObjectArithmetic<Value>::operator=;
};

/**
 * \brief Factory for boolean ConfigObjectIntegral services.
 */
using ConfigObjectBoolFactory = ConfigObjectArithmeticFactory<bool>;

} // namespace fbh
