/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_HISTORY_MANAGER_HPP
#define XEUS_HISTORY_MANAGER_HPP

#include <string>
#include <vector>


namespace xeus
{

    class xhistory_manager
    {
    public:

        using history_type = std::vector<std::string>;

        xhistory_manager();
        virtual ~xhistory_manager() = default;

        xhistory_manager(const xhistory_manager&) = delete;
        xhistory_manager& operator=(const xhistory_manager&) = delete;

        xhistory_manager(xhistory_manager&&) = delete;
        xhistory_manager& operator=(xhistory_manager&&) = delete;

        void configure();
        void store_inputs(int line_num, const std::string& code);
        // void store_output(int line_num, const std::string& output);
        history_type get_tail(int n, bool raw, bool output);
        history_type get_range(int session, int start, int stop, bool raw, bool output);
        history_type search(const std::string& pattern, bool raw, bool output, int n, bool unique);

    private:

        virtual void configure_impl() = 0;
        virtual void store_inputs_impl(int line_num, const std::string& code) = 0;
        // virtual void store_output_impl(int line_num, const std::string& output) = 0;
        virtual history_type get_tail_impl(int n, bool raw, bool output) = 0;
        virtual history_type get_range_impl(int session, int start, int stop, bool raw, bool output) = 0;
        virtual history_type search_impl(const std::string& pattern, bool raw, bool output, int n, bool unique) = 0;

    };

}

#endif
