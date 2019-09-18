//
// Created by serik1987 on 18.09.19.
//

#ifndef MPI2_FOLDER_H
#define MPI2_FOLDER_H


#include <string>
#include <sys/types.h>
#include <dirent.h>


namespace sys {

/**
 * An auxiliary class that allows to apply C++ routines to list among all the directory.
 *
 * The best way to list among all files in the directory:
 * for (auto info: sys::Folder(dirname){
 * ...
 * }
 *
 * The info field will have the same type as dirent struct (see man readdir for more info)
 */
    class Folder {
    private:
        std::string name;
        DIR *dir;

        void open();
        void close();

    public:

        /**
         * std::string -> Folder
         *
         * @param folder_name name of the folder
         */
        Folder(const std::string &folder_name);
        Folder(const Folder& other);
        Folder(Folder&& other);

        ~Folder();

        /**
         *
         * @return directory name
         */
        const std::string& getDirectoryName() const{
            return name;
        }

        operator std::string(){
            return name;
        }

        /**
         *
         * @return the directory pointer
         */
        DIR* getDirectoryPointer() const{
            return dir;
        }

        Folder& operator=(const Folder& other);


        /**
         * Iterator that represents a single file in the directory
         */
        class Iterator{
        private:
            struct dirent* d;
            DIR* D;
            bool eod = false;

            void next();

        public:
            Iterator(const Folder& parent, bool empty);
            Iterator(const Iterator& other)=delete;
            Iterator(Iterator&& other){
                d = other.d;
                D = other.D;
            }

            Iterator& operator=(const Iterator& other) = delete;
            Iterator& operator=(Iterator&& other) {
                d = other.d;
                D = other.D;
            }

            Iterator& operator++(){
                next();
                return *this;
            }

            const dirent& operator*(){
                if (d == NULL){
                    throw std::bad_alloc();
                }
                return *d;
            }

            dirent* operator->(){
                if (d == NULL){
                    throw std::bad_alloc();
                }
                return d;
            }

            bool operator==(const Iterator& other){
                return d == other.d;
            }

            bool operator!=(const Iterator& other){
                return d != other.d;
            }
        };

        /**
         *
         * @return the iterator corresponding to the first element within the folder
         */
        Iterator begin(){
            return Iterator(*this, false);
        }

        /**
         *
         * @return the iterator corresponding to the last element within the folder
         */
        Iterator end(){
            return Iterator(*this, true);
        }

    };

}


#endif //MPI2_FOLDER_H
