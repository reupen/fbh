#ifndef _MMH_SORT_H_
#define _MMH_SORT_H_

#ifndef uT
#define uT(x) (pfc::stringcvt::string_os_from_utf8(x).get_ptr())
#define uTS(x,s) (pfc::stringcvt::string_os_from_utf8(x,s).get_ptr())
#endif


namespace mmh
{
	typedef pfc::list_t<t_size> permutation_base_t;

	//template <template<typename> class t_alloc = pfc::alloc_fast>
	class permutation_t : public permutation_base_t
	{
	public:
		void reset()
		{
			t_size j, count=get_size();
			t_size * ptr = get_ptr();
			for (j=0; j<count; j++)
				ptr[j] = j;
		}
		void reset_reverse()
		{
			t_size j, count=get_size();
			t_size * ptr = get_ptr();
			for (j=0; j<count; j++)
				ptr[j] = count-j-1;
		}
		void set_size (t_size size)
		{
			pfc::list_t<t_size>::set_size(size);
			reset();
		}
		void set_count (t_size size)
		{
			set_size(size);
		};
		permutation_t(t_size size)
		{
			set_size(size);
			reset();
		};
		permutation_t() {};

		using pfc::list_t<t_size>::operator[];
	};

	class permutation_inverse_t : public permutation_t
	{
	public:
		permutation_inverse_t(const permutation_t & p_source)
		{
			t_size i, count = p_source.get_count();
			permutation_base_t::set_size (count);
			for(i=0;i<count;i++)
				(*this)[p_source[i]] = i;
		}
	};

	class permutation_reverse_t : public permutation_t
	{
	public:
		permutation_reverse_t(const permutation_t & p_source)
		{
			t_size i, count = p_source.get_count();
			permutation_base_t::set_size (count);
			for(i=0;i<count;i++)
				(*this)[i] = p_source[count-i-1];
		}
	};

	class sort_base
	{
	public:
		virtual int compare (const void * item1, const void* item2)=0;
	};

	template <typename t_list, typename t_compare>
	class sort_base_impl : public sort_base
	{
		t_compare m_compare;
		permutation_t & m_perm;
		t_list & m_list;
		bool m_stabilise;
	public:
		virtual int compare (const void * item1, const void* item2)
		{
			int ret = m_compare(m_list[*(t_size*)(item1)], m_list[*(t_size*)(item2)]);
			if (!ret && m_stabilise) ret = pfc::compare_t(*(t_size*)(item1), *(t_size*)(item2));
			return ret;
		}
		sort_base_impl(t_list & p_list, permutation_t & p_perm, t_compare p_compare, bool p_stabilise)
			: m_perm(p_perm), m_compare(p_compare), m_list(p_list), m_stabilise(p_stabilise)
		{
		};
	};

	__forceinline int g_compare_context(void* context, const void * item1, const void* item2);

	template <typename context_t>
	__forceinline int g_compare_context_v2(context_t & context, const void * item1, const void* item2)
	{
		return context.compare(item1, item2);
	}

	template <typename context_t>
	__forceinline int g_compare_context_v3(context_t & context, const t_size * item1, const t_size* item2)
	{
		return context.compare(item1, item2);
	}

	template <typename context_t>
	__forceinline bool g_compare_context_v2_lt(context_t & context, const void * item1, const void* item2)
	{
		return context.compare(item1, item2)<0;
	}

	template <typename t_list, typename t_compare>
	void g_sort_get_permutation_qsort(t_list & p_list, permutation_t & p_out, t_compare p_compare, bool stabilise)
	{
		t_size size = pfc::array_size_t(p_list);
		sort_base_impl<t_list, t_compare> p_context(p_list, p_out, p_compare, stabilise);
		qsort_s(p_out.get_ptr(), size, sizeof(t_size), g_compare_context, static_cast<sort_base*>(&p_context));
	}

	template <typename t_item, typename t_compare>
	class sort_base_impl_v2 : public sort_base
	{
		t_compare m_compare;
		permutation_t & m_perm;
		t_item * m_list;
		bool m_stabilise, m_reverse;
	public:
		virtual int compare (const void * item1, const void* item2)
		{
			int ret = m_compare(m_list[*(t_size*)(item1)], m_list[*(t_size*)(item2)]);
			if (m_reverse)
				ret = -ret;
			if (!ret && m_stabilise) ret = pfc::compare_t(*(t_size*)(item1), *(t_size*)(item2));
			return ret;
		}
		sort_base_impl_v2(t_item * p_list, permutation_t & p_perm, t_compare p_compare, bool p_stabilise, bool b_reverse)
			: m_perm(p_perm), m_compare(p_compare), m_list(p_list), m_stabilise(p_stabilise), m_reverse(b_reverse)
		{
		};
	};

