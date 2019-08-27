//
// Created by serik1987 on 27.08.19.
//

#include <iostream>
#include <fstream>
#include <pwd.h>
#include "auxiliary.h"
#include "exceptions.h"
#include "FileInfo.h"

namespace sys {

    void create_empty_file(const std::string& filename){
        std::string file_path = get_file_path(filename);
        create_empty_dir(file_path);
        std::ofstream file(filename, std::ios_base::out);
        if (file.fail()){
            create_exception();
        }
        file.close();
    }

    std::string get_file_path(const std::string& filename){
        auto pos = filename.rfind('/');
        if (pos == std::string::npos){
            return ".";
        }
        if (pos == 0){
            return "/";
        }
        return filename.substr(0, pos);
    }

    void create_empty_dir(const std::string& pathname){
        try {
            FileInfo info(pathname.c_str());
            if (!info.isDir()) {
                throw directory_not_exists();
            }
        } catch (file_not_exists& e){
            std::string parent_dir = get_file_path(pathname);
            create_empty_dir(parent_dir);
            create_dir(pathname);
        }
    }

    std::string get_home_directory(){
        struct passwd *pw = getpwuid(getuid());
        return pw->pw_dir;
    }

    void create_dir(const std::string& pathname){
        if (mkdir(pathname.c_str(), 0755) != 0){
            create_exception();
        }
    }

}