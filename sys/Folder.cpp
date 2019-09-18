//
// Created by serik1987 on 18.09.19.
//

#include <iostream>
#include <dirent.h>
#include "Folder.h"
#include "exceptions.h"

namespace sys{


    Folder::Folder(const std::string &folder_name): name(folder_name), dir(nullptr) {
        open();
    }

    Folder::~Folder(){
        if (dir != nullptr) {
            close();
        }
    }

    Folder::Folder(const sys::Folder &other) {
        name = other.name;
        open();
    }



    void Folder::open(){
        std::cout << "Directory initialization...\n";
        if ((dir = opendir(name.c_str())) == NULL){
            create_exception();
        }
    }

    void Folder::close(){
        std::cout << "directory destruction\n";
        if (closedir(dir) == -1){
            create_exception();
        }
    }

    Folder& Folder::operator=(const Folder& other){
        close();
        name = other.name;
        open();
    }




    Folder::Iterator::Iterator(const sys::Folder &parent, bool empty) {
        D = parent.dir;
        d = NULL;
        if (!empty){
            next();
        }
    }

    void Folder::Iterator::next(){
        if (eod){
            throw std::range_error("Directory end was finished");
        }
        d = readdir(D);
        if (errno != 0){
            create_exception();
        }
        if (d == NULL){
            eod = true;
        }
    }


}
