#pragma once

namespace fbh {

template <typename... Args>
void print_to_console(Args&&... args)
{
    console::formatter formatter;
    (formatter.add_string(std::forward<Args>(args)), ...);
}

} // namespace fbh