	template <typename t_item, typename t_compare>
	class sort_base_impl_v3
	{
		t_compare m_compare;
		permutation_t & m_perm;
		t_item * m_list;
		bool m_stabilise, m_reverse;
	public:
		__forceinline int compare (const void * item1, const void* item2)
		{
			int ret = m_compare(m_list[*(t_size*)(item1)], m_list[*(t_size*)(item2)]);
			if (m_reverse)
				ret = -ret;
			if (!ret && m_stabilise) ret = pfc::compare_t(*(t_size*)(item1), *(t_size*)(item2));
			return ret;
		}
		sort_base_impl_v3(t_item * p_list, permutation_t & p_perm, t_compare p_compare, bool p_stabilise, bool b_reverse)
			: m_perm(p_perm), m_compare(p_compare), m_list(p_list), m_stabilise(p_stabilise), m_reverse(b_reverse)
		{
		};
	};

	template <typename t_item, typename t_compare, bool b_stabilise, bool b_reverse>
	class sort_base_impl_v4
	{
		t_compare m_compare;
		permutation_t & m_perm;
		t_item * m_list;
	public:
		__forceinline int compare (const t_size * item1, const t_size * item2)
		{
			int ret = m_compare(m_list[*item1], m_list[*item2]);
			if (b_reverse)
				ret = -ret;
			if (b_stabilise && !ret) ret = pfc::compare_t(*item1, *item2);
			return ret;
		}
		sort_base_impl_v4(t_item * p_list, permutation_t & p_perm, t_compare p_compare)
			: m_perm(p_perm), m_compare(p_compare), m_list(p_list)
		{
		};
	};

	template <typename t_item, typename t_compare>
	void g_sort_get_permutation_qsort_v2(t_item * p_items, permutation_t & p_out, t_compare p_compare, bool stabilise, bool b_reverse = false)
	{
		t_size psize = pfc::array_size_t(p_out);
		sort_base_impl_v2<t_item, t_compare> p_context(p_items, p_out, p_compare, stabilise, b_reverse);
		qsort_s(p_out.get_ptr(), psize, sizeof(t_size), &g_compare_context, (void*)&p_context);
		{
			//t_size * ptr = p_out.get_ptr();
			//_qsort_s(ptr, psize, sizeof(t_size), (g_compare_context_v2< sort_base_impl_v3<t_item, t_compare> >), p_context);
			//QSORT(t_size, ptr, psize, (g_compare_context_v2_lt< sort_base_impl_v3<t_item, t_compare> >), p_context);
		}
	}

	template <typename t_item, typename t_compare>
	void g_sort_qsort_v2(t_item * p_items, t_size count, t_compare p_compare, bool stabilise, bool b_reverse = false)
	{
		const t_size psize = count;
		permutation_t permutation(psize);
		sort_base_impl_v2<t_item, t_compare> p_context(p_items, permutation, p_compare, stabilise, b_reverse);
		qsort_s(permutation.get_ptr(), psize, sizeof(t_size), &g_compare_context, (void*)&p_context);
		pfc::reorder_t(p_items, permutation.get_ptr(), psize);
		{
			//t_size * ptr = p_out.get_ptr();
			//_qsort_s(ptr, psize, sizeof(t_size), (g_compare_context_v2< sort_base_impl_v3<t_item, t_compare> >), p_context);
			//QSORT(t_size, ptr, psize, (g_compare_context_v2_lt< sort_base_impl_v3<t_item, t_compare> >), p_context);
		}
	}

	template <bool b_stabilise, bool b_reverse, typename t_item, typename t_compare>
	__forceinline void g_sort_get_permutation_qsort_v3(t_item * p_items, permutation_t & p_out, t_compare p_compare)
	{
		t_size psize = pfc::array_size_t(p_out);
		sort_base_impl_v3<t_item, t_compare> p_context(p_items, p_out, p_compare, b_stabilise, b_reverse);
		crtsort::qsort_s(p_out.get_ptr(), psize, sizeof(t_size), g_compare_context_v3< sort_base_impl_v4<t_item, t_compare, b_stabilise, b_reverse> >, p_context);
		{
			//t_size * ptr = p_out.get_ptr();
			//_qsort_s(ptr, psize, sizeof(t_size), (g_compare_context_v2< sort_base_impl_v3<t_item, t_compare> >), p_context);
			//QSORT(t_size, ptr, psize, (g_compare_context_v2_lt< sort_base_impl_v3<t_item, t_compare> >), p_context);
		}
	}

	#define g_sort_get_permutation_qsort_v4(item_type_t, p_items, p_out, p_compare, stabilise, b_reverse) \
	{ \
		_qsort_s(p_out.get_ptr(), pfc::array_size_t(p_out), 1, p_compare, item_type_t, p_items); \
	}

