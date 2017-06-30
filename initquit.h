#pragma once

namespace fbh {
    class initquit_dynamic {
    public:
        virtual void on_init() {}
        virtual void on_quit() {}
    };

    class initquit_multiplexer : public initquit {
    public:
        void register_instance(initquit_dynamic * ptr)
        {
            m_instances.add_item(ptr);
        }
        void deregister_instance(initquit_dynamic * ptr)
        {
            m_instances.remove_item(ptr);
        }
        static void g_register_instance(initquit_dynamic * ptr);
        static void g_deregister_instance(initquit_dynamic * ptr);
    private:
        void on_init() override;
        void on_quit() override;

        pfc::ptr_list_t<initquit_dynamic> m_instances;
    };
}