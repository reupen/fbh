#include "stdafx.h"

namespace fbh {

void show_info_box(HWND wnd_parent, const char* p_title, const char* p_text, INT icon, uih::alignment text_alignment,
    std::function<std::optional<INT_PTR>(HWND, UINT, WPARAM, LPARAM)> on_before_message)
{
    auto handle_before_message = [on_before_message{std::move(on_before_message)}](
                                     HWND wnd, UINT msg, WPARAM wp, LPARAM lp) -> std::optional<INT_PTR> {
        if (msg == WM_INITDIALOG)
            modeless_dialog_manager::g_add(wnd);
        else if (msg == WM_NCDESTROY)
            modeless_dialog_manager::g_remove(wnd);

        return on_before_message ? on_before_message(wnd, msg, wp, lp) : std::nullopt;
    };

    uih::InfoBox::s_run(wnd_parent, p_title, p_text, icon, std::move(handle_before_message), text_alignment);
}

void show_info_box_threadsafe(
    HWND wnd, const char* p_title, const char* p_text, INT oem_icon, uih::alignment text_alignment)
{
    queue_main_thread_callback(
        [title = pfc::string8{p_title}, text = pfc::string8{p_text}, wnd, oem_icon, text_alignment]() {
            show_info_box(wnd ? wnd : core_api::get_main_window(), title, text, oem_icon, text_alignment);
        });
}

void show_info_box_threadsafe(const char* p_title, const char* p_text, INT oem_icon, uih::alignment text_alignment)
{
    show_info_box_threadsafe(nullptr, p_title, p_text, oem_icon, text_alignment);
}
} // namespace fbh
