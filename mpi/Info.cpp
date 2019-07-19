//
// Created by serik1987 on 19.07.19.
//

#include <iostream>
#include "Info.h"

#define MPI_INFO_VALUE_LENGTH 256


namespace mpi{

    inline void create_info(MPI_Info& info){
        int errcode;
        if ((errcode = MPI_Info_create(&info)) != MPI_SUCCESS){
            throw_exception(errcode);
        }
        std::cout << "MPI INFO CREATED\n";
    }

    inline void destroy_info(MPI_Info& info){
        if (info != MPI_INFO_NULL) {
            if (MPI_Info_free(&info) != MPI_SUCCESS) {
                std::cerr << "[ERROR] occured during the execution of mpi::Info::~Info" << std::endl;
            }
            std::cout << "MPI INFO DESTROYED\n";
        }
    }

    Info::Info(){
        create_info(info);
    }

    Info::~Info(){
        destroy_info(info);
    }

    std::string Info::get(const std::string &key) {
        int flag;
        char value[MPI_INFO_VALUE_LENGTH];
        int errcode;
        if ((errcode = MPI_Info_get(info, key.c_str(), MPI_INFO_VALUE_LENGTH, value, &flag)) != MPI_SUCCESS){
            throw_exception(errcode);
        }
        if (!flag){
            throw mpi_info_key_not_found();
        }
        return std::string(value);
    }

    std::ostream &operator<<(std::ostream &os, Info &info) {
        bool first = true;
        for (auto hint: info){
            if (!first){
                std::cout << ", ";
            }
            std::cout << hint.key << " = " << hint.value;
            first = false;
        }
        return os;
    }

    Info::Info(Info &other): Info() {
        for (auto hint: other){
            set(hint);
        }
    }

    Info &Info::operator=(Info &other) {
        destroy_info(info);
        create_info(info);
        for (auto hint: other){
            set(hint);
        }
        return *this;
    }

    Info &Info::operator=(Info &&other) {
        destroy_info(info);
        info = other.info;
        other.info = MPI_INFO_NULL;
        return *this;
    }

}