//
// Created by serik1987 on 19.07.19.
//

#ifndef MPI2_INFO_H
#define MPI2_INFO_H

#include "mpi.h"
#include "exceptions.h"

#define MPI_INFO_KEY_LENGTH 256


namespace mpi {

    /**
     * Hints for MPI implementation
     */
    class Info {

    private:
        MPI_Info info;
        int keyNumber = -1;

    public:

        /**
         * Creates new empty hints
         */
        Info();

        ~Info();

        /**
         * Implicit type conversion MPI_Info -> mpi::Info
         *
         * @param info
         */
        Info(MPI_Info other_info): info(other_info) {};


        Info(Info& other);

        Info(Info&& other){
            info = other.info;
            other.info = MPI_INFO_NULL;
        }

        Info& operator=(Info& other);

        Info& operator=(Info&& other);

        /**
         * Conversion mpi::Info -> MPI_Info
         *
         * @return an instance of MPI_Info
         */
        operator MPI_Info(){
            return info;
        }

        /**
         *
         * @return the p
         */
        MPI_Info& getContent() { return info; }

        /**
         * Sets some implementation hint
         *
         * @param key the hint key
         * @param value the hint value
         */
        void set(const std::string& key, const std::string& value){
            int errcode;
            if ((errcode = MPI_Info_set(info, key.c_str(), value.c_str())) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            keyNumber = -1;
        }

        /**
         * Gets some implementation hint
         *
         * @param key the hint key
         * @return the hint itself
         */
        std::string get(const std::string& key);


        /**
         *
         * @return the total number of all implementation hints
         */
        int getKeyNumber(){
            if (info == MPI_INFO_NULL){
                return 0;
            }
            if (keyNumber == -1){
                int errcode;
                if ((errcode = MPI_Info_get_nkeys(info, &keyNumber)) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
            }
            return keyNumber;
        };

        /**
         *
         * @param n
         * @return the name of the key for the n-th implementation hint
         */
        std::string getKey(int n){
            int errcode;
            char key[MPI_INFO_KEY_LENGTH];
            if ((errcode = MPI_Info_get_nthkey(info, n, key)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return std::string(key);
        }

        /**
         * key - the hint key
         * value - the hint value
         */
        struct Hint{
            std::string key;
            std::string value;
        };

        /**
         *
         * @param n
         * @return the n-th hint (the structure contains both key and value)
         */
        Hint getHint(int n){
            Hint hint;
            hint.key = getKey(n);
            hint.value = get(hint.key);
            return hint;
        }

        /**
         * A synonym for getHint(...)
         *
         * @param n
         * @return
         */
        Hint operator[](int n){
            return getHint(n);
        }

        /**
         * Sets some implementation hint
         *
         * @param hint the hint as mpi::Info::Hint structure
         */
        void set(const Hint& hint){
            set(hint.key, hint.value);
        }

        /**
         * The iterator for the MPI hint
         */
        class Iterator{
        private:
            Info* reference;
            int index;
            Hint hintReference;
        public:
            /**
             *
             * @param ref the Info object itself
             * @param idx the index to the object
             */
            Iterator(Info* ref, int idx): reference(ref), index(idx) {};

            /**
             *
             * @return the number of the hint
             */
            int getHintNumber() { return index; }

            /**
             *
             * @return the hint key
             */
            std::string getHintKey() { return reference->getKey(index); }

            /**
             *
             * @return the hint value
             */
            std::string getHintValue() { return reference->getHint(index).value; }

            /**
             *
             * @return the hint itself
             */
            Hint getHint() { return reference->getHint(index); }

            /**
             * Alias for getHint()
             *
             * @return
             */
            Hint operator*() { return getHint(); }

            /**
             * Alias for &getHint()
             *
             * @return
             */
            Hint* operator->(){
                hintReference = getHint();
                return &hintReference;
            }

            Iterator& operator++(){
                index++;
                return *this;
            }

            Iterator& operator--(){
                index--;
                return *this;
            }

            Iterator operator++(int){
                Iterator new_iterator = *this;
                index++;
                return new_iterator;
            }

            Iterator operator--(int){
                Iterator new_iterator = *this;
                index--;
                return new_iterator;
            }

            bool operator==(const Iterator& other){
                return reference == other.reference && index == other.index;
            }

            bool operator!=(const Iterator& other){
                return reference != other.reference || index != other.index;
            }
        };

        Iterator begin(){
            return Iterator(this, 0);
        }

        Iterator end(){
            return Iterator(this, getKeyNumber());
        }

        friend std::ostream& operator<<(std::ostream& os, Info& info);
    };

}


#endif //MPI2_INFO_H
