//
// Created by serik1987 on 13.07.19.
//

#ifndef MPI2_STATUS_H
#define MPI2_STATUS_H

#include "mpi.h"

namespace mpi {

    class Status {
    private:
        MPI_Status *statusPointer;

        void deleteStatusPointer(){
            if (statusPointer != MPI_STATUS_IGNORE && statusPointer != nullptr) {
                // std::cout << "STATUS DEALLOCATED\n";
                delete statusPointer;
            }
        };
    public:
        Status(){
            // std::cout << "STATUS ALLOCATED\n";
            statusPointer = new MPI_Status;
        };

        Status(MPI_Status* status): statusPointer(status) { // MPI_Status* -> Status
        };

        Status(const Status& other){ // copy constructor
            // std::cout << "COPY CONSTRUCTOR\n";
            statusPointer = new MPI_Status;
            *statusPointer = *other.statusPointer;
        }

        ~Status(){
            deleteStatusPointer();
        }

        operator MPI_Status*(){ // overload of (MPI_Status*)
            return statusPointer;
        }

        MPI_Status* operator&(){
            return statusPointer;
        }


        Status& operator=(MPI_Status* other){
            // std::cout << "ENVELOPING ASSIGNMENT\n";
            deleteStatusPointer();
            statusPointer = other;
            return *this;
        }

        Status& operator=(const Status& other){
            // std::cout << "COPY ASSIGNMENT\n";
            if (other.statusPointer != MPI_STATUS_IGNORE) {
                *statusPointer = *other.statusPointer;
            } else {
                deleteStatusPointer();
                statusPointer = MPI_STATUS_IGNORE;
            }
            return *this;
        }

        Status& operator=(Status&& other){
            // std::cout << "MOVE ASSIGNMENT\n";
            deleteStatusPointer();
            statusPointer = other.statusPointer;
            other.statusPointer = MPI_STATUS_IGNORE;
            return *this;
        }

        /**
         *
         * @return rank of the process which message has been received
         */
        int getSourceRank(){
            return statusPointer->MPI_SOURCE;
        }

        /**
         *
         * @return the tag which message has been received
         */
        int getSourceTag(){
            return statusPointer->MPI_TAG;
        }


        /**
         *
         * @return total number of items received
         */
        int getReceivedItems(MPI_Datatype dtype){
            int items;
            int errcode;
            if ((errcode = MPI_Get_count(statusPointer, dtype, &items)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return items;
        }


    };

}

#endif //MPI2_STATUS_H
