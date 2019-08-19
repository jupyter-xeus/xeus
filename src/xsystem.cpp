/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstdlib>

#ifdef WIN32
#include "Windows.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "xeus/xsystem.hpp"

namespace xeus
{
    std::string get_temp_directory_path_impl()
    {
#ifdef WIN32
        std::string tmp_prefix;
        char char_path[MAX_PATH];
        if(GetTempPathA(MAX_PATH, char_path))
        {
            tmp_prefix = char_path;
        }
        return tmp_prefix;
#else
        const char* tmpdir = std::getenv("TMPDIR");
        const char* tmp = std::getenv("TMP");
        const char* tempdir = std::getenv("TEMPDIR");
        const char* temp = std::getenv("TEMP");
        if(tmpdir != nullptr) return tmpdir;
        else if(tmp != nullptr) return tmp;
        else if(tempdir != nullptr) return tempdir;
        else if(temp != nullptr) return temp;
        else return "/tmp";
#endif
    }

    std::string get_temp_directory_path()
    {
        static const std::string path = get_temp_directory_path_impl();
        return path;
    }

    bool create_directory(const std::string& path)
    {
        std::size_t pos = path.rfind('/');
        if(pos != 0 && pos != std::string::npos)
        {
            create_directory(path.substr(0, pos));
        }
#ifdef WIN32
        return CreateDirectoryA(path.c_str(), NULL);
#else
        struct stat st = { 0 };
        int ret = 0;
        if(stat(path.c_str(), &st) == -1)
        {
            ret = mkdir(path.c_str(), 0700);
        }
        return ret == 0;
#endif
    }
}


