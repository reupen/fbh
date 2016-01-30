#include "stdafx.h"





namespace mmh 
{

	int g_compare_context(void* context, const void * item1, const void* item2)
	{
		return ((sort_base*)(context))->compare(item1, item2);
	}

	int g_compare_context_lt(void* context, const void * item1, const void* item2)
	{
		return ((sort_base*)(context))->compare(item1, item2)<0;
	}

	namespace fb2k
	{
		int g_sort_metadb_handle_list_by_format_gepermutation_t_partial_compare(const pfc::array_t<WCHAR> & elem1, const pfc::array_t<WCHAR> & elem2 )
		{
			return StrCmpLogicalW(elem1.get_ptr(), elem2.get_ptr());
		}

		void g_sort_metadb_handle_list_by_format_get_permutation_t_partial(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,t_size base,t_size count,permutation_t & order,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise)
		{
			assert(base+count<=p_list.get_count());
			t_size n;
			pfc::array_t< pfc::array_t<WCHAR> > data;
			data.set_size(count);

			pfc::string8_fastalloc temp;
			pfc::string8_fastalloc temp2;
			temp.prealloc(512);
			for(n=0;n<count;n++)
			{
				metadb_handle_ptr item;
				p_list.get_item_ex(item,base+n);
				assert(item.is_valid());

				item->format_title(p_hook,temp,p_script,0);
				data[n].set_size(pfc::stringcvt::estimate_utf8_to_wide_quick(temp, temp.length()));
				pfc::stringcvt::convert_utf8_to_wide_unchecked(data[n].get_ptr(), temp);
			}

			g_sort_get_permutation_qsort(data, order, g_sort_metadb_handle_list_by_format_gepermutation_t_partial_compare, b_stablise);
		}

		void g_sort_metadb_handle_list_by_format_get_permutation(metadb_handle_ptr * p_list, permutation_t & order,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise)
		{
			g_sort_metadb_handle_list_by_format_get_permutation_t_partial(p_list, order.get_count(), 0, order.get_count(), order, p_script, p_hook, b_stablise);
		}
		void g_sort_metadb_handle_list_by_format_get_permutation_t_partial(metadb_handle_ptr * p_list, t_size p_list_count, t_size base,t_size count,permutation_t & order,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise)
		{
			assert(base+count<=p_list_count);
			t_size n;
			pfc::array_t< pfc::array_t<WCHAR> > data;
			data.set_size(count);

			pfc::string8_fastalloc temp;

			temp.prealloc(512);
			for(n=0;n<count;n++)
			{
				p_list[base+n]->format_title(p_hook,temp,p_script,0);
				data[n].set_size(pfc::stringcvt::estimate_utf8_to_wide_quick(temp, temp.length()));
				pfc::stringcvt::convert_utf8_to_wide_unchecked(data[n].get_ptr(), temp);
			}

			g_sort_get_permutation_qsort(data, order, g_sort_metadb_handle_list_by_format_gepermutation_t_partial_compare, b_stablise);

		}
		void g_sort_metadb_handle_list_by_format(pfc::list_base_t<metadb_handle_ptr> & p_list,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise)
		{
			permutation_t perm (p_list.get_count());
			g_sort_metadb_handle_list_by_format_get_permutation_t_partial(p_list, 0, perm.get_count(), perm, p_script, p_hook, b_stablise);
			p_list.reorder(perm.get_ptr());
		}
	}
}
