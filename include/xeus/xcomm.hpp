/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XCOMM_HPP
#define XCOMM_HPP

#include <string>
#include <functional>
#include <map>
#include <utility>

#include <iostream>

#include "xguid.hpp"
#include "xmessage.hpp"

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

        using function_type = std::function<void (const xcomm&, const xmessage&)>;

        xtarget();
        xtarget(const std::string& name, const function_type& callback, xcomm_manager* manager);

        // name accessor
        std::string& name() & noexcept;
        const std::string& name() const & noexcept;
        std::string name() const && noexcept;

        void operator()(const xcomm& comm, const xmessage& request) const;

        void publish_message(const std::string&, xjson, xjson);
        void unregister_comm(xguid);

    private:

        std::string m_name;
        function_type m_callback;
        xcomm_manager* p_manager;

        friend class xcomm_manager;
    };

    /*********************
     * xcomm declaration *
     *********************/

    /**
     * @class xcomm
     * @brief Comm object.
     *
     * Instances of xcomm are lightweight stack-allocated objects which can
     * be cheaply instantiated and copied.
     *
     * xcomm objects cannot be directly instantiated or destroyed by the user, or only with the
     * methods `xinterpreter::create_comm` and `xinterpreter::delete_comm` respectively.
     *
     */
    class XEUS_API xcomm
    {
    public:

        xtarget& target() noexcept;
        const xtarget& target() const noexcept;

        void handle_message(const xmessage& request);
        void handle_close(const xmessage& request);
        //void on_message(callback);
        void send();

    private:

        friend class xcomm_manager;

        /**
         * Private constructors. May only be called by `kernel_core`.
         */
        xcomm();
        xcomm(xtarget* target, xguid id);

        /**
         * Private xcomm::open and xcomm::close. May only be called by `kernel_core`.
         */
        void open(xjson metadata, xjson data);
        void close(xjson metadata, xjson data);

        /**
         * Send comm message on iopub. Compose iopub message with specified data
         * and rebinds to target's publish_message
         * {
         *    "comm_id": m_id,
         *    "data": data
         * }
         */
        void publish_message(const std::string& msg_type, xjson metadata, xjson data);
        void publish_message(const std::string& msg_type, xjson metadata, xjson data, const std::string&);

        xtarget* p_target;
        xguid m_id;
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

        xcomm_manager(xkernel_core* kernel);

        using target_function_type = xtarget::function_type;

        void register_comm_target(const std::string& target_name, const target_function_type& callback);
        void unregister_comm_target(const std::string& target_name);

        void comm_open(const xmessage& request);
        void comm_close(const xmessage& request);
        void comm_msg(const xmessage& request);

        std::map<xguid, xcomm>& comms() & noexcept;
        const std::map<xguid, xcomm>& comms() const & noexcept;
        std::map<xguid, xcomm> comms() const && noexcept;

        xkernel_core& kernel() noexcept;
        const xkernel_core& kernel() const noexcept;

    private:

        xjson get_metadata() const;

        std::map<xguid, xcomm> m_comms;
        std::map<std::string, xtarget> m_targets;
        xkernel_core* p_kernel;
    };

    /**************************
     * xtarget implementation *
     **************************/

    inline xtarget::xtarget()
        : m_name(), m_callback(), p_manager(nullptr)
    {
    }

    inline xtarget::xtarget(const std::string& name, const function_type& callback, xcomm_manager* manager)
        : m_name(name), m_callback(callback), p_manager(manager)
    {
    }

    inline std::string& xtarget::name() & noexcept
    {
        return m_name;
    }

    inline const std::string& xtarget::name() const & noexcept
    {
        return m_name;
    }

    inline std::string xtarget::name() const && noexcept
    {
        return m_name;
    }

    inline void xtarget::operator()(const xcomm& comm, const xmessage& message) const
    {
        return m_callback(comm, message);
    }

    /************************
     * xcomm implementation *
     ************************/

    inline xtarget& xcomm::target() noexcept
    {
        return *p_target;
    }

    inline const xtarget& xcomm::target() const noexcept
    {
        return *p_target;
    }

    inline void xcomm::handle_message(const xmessage&)
    {
    }

    inline void xcomm::publish_message(const std::string& msg_type, xjson metadata, xjson data)
    {
        xjson content;
        content["comm_id"] = guid_to_hex(m_id);
        content["data"] = std::move(data);
        target().publish_message(msg_type, std::move(metadata), std::move(content));
    }

    inline void xcomm::publish_message(const std::string& msg_type, xjson metadata, xjson data,
                                       const std::string& target_name)
    {
        xjson content;
        content["comm_id"] = guid_to_hex(m_id);
        content["target_name"] = target_name;
        content["data"] = std::move(data);
        target().publish_message(msg_type, std::move(metadata), std::move(content));
    }

    inline xcomm::xcomm()
        : p_target(nullptr), m_id() 
    {
    }

    inline xcomm::xcomm(xtarget* target, xguid id)
        : p_target(target), m_id(id)
    {
    }

    inline void xcomm::open(xjson metadata, xjson data)
    {
        publish_message("comm_open", std::move(metadata), std::move(data), p_target->name());
    }

    inline void xcomm::close(xjson metadata, xjson data)
    {
        publish_message("comm_close", std::move(metadata), std::move(data));
    }

    /********************************
     * xcomm_manager implementation *
     ********************************/

    inline std::map<xguid, xcomm>& xcomm_manager::comms() & noexcept
    {
        return m_comms;
    }

    inline const std::map<xguid, xcomm>& xcomm_manager::comms() const & noexcept
    {
        return m_comms;
    }

    inline std::map<xguid, xcomm> xcomm_manager::comms() const && noexcept
    {
        return m_comms;
    }

}

#endif
