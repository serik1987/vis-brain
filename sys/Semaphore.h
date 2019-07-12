//
// Created by serik1987 on 12.07.19.
//

#ifndef MPI2_SEMAPHORE_H
#define MPI2_SEMAPHORE_H

#include <string>
#include <iostream>

#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "exceptions.h"

namespace sys {


    /**
     * The class represents the named semaphores and provides a simple access to them
     */
    class Semaphore {
    private:
        std::string semaphoreName;
        sem_t* semaphore;
    public:
        /**
         * Creating athe new semaphore.
         * The new semaphore is created an once. During the other calls of this methos the semaphore
         * opens. The initial value of the semaphore is 0
         *
         * @param sem_name unique name of the semaphore (without initial / symbol)
         * @param value initial value (default 0). This means that the value represents the number of
         * processes that shares the resource
         */
        Semaphore(std::string sem_name, int value = 1) : semaphoreName(sem_name) {
            if ((semaphore = sem_open(("/"+sem_name).c_str(), O_CREAT, 0666, value)) == SEM_FAILED){
                create_exception();
            }
        }

        Semaphore(const Semaphore& other) = delete;

        Semaphore(Semaphore&& other){
            semaphoreName = other.semaphoreName;
        }

        ~Semaphore(){
            if (sem_close(semaphore) != 0){
                std::cerr << "[ERROR] an error encountered during the semaphore closing\n";
            }
        }

        /**
         * Decrements the semaphore value is it is greater than zero.
         * If the value is zero the process locks until the other process will increment the value
         */
        void wait(){
            if (sem_wait(semaphore) == -1){
                create_exception();
            }
        }

        /**
         * If the semaphore is greater than zero it decrements the value and returns true
         * If the semaphore is zero it returns false
         *
         * @return (see above)
         */
        bool tryWait(){
            if (sem_trywait(semaphore) == -1){
                if (errno == EAGAIN){
                    return false;
                } else {
                    create_exception();
                }
            }
            return true;
        }

        /**
         * If the semaphore is greater than zero the function decrements its value
         * If the semaphore is zero the function waits for a certain time defined as ts
         * When nobody increases the ts value the function returns error
         *
         * @param ts
         */
        void timedWait(const struct timespec* ts){
            if (sem_timedwait(semaphore, ts) == -1){
                create_exception();
            }
        }

        /**
         * If the semaphore is locked by the processes this unlocks some of these processes
         * If the semaphore is not locked by any processes this returns zero
         */
        void post(){
            if (sem_post(semaphore) == -1){
                create_exception();
            }
        }
    };

}



#endif //MPI2_SEMAPHORE_H
