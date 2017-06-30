#pragma once

namespace fbh {
    
    static void show_info_box(HWND wnd_parent, const char* p_title, const char* p_text, INT icon = OIC_INFORMATION,
        std::function<void(HWND)> on_creation = nullptr, std::function<void(HWND)> on_destruction = nullptr)
    {
        show_info_box(wnd_parent, p_title, p_text, icon,
            std::bind(&modeless_dialog_manager::g_add, std::placeholders::_1),
            std::bind(&modeless_dialog_manager::g_remove, std::placeholders::_1));
    }

    class callback_t : public main_thread_callback {
    public:
        callback_t(HWND wnd, const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION)
            : m_title(p_title), m_text(p_text), m_wnd(wnd), m_oem_icon(oem_icon) {};
    private:
        void callback_run() override
        {
            show_info_box(m_wnd ? m_wnd : core_api::get_main_window(), m_title, m_text, m_oem_icon, 
                std::bind(&modeless_dialog_manager::g_add, std::placeholders::_1),
                std::bind(&modeless_dialog_manager::g_remove, std::placeholders::_1));
        }

        pfc::string8 m_title, m_text;
        HWND m_wnd;
        INT m_oem_icon;
    };

    static void show_info_box_threadsafe(HWND wnd, const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION)
    {
        service_ptr_t<main_thread_callback> cb = new service_impl_t<callback_t>(wnd, p_title, p_text, oem_icon);
        static_api_ptr_t<main_thread_callback_manager>()->add_callback(cb);
    }

    static void show_info_box_threadsafe(const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION)
    {
        service_ptr_t<main_thread_callback> cb = new service_impl_t<callback_t>((HWND)nullptr, p_title, p_text, oem_icon);
        static_api_ptr_t<main_thread_callback_manager>()->add_callback(cb);
    }
}
