#include "stdafx.h"

namespace fbh {
    class library_callback_multiplex_t : public library_callback {
    public:
        void register_callback(library_callback_t * p_callback);
        void deregister_callback(library_callback_t * p_callback);
    private:
        void on_items_added(const pfc::list_base_const_t<metadb_handle_ptr> & p_data) override;
        void on_items_removed(const pfc::list_base_const_t<metadb_handle_ptr> & p_data) override;
        void on_items_modified(const pfc::list_base_const_t<metadb_handle_ptr> & p_data) override;

        pfc::ptr_list_t<library_callback_t> m_callbacks;
    };

    void library_callback_multiplex_t::register_callback(library_callback_t * p_callback)
    {
        m_callbacks.add_item(p_callback);
    }

    void library_callback_multiplex_t::deregister_callback(library_callback_t * p_callback)
    {
        m_callbacks.remove_item(p_callback);
    }

    void library_callback_multiplex_t::on_items_added(const pfc::list_base_const_t<metadb_handle_ptr> & p_data)
    {
        t_size i, count = m_callbacks.get_count();
        for (i = 0; i < count; i++)
            m_callbacks[i]->on_items_added(p_data);
    }

    void library_callback_multiplex_t::on_items_removed(const pfc::list_base_const_t<metadb_handle_ptr> & p_data)
    {
        t_size i, count = m_callbacks.get_count();
        for (i = 0; i < count; i++)
            m_callbacks[i]->on_items_removed(p_data);
    }

    void library_callback_multiplex_t::on_items_modified(const pfc::list_base_const_t<metadb_handle_ptr> & p_data)
    {
        t_size i, count = m_callbacks.get_count();
        for (i = 0; i < count; i++)
            m_callbacks[i]->on_items_modified(p_data);
    }

    library_callback_factory_t<library_callback_multiplex_t> g_library_callback_multiplex;

    namespace library_callback_manager
    {
        void g_register_callback(library_callback_t * p_callback)
        {
            g_library_callback_multiplex.get_static_instance().register_callback(p_callback);
        }
        void g_deregister_callback(library_callback_t * p_callback)
        {
            g_library_callback_multiplex.get_static_instance().deregister_callback(p_callback);
        }
    };

    library_callback_autoreg_t::library_callback_autoreg_t()
    {
        library_callback_manager::g_register_callback(this);
    }

    library_callback_autoreg_t::~library_callback_autoreg_t()
    {
        library_callback_manager::g_deregister_callback(this);
    }
}
