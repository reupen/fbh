#pragma once

namespace fbh {
template <typename TList>
void sort_metadb_handle_list_by_format_get_permutation_partial(TList&& p_list, t_size p_list_count, t_size base,
    t_size count, mmh::Permutation& order, const service_ptr_t<titleformat_object>& p_script, titleformat_hook* p_hook,
    bool stablise = false, bool reverse = false)
{
    assert(base + count <= p_list_count);
    pfc::array_t<pfc::array_t<WCHAR>> data;
    data.set_size(count);

    concurrency::parallel_for(size_t{0}, count, [&](size_t n) {
        pfc::string8_fastalloc temp;
        p_list[base + n]->format_title(p_hook, temp, p_script, nullptr);
        data[n].set_size(pfc::stringcvt::estimate_utf8_to_wide_quick(temp, temp.length()));
        pfc::stringcvt::convert_utf8_to_wide_unchecked(data[n].get_ptr(), temp);
    });

    const auto comparator = [](const pfc::array_t<WCHAR>& elem1, const pfc::array_t<WCHAR>& elem2) {
        return StrCmpLogicalW(elem1.get_ptr(), elem2.get_ptr());
    };

    mmh::sort_get_permutation(data, order, comparator, stablise, reverse, true);
}

template <typename TList>
void sort_metadb_handle_list_by_format_get_permutation(TList&& p_list, mmh::Permutation& order,
    const service_ptr_t<titleformat_object>& p_script, titleformat_hook* p_hook, bool stablise = false,
    bool reverse = false)
{
    sort_metadb_handle_list_by_format_get_permutation_partial(
        p_list, order.get_count(), 0, order.get_count(), order, p_script, p_hook, stablise, reverse);
}

template <typename TList>
void sort_metadb_handle_list_by_format(TList&& p_list, const service_ptr_t<titleformat_object>& p_script,
    titleformat_hook* p_hook, bool stablise = false, bool reverse = false)
{
    mmh::Permutation perm(p_list.get_count());
    sort_metadb_handle_list_by_format_get_permutation_partial(
        p_list.get_ptr(), p_list.get_count(), 0, perm.get_count(), perm, p_script, p_hook, stablise, reverse);
    p_list.reorder(perm.get_ptr());
}

template <template <typename> class t_alloc>
void metadb_handle_list_remove_duplicates(metadb_handle_list_t<t_alloc>& p_handles)
{
    mmh::remove_duplicates(p_handles, pfc::compare_t<metadb_handle_ptr, metadb_handle_ptr>);
}
} // namespace fbh
