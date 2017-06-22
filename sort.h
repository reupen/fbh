#pragma once

#ifndef uT
#define uT(x) (pfc::stringcvt::string_os_from_utf8(x).get_ptr())
#define uTS(x,s) (pfc::stringcvt::string_os_from_utf8(x,s).get_ptr())
#endif


namespace mmh {
	namespace fb2k {

		void g_sort_metadb_handle_list_by_format_get_permutation_t_partial(metadb_handle_ptr * p_list, t_size p_list_count, t_size base, t_size count, permutation_t & order, const service_ptr_t<titleformat_object> & p_script, titleformat_hook * p_hook, bool b_stablise = false);
		void g_sort_metadb_handle_list_by_format_get_permutation(metadb_handle_ptr * p_list, permutation_t & order, const service_ptr_t<titleformat_object> & p_script, titleformat_hook * p_hook, bool b_stablise = false);
		void g_sort_metadb_handle_list_by_format_get_permutation_t_partial(const pfc::list_base_const_t<metadb_handle_ptr> & p_list, t_size base, t_size count, permutation_t & order, const service_ptr_t<titleformat_object> & p_script, titleformat_hook * p_hook, bool b_stablise = false);
		void g_sort_metadb_handle_list_by_format(pfc::list_base_t<metadb_handle_ptr> & p_list, const service_ptr_t<titleformat_object> & p_script, titleformat_hook * p_hook, bool b_stablise = false);

		template <template<typename> class t_alloc>
		void g_sort_metadb_handle_list_by_format_v2(metadb_handle_list_t<t_alloc> & p_list, const service_ptr_t<titleformat_object> & p_script, titleformat_hook * p_hook, bool b_stablise = false)
		{
			permutation_t perm(p_list.get_count());
			g_sort_metadb_handle_list_by_format_get_permutation_t_partial(p_list.get_ptr(), p_list.get_count(), 0, perm.get_count(), perm, p_script, p_hook, b_stablise);
			p_list.reorder(perm.get_ptr());
		}
		template <template<typename> class t_alloc>
		void g_metadb_handle_list_remove_duplicates(metadb_handle_list_t<t_alloc> & p_handles)
		{
			t_size count = p_handles.get_count();
			if (count > 0)
			{
				metadb_handle_ptr * p_list = p_handles.get_ptr();
				bit_array_bittable mask(count);
				permutation_t order(count);

				g_sort_get_permutation_qsort_v2(p_list, order, (pfc::compare_t<metadb_handle_ptr, metadb_handle_ptr>), false, false);

				t_size n;
				bool found = false;
				for (n = 0; n < count - 1; n++)
				{
					if (p_list[order[n]] == p_list[order[n + 1]])
					{
						found = true;
						mask.set(order[n + 1], true);
					}
				}

				if (found) p_handles.remove_mask(mask);
			}
		}

	}
}
