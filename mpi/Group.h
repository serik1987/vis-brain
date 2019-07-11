//
// Created by User on 30.06.2019.
//

#ifndef MPI2_GROUP_H
#define MPI2_GROUP_H

#include <initializer_list>
#include <vector>

#include "Communicator.h"
#include "exceptions.h"



namespace mpi{

    class Communicator;

    /**
     * Represents a group of the processes. The group doesn't belong to any communicator but it can be
     * used to create the communicator. General way to deal with groups:
     *
     * 1) create the group:
     * mpi::Group g0(old_communicator)
     *
     * 2) manipulate the group:
     * mpi::Group g1 = g0.exclude(...)
     *
     * 3) create communicator from the group:
     * mpi::Communicator new_comm = g1.createCommunicator()
     */
    class Group{
    private:
        const Communicator* comm;
        MPI_Group group;

        int gsize = -1;
        int grank = -1;

        template<typename V> int* to_c_array(const V& v, int& size) const{
            size = (int)v.size();
            int* cranks = new int[size];
            int i = 0;
            for (auto it = v.begin(); it != v.end(); ++it){
                cranks[i] = *it;
                i++;
            }
            return cranks;
        }

        int identityTest(const Group& other) const {
            int result;
            int errcode;
            if ((errcode = MPI_Group_compare(group, other.group, &result)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return result;
        }

        std::vector<int> getRange(int start, int stop, int step = 1) const{
            int N = (int)ceil((double)(stop-start)/step);
            std::vector<int> result(N);
            int i = start;
            for (int& x: result){
                x = i;
                i += step;
            }
            return result;
        }


    protected:
        Group(const Communicator* c, MPI_Group& g) {
            comm = c;
            group = g;
        }
    public:

        /**
         * Creates the group from the communicator pointer
         *
         * @param c the communicator pointer
         */
        Group(const Communicator* c);

        /**
         * Creates the group from communicator
         *
         * @param c - the communicator
         */
        Group(const Communicator& c): Group(&c) {};

        Group(const Group&) = delete;

        Group& operator=(const Group& other)  = delete;

        /**
         * Creates the group from the rvalue
         *
         * @param other the rvalue. The rvalue will be destroyed
         */
        Group(Group&& other){
            comm = other.comm;
            group = other.group;
            other.comm = nullptr;
            other.group = MPI_GROUP_NULL;
        }

        ~Group(){
            if (group == MPI_GROUP_NULL){

            } else {
                if (MPI_Group_free(&group) == MPI_ERR_ARG) {
                    std::cerr << "[ERROR] Unable to destroy the MPI group\n";
                }
            }
        }

        /**
         * Returns the group size
         *
         * @return the group size
         */
        int getSize(){
            if (gsize == -1){
                int errcode;
                if ((errcode = MPI_Group_size(group, &gsize)) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
            }
            return gsize;
        }

        /**
         * Returns the rank of the process in the group
         *
         * @return the rank
         */
        int getRank(){
            if (grank == -1){
                int errcode;
                if ((errcode = MPI_Group_rank(group, &grank)) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
            }
            return grank;
        }


        /**
         * Creates the new group that contains all processes in the current group but not the processes
         * mentioned as input arguments
         *
         * @tparam V the container type. Any class that contains begin() and end() methods
         * @param v list of processes to exclude
         * @return the new group
         */
        template<typename V> Group exclude(const V& v) const{
            int size;
            int* cranks = to_c_array(v, size);
            MPI_Group g_id;
            int errcode;
            if ((errcode = MPI_Group_excl(group, size, cranks, &g_id)) != MPI_SUCCESS){
               throw_exception(errcode);
            }
            Group g(comm, g_id);
            delete [] cranks;
            return g;
        }

        /**
         * Creates new group that contains all processes belonging to the current group lying outside some range
         *
         * @param start lower bound of the range
         * @param stop upper bound of the range
         * @param step step of the range
         * @return new group
         */
        Group exclude(int start, int stop, int step = 1) const {
            std::vector<int> range = getRange(start, stop, step);
            return exclude(range);
        }


        /**
         * Produces a group by reordering an existing group and taking only listed members
         *
         * @tparam V container that shall contain begin() and end() methods
         * @param v list of all processes to include
         * @return the new group
         */
        template<typename V> Group include(const V& v) const{
            int size;
            int* cranks = to_c_array(v, size);
            MPI_Group g_id;
            int errcode;
            std::cout << "MPI_Group_incl" << std::endl;
            if ((errcode = MPI_Group_incl(group, size, cranks, &g_id)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            Group g(comm, g_id);
            delete [] cranks;
            return g;
        }

        /**
         * Produces a new group that contains all processes within the certain range
         *
         * @param start lower bound of the range
         * @param stop upper bound of the range
         * @param step step of the range
         * @return the new group
         */
        Group include(int start, int stop, int step = 1) const{
            std::vector<int> range = getRange(start, stop, step);
            return include(range);
        }

        /**
         * Difference g1-g2 contains all processes that is included in g1 but not in g2
         *
         * @param other
         * @return new group
         */
        Group operator-(const Group& other) const{
            MPI_Group id;
            int errcode;
            if ((errcode = MPI_Group_difference(group, other.group, &id)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            Group g(comm, id);
            return g;
        }

        /**
         * g1 & g2 contains processes that belong both to g1 and to g2
         *
         * @param other
         * @return the new grou[
         */
        Group operator&(const Group& other) const{
            MPI_Group id;
            int errcode;
            if ((errcode = MPI_Group_intersection(group, other.group, &id)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            Group g(comm, id);
            return g;
        }

        /**
         * g1 | g2 contains all processes that belongs either to g1 or to g2
         *
         * @param other
         * @return the new group
         */
        Group operator|(const Group& other) const{
            MPI_Group id;
            int errcode;
            if ((errcode = MPI_Group_union(group, other.group, &id)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            Group g(comm, id);
            return g;
        }

        /**
         * Returns true if the group is identical to the other group
         *
         * @param other the other group
         * @return true if two groups contain the same processes arranged in the same order, false otherwise
         */
        bool isIdentical(const Group& other) const{
            /* Returns true if both groups contain the same processes in the same order */
            return identityTest(other) == MPI_IDENT;
        }

        /**
         * g1 == g2 if they contain the same processes
         *
         * @param other
         * @return true if two groups contain the same processes
         */
        bool operator==(const Group& other) const{
            /* Returns true if both groups contain the same processes */
            int result = identityTest(other);
            return result == MPI_IDENT || result == MPI_SIMILAR;
        }

        /**
         * g1 != g2 if the contain different processes
         *
         * @param other
         * @return
         */
        bool operator!=(const Group& other) const{
            /* Returns true if both groups contain different processes */
            return identityTest(other) == MPI_UNEQUAL;
        }

        /**
         * Creates the communicator that contains all processes belonging to the certain group
         *
         * @return a new communicated created
         */
        Communicator createCommunicator();
    };

}



#endif //MPI2_GROUP_H
