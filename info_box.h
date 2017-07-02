#pragma once

#include "main_thread_callback.h"

namespace fbh {
    
    static void show_info_box(HWND wnd_parent, const char* p_title, const char* p_text, INT icon = OIC_INFORMATION)
    {
        uih::InfoBox::g_run(wnd_parent, p_title, p_text, icon,
            std::bind(&modeless_dialog_manager::g_add, std::placeholders::_1),
            std::bind(&modeless_dialog_manager::g_remove, std::placeholders::_1));
    }

    static void show_info_box_threadsafe(HWND wnd, const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION)
    {
        queue_main_thread_callback([title=pfc::string8{p_title}, text=pfc::string8{p_text},wnd,oem_icon]()
        {
            show_info_box(wnd ? wnd : core_api::get_main_window(), title, text, oem_icon);
        });
    }

    static void show_info_box_threadsafe(const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION)
    {
        show_info_box_threadsafe(nullptr, p_title, p_text, oem_icon);
    }
}
