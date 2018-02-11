#pragma once

namespace fbh {
class LambdaMainThreadCallback : public main_thread_callback {
public:
    LambdaMainThreadCallback(std::function<void()> callback) : m_callback{callback} {}

private:
    void callback_run() override { m_callback(); }

    std::function<void()> m_callback;
};

inline void queue_main_thread_callback(std::function<void()> callback)
{
    service_ptr_t<main_thread_callback> cb = new service_impl_t<LambdaMainThreadCallback>(callback);
    static_api_ptr_t<main_thread_callback_manager>()->add_callback(cb);
}
} // namespace fbh