	template <typename t_item, template<typename> class t_alloc, typename t_compare>
	void g_list_remove_duplicates(pfc::list_t<t_item, t_alloc> & p_handles, t_compare p_compare)
	{
		t_size count = p_handles.get_count();
		if (count>0)
		{
			t_item * p_list = p_handles.get_ptr();
			bit_array_bittable mask(count);
			permutation_t order(count);

			g_sort_get_permutation_qsort_v2(p_list, order, p_compare, false, false);
			
			t_size n;
			bool found = false;
			for(n=0;n<count-1;n++)
			{
				if (p_list[order[n]]==p_list[order[n+1]])
				{
					found = true;
					mask.set(order[n+1],true);
				}
			}
			
			if (found) p_handles.remove_mask(mask);
		}
	}

	template<typename t_container,typename t_compare, typename t_param>
	class bsearch_callback_impl_simple_partial_t : public pfc::bsearch_callback {
	public:
		int test(t_size p_index) const {
			return m_compare(m_container[m_base+p_index],m_param);
		}
		bsearch_callback_impl_simple_partial_t(const t_container & p_container,t_compare p_compare,const t_param & p_param, t_size base)
			: m_container(p_container), m_compare(p_compare), m_param(p_param), m_base(base)
		{
		}
	private:
		const t_container & m_container;
		t_compare m_compare;
		const t_param & m_param;
		t_size m_base;
	};

	template<typename t_container,typename t_compare, typename t_param>
	bool bsearch_partial_t(t_size p_count,const t_container & p_container,t_compare p_compare,const t_param & p_param,t_size base,t_size & p_index) 
	{
		t_size index = p_index;
		bool ret = bsearch(
			p_count,
			bsearch_callback_impl_simple_partial_t<t_container,t_compare,t_param>(p_container,p_compare,p_param,base),
			index);
		p_index = index + base;
		return ret;
	}

#if 0
	template<typename t_container,typename t_compare, typename t_param>
	bool bsearch_nearest(t_size p_count,const t_container & p_container,t_compare p_compare,const t_param & p_param,t_size & p_index) {
		return bsearch(
			p_count,
			bsearch_callback_impl_simple_t<t_container,t_compare,t_param>(p_container,p_compare,p_param),
			p_index);
	}

	template<typename t_callback>
	void __bsearch_nearest(t_size p_count, const t_callback & p_callback,t_size & p_result)
	{
		t_size max = m_items.get_count();
		t_size min = 0;
		t_size ptr;
		while(min<max)
		{
			ptr = min + ( (max-min) >> 1);
			int result = p_callback.test(ptr);
			if (result<0) min = ptr + 1;
			else if (result>0) max = ptr;
			else 
			{
				return ptr;
				//return true;
			}
		}
		if (min==m_items.get_count()) min--;
		return min;
		//return true;
	}
#endif

	namespace fb2k
	{
		void g_sort_metadb_handle_list_by_format_get_permutation_t_partial(metadb_handle_ptr * p_list, t_size p_list_count,t_size base,t_size count,permutation_t & order,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise = false);
		void g_sort_metadb_handle_list_by_format_get_permutation(metadb_handle_ptr * p_list, permutation_t & order,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise = false);
		void g_sort_metadb_handle_list_by_format_get_permutation_t_partial(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,t_size base,t_size count,permutation_t & order,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise = false);
		void g_sort_metadb_handle_list_by_format(pfc::list_base_t<metadb_handle_ptr> & p_list,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise = false);

		template <template<typename> class t_alloc>
		void g_sort_metadb_handle_list_by_format_v2(metadb_handle_list_t<t_alloc> & p_list,const service_ptr_t<titleformat_object> & p_script,titleformat_hook * p_hook, bool b_stablise = false)
		{
			permutation_t perm (p_list.get_count());
			g_sort_metadb_handle_list_by_format_get_permutation_t_partial(p_list.get_ptr(), p_list.get_count(), 0, perm.get_count(), perm, p_script, p_hook, b_stablise);
			p_list.reorder(perm.get_ptr());
		}
		template <template<typename> class t_alloc>
		void g_metadb_handle_list_remove_duplicates(metadb_handle_list_t<t_alloc> & p_handles)
		{
			t_size count = p_handles.get_count();
			if (count>0)
			{
				metadb_handle_ptr * p_list = p_handles.get_ptr();
				bit_array_bittable mask(count);
				permutation_t order(count);

				g_sort_get_permutation_qsort_v2(p_list, order, (pfc::compare_t<metadb_handle_ptr,metadb_handle_ptr>), false, false);
				
				t_size n;
				bool found = false;
				for(n=0;n<count-1;n++)
				{
					if (p_list[order[n]]==p_list[order[n+1]])
					{
						found = true;
						mask.set(order[n+1],true);
					}
				}
				
				if (found) p_handles.remove_mask(mask);
			}
		}

	}

};

#endif //_MMH_SORT_H_