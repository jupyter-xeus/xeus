/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_COMM_HPP
#define XEUS_COMM_HPP

#include <functional>
#include <list>
#include <map>
#include <string>
#include <utility>

#include "xeus/xguid.hpp"
#include "xeus/xjson.hpp"
#include "xeus/xmessage.hpp"

namespace nl = nlohmann;

namespace xeus
{
    /***********************
     * xtarget declaration *
     ***********************/

    class xcomm;
    class xcomm_manager;

    /**
     * @class xtarget
     * @brief Comm target.
     */
    class XEUS_API xtarget
    {
    public:

        using function_type = std::function<void(xcomm&&, xmessage)>;

        xtarget(const std::string& name, const function_type& callback, xcomm_manager* manager);

        const std::string& name() const noexcept;

        void operator()(xcomm&& comm, xmessage request) const;

        void publish_message(const std::string&, nl::json, nl::json, buffer_sequence) const;

        void register_comm(xguid, xcomm*) const;
        void unregister_comm(xguid) const;

    private:

        std::string m_name;
        function_type m_callback;
        xcomm_manager* p_manager;
    };

    /*********************
     * xcomm declaration *
     *********************/

    /**
     * @class xcomm
     * @brief Comm object.
     *
     */
    class XEUS_API xcomm
    {
    public:

        using handler_type = std::function<void(xmessage)>;

        explicit xcomm(const xtarget* target, xguid id = xeus::new_xguid());
        ~xcomm();
        xcomm(xcomm&&);
        xcomm(const xcomm&);

        xcomm& operator=(xcomm&&);
        xcomm& operator=(const xcomm&);

        void open(nl::json metadata, nl::json data, buffer_sequence buffers);
        void close(nl::json metadata, nl::json data, buffer_sequence buffers);
        void send(nl::json metadata, nl::json data, buffer_sequence buffers) const;

        const xtarget& target() const noexcept;

        void handle_message(xmessage request);
        void handle_close(xmessage request);

        xguid id() const noexcept;

        template <class T>
        void on_message(T&& handler);
        template <class T>
        void on_close(T&& handler);

    private:

        friend class xcomm_manager;

        /**
         * Send comm message on iopub. Compose iopub message with specified data
         * and rebinds to target's publish_message
         * {
         *    "comm_id": m_id,
         *    "target_name": specified target name,
         *    "data": specified data
         * }
         */
        void send_comm_message(const std::string& msg_type,
                               nl::json metadata,
                               nl::json data,
                               buffer_sequence) const;
        void send_comm_message(const std::string& msg_type,
                               nl::json metadata,
                               nl::json data,
                               buffer_sequence,
                               const std::string& target_name) const;

        handler_type m_close_handler;
        handler_type m_message_handler;
        const xtarget* p_target;
        xguid m_id;
        bool m_moved_from;
    };

    /*****************************
     * xcomm_manager declaration *
     *****************************/

    class xkernel_core;

    /**
     * @class xcomm_manager
     * @brief Manager and registry for comms and comm targets in the kernel.
     */
    class XEUS_API xcomm_manager
    {
    public:

        xcomm_manager(xkernel_core* kernel = nullptr);

        using target_function_type = xtarget::function_type;

        void register_comm_target(const std::string& target_name,
                                  const target_function_type& callback);
        void unregister_comm_target(const std::string& target_name);

        void comm_open(xmessage request);
        void comm_close(xmessage request);
        void comm_msg(xmessage request);

        const std::map<xguid, xcomm*>& comms() const noexcept;

        const xtarget* target(const std::string& target_name) const;

    private:

        friend class xtarget;

        void register_comm(xguid, xcomm*);
        void unregister_comm(xguid);

        nl::json get_metadata() const;

        std::map<xguid, xcomm*> m_comms;
        std::map<std::string, xtarget> m_targets;
        xkernel_core* p_kernel;
    };

    /************************
     * xcomm implementation *
     ************************/

    template <class T>
    inline void xcomm::on_message(T&& handler)
    {
        m_message_handler = std::forward<T>(handler);
    }

    template <class T>
    inline void xcomm::on_close(T&& handler)
    {
        m_close_handler = std::forward<T>(handler);
    }
}

#endif
