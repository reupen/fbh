#pragma once

namespace fbh {
    void show_info_box(HWND wnd_parent, const char* p_title, const char* p_text, INT icon = OIC_INFORMATION);
    void show_info_box_threadsafe(HWND wnd, const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION);
    void show_info_box_threadsafe(const char* p_title, const char* p_text, INT oem_icon = OIC_INFORMATION);
}
