//
// Created by User on 02.07.2019.
//

#ifndef MPI2_DATATYPE_H
#define MPI2_DATATYPE_H

#include <complex>
#include <iostream>
#include <vector>

#include "mpi.h"
#include "exceptions.h"

namespace mpi {

    extern int autocommits;


    /**
     * The general class for the datatypes.
     *
     * In order to transmit the data you have to tell the MPI machine what type is it. In order to do it,
     * you may use predefined constants MPI_INT, MPI_DOUBLE.
     *
     * However if you use an instance of this class, such instance will be transformed into MPI_Datatype implicitly
     *
     * Three ways you can use routines to transmit the information:
     * 1) comm.send(..., mpi_datatype_instance, ...)
     * 2) comm.send(..., instance_of_this_object, ...)
     * 3) commm.send(..., mpi::ScalarType<some_c_type>(), ...)
     * Example: template<typename T> void somefunc(...){
     * ...
     * comm.send(..., mpi::ScalarType<T>(), ...);
     * ...
     * }
     */
    class Datatype {

    protected:
        MPI_Datatype datatype;

    public:

        /**
         * Implicit cast MPI_Datatype -> mpi::Datatype
         *
         * @param dt - an instance of MPI_Datatype
         */
        Datatype(MPI_Datatype dt) : datatype(dt) {};
        virtual ~Datatype() {};

        /**
         * Implicit cast mpi::Datatype -> MPI_Datatype
         *
         * @return an instance of MPI_Datatype
         */
        virtual operator MPI_Datatype() { return datatype; };

    };

    /** By default, any C++ type transforms to the MPI_DATATYPE_NULL */
    template <typename T> class ScalarType: public Datatype{
    public:
        ScalarType(): Datatype(MPI_DATATYPE_NULL) {};
    };

    template<> class ScalarType<signed char>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_CHAR) {};
    };

    template<> class ScalarType<float>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_FLOAT) {};
    };

    template<> class ScalarType<double>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_DOUBLE) {};
    };

    template<> class ScalarType<long double>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_LONG_DOUBLE) {};
    };

    template<> class ScalarType<wchar_t>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_WCHAR) {};
    };

    template<> class ScalarType<short>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_SHORT) {};
    };

    template<> class ScalarType<int>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_INT) {};
    };

    template<> class ScalarType<long>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_LONG) {};
    };

    template<> class ScalarType<long long>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_LONG_LONG_INT) {};
    };

    template<> class ScalarType<unsigned char>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_UNSIGNED_CHAR) {};
    };

    template<> class ScalarType<unsigned short>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_UNSIGNED_SHORT) {};
    };

    template<> class ScalarType<unsigned int>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_UNSIGNED) {};
    };

    template<> class ScalarType<unsigned long>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_UNSIGNED_LONG) {};
    };

    template<> class ScalarType<unsigned long long>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_UNSIGNED_LONG_LONG) {};
    };

    template<> class ScalarType<std::complex<float>>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_C_COMPLEX) {};
    };

    template<> class ScalarType<std::complex<double>>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_C_DOUBLE_COMPLEX) {};
    };

    template<> class ScalarType<std::complex<long double>>: public Datatype{
    public:
        ScalarType(): Datatype(MPI_C_LONG_DOUBLE_COMPLEX) {};
    };







    /* Complex Datatypes */

    /**
     * A base class for all complex datatypes. A complex datatypes is a vector or a list that contains several
     * simple datatypes
     *
     * This is the abstract class
     */
    class ComplexDatatype: public Datatype{
    private:
        MPI_Aint extent;
        MPI_Aint lb;
        int size;
        MPI_Aint upperBound;
        bool extentCalculated = false;
        bool sizeCalculated = false;
        bool upperBoundCalculated = false;


        void get_extent(){
            int errcode;
            if ((errcode = MPI_Type_get_extent(datatype, &lb, &extent)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            extentCalculated = true;
        }

        void get_size(){
            int errcode;
            if ((errcode = MPI_Type_size(datatype, &size)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            sizeCalculated = true;
        }

        void get_upper_bound(){
            /*
            int errcode;
            if ((errcode = MPI_Type_ub(datatype, &upperBound)) != MPI_SUCCESS){
                switch (errcode){
                    case MPI_ERR_ARG: throw std::invalid_argument("getExtent/getLb");
                    case MPI_ERR_TYPE: throw invalid_datatype_exception();
                    default: throw unknown_exception();
                }
            }
             */
            upperBound = lb + extent;
            upperBoundCalculated = true;
        }
    protected:
        bool deletable = true;
        bool commited = false;
        bool autocommit = true;
    public:
        /**
         * For internal usage only
         */
        ComplexDatatype(): Datatype(MPI_DATATYPE_NULL) {};
        ComplexDatatype(MPI_Datatype) = delete;
        ComplexDatatype(const ComplexDatatype&) = delete;
        ComplexDatatype(ComplexDatatype&& other): Datatype(other.datatype){
            deletable = other.deletable;
            commited = other.commited;
            other.deletable = false;
            other.commited = false;
        }

        virtual const char* name() = 0;

        virtual ~ComplexDatatype() override{
            if (deletable){
                int errcode;
                if ((errcode = MPI_Type_free(&datatype)) != MPI_SUCCESS){
                    switch (errcode){
                        case MPI_ERR_TYPE:
                            std::cerr << "Error on MPI type delete: invalid type\n";
                            break;
                        case MPI_ERR_ARG:
                            std::cerr << "Error in MPI type delete: invalid argument\n";
                            break;
                        default:
                            std::cerr << "Error in MPI type delete: unknown argument\n";
                    }
                }
            }
        }

        /**
         * Commits the datatype
         *
         * Every datatype shall be commited before it can be used in sending and receiving routines
         * Commiting is performed by application of this function
         * When transforming to MPI_Datatype (u.e., immediately before the call of the data transmission functions)
         * the commit will occur implicitly
         *
         */
        virtual void commit(){
            int errcode;
            if ((errcode = MPI_Type_commit(&datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            commited = true;
        }

        /**
         * Casting mpi::ComplexDatatype -> MPI_Datatype with autocommit if autocommit is not disabled
         *
         * @return
         */
        virtual operator MPI_Datatype() override{
            if (!commited && autocommit){
                ++autocommits;
                commit();
            }
            return datatype;
        }

        /**
         * Disables autocommit
         */
        void disableAutocommit() { autocommit = false; }

        /**
         * Enables autocommit
         */
        void enableAutocommit() { autocommit = true; }

        /**
         * Returns the total memory engaged by an instance of this datatype
         *
         * @return the size in bytes
         */
        MPI_Aint getExtent(){
            if (!extentCalculated) get_extent();
            return extent;
        }

        /**
         * Returns the lower bound
         *
         * @return the lower bound in bytes
         */
        MPI_Aint getLowerBound(){
            if (!extentCalculated) get_extent();
            return lb;
        }

        /**
         * Returns the total size engaged by all useful information
         *
         * @return the total size in bytes
         */
        int getSize(){
            if (!sizeCalculated) get_size();
            return size;
        }

        /**
         * The upper bound of the datatype
         *
         * @return the upper bound in bytes
         */
        MPI_Aint getUpperBound(){
            if (!upperBoundCalculated) get_upper_bound();
            return upperBound;
        }
    };


    /**
     * A contiguous array. Corresponds to simple C++ array
     */
    class ContiguiousDatatype: public ComplexDatatype{
    public:
        ContiguiousDatatype() = delete;

        /**
         *
         * @param oldtype - the datatype of the array items
         * @param count - total number of items in the array
         */
        ContiguiousDatatype(MPI_Datatype oldtype, int count): ComplexDatatype(){
            int errcode;
            if ((errcode = MPI_Type_contiguous(count, oldtype, &datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            deletable = true;
            commited = false;
        }

        const char* name() override{
            return "contiguous datatype";
        }
    };


    /**
     * XXXXXXX+++XXXXXXX+++XXXXXXX+++
     * X = a data represented by oldtype document
     * + = empty space
     * count = number of periods
     * blocklength - number of XXXX per period
     * stride - number of XXX and +++ per period
     */
    class VectorDatatype: public ComplexDatatype{
    public:
        VectorDatatype() =  delete;
        VectorDatatype(MPI_Datatype oldtype, int blocklength, int stride, int count): ComplexDatatype() {
            int errcode;
            if ((errcode = MPI_Type_vector(count, blocklength, stride, oldtype, &datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            deletable = true;
            commited = false;
        }

        const char* name() override{
            return "vector datatype";
        }
    };

    /**
     * The same as VectorDatatype but the stride is given in bytes, not in element number
     */
    class HvectorDatatype: public ComplexDatatype{
    public:
        HvectorDatatype() = delete;
        HvectorDatatype(MPI_Datatype oldtype, int blocklength, MPI_Aint stride, int count): ComplexDatatype(){
            int errcode;
            if ((errcode = MPI_Type_create_hvector(count, blocklength, stride, oldtype, &datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        const char* name() override {
            return "hvector datatype";
        }
    };

    /**
     * Indexed datatype: the data contains count blocks. Size of n-th block is defined by blocklengths[n]
     * while its displacement is in displacements[n]
     */
    class IndexedDatatype: public ComplexDatatype{
    public:
        IndexedDatatype() = delete;
        IndexedDatatype(MPI_Datatype oldtype, const int blocklengths[], const int displacements[], int count)
            :ComplexDatatype(){
            int errcode;
            if ((errcode = MPI_Type_indexed(count, blocklengths, displacements, oldtype, &datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            deletable = true;
        }

        const char* name() override{
            return "indexed datatype";
        }
    };

    /** The same as IndexedDatatype but blocklengths are the same and represented as blocklength variable */
    class IndexedBlockDatatype: public ComplexDatatype{
    public:
        IndexedBlockDatatype() = delete;
        IndexedBlockDatatype(MPI_Datatype oldtype, int blocklength, int displacements[], int count): ComplexDatatype(){
            int errcode;
            if ((errcode = MPI_Type_create_indexed_block(count,blocklength,displacements, oldtype, &datatype))){
                throw_exception(errcode);
            }
        }

        const char* name()  override{
            return "indexed block";
        }
    };



    /** The datatype for simultaneous send of objects located at different places on the memory */
    class HindexedBlock: public ComplexDatatype {
    private:
        std::vector<MPI_Aint> sentdata;
        MPI_Datatype _oldtype;
        int _blocklength;
    public:
        HindexedBlock() = delete;

        HindexedBlock(MPI_Datatype oldtype, int blocklength, int sizeEstimate = 0)
                : ComplexDatatype(), _oldtype(oldtype), _blocklength(blocklength) {
            if (sizeEstimate != 0) {
                sentdata.reserve(sizeEstimate);
            }
            deletable = false;
        }

        const char *name() override {
            return "hindexed block";
        }

        void addData(const void *data) {
            int errcode;
            MPI_Aint address;
            if ((errcode = MPI_Get_address(data, &address)) != MPI_SUCCESS) {
                throw_exception(errcode);
            }
        }

        void commit() override{
            if (datatype == MPI_DATATYPE_NULL){
                int errcode;
                if ((errcode = MPI_Type_create_hindexed_block(
                        (int)sentdata.size(), _blocklength, &sentdata[0], _oldtype, &datatype
                        )) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
            }
            deletable = true;
            ComplexDatatype::commit();
        }

        ~HindexedBlock(){
            sentdata.~vector<MPI_Aint>();
        }

    };



    /** Creates the datatype for a certain structure
     *
     * @tparam T the structure type
     *
     * The way to use this datatype:
     * consider:
     * struct S { int a; double b; int c; };
     *
     * In order to create a datatype for this structure you shall use the following operators:
     * S demo;
     * StructureDatatype<S> struct_type(3); // 3 is total number of fields in the structure
     * struct_type.addField(demo.a);
     * struct_type.addField(demo.b);
     * struct_type.addField(demo.c);
     */
    template <typename T> class StructureDatatype: public ComplexDatatype{
    private:
        std::vector<MPI_Datatype> types;
        std::vector<MPI_Aint> displacements;
        MPI_Aint base_displacement;
        std::vector<int> blocklengths;

        int current;
    public:
        StructureDatatype() = delete;
        StructureDatatype(int fieldsize): ComplexDatatype(), types(fieldsize), displacements(fieldsize),
            current(0), blocklengths(fieldsize){
            deletable = false;
        };

        const char* name(){
            return "structure datatype";
        }

        template <typename Q> void addField(Q& q){
            MPI_Aint displacement;
            int errcode;
            if ((errcode = MPI_Get_address(&q, &displacement)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            if (current == 0){
                base_displacement = displacement;
                displacements[0] = 0;
            } else {
                displacements[current] = displacement - base_displacement;
            }
            types[current] = ScalarType<Q>();
            blocklengths[current] = 1;
            current++;
        }

        void commit() override{
            if (datatype == MPI_DATATYPE_NULL){
                int errcode;
                if ((errcode = MPI_Type_create_struct((int)types.size(), &blocklengths[0],
                        &displacements[0], &types[0], &datatype))){
                    throw_exception(errcode);
                }
                if (getExtent() != sizeof(T)){
                    MPI_Datatype dtype  = datatype;
                    int errcode;
                    if ((errcode = MPI_Type_create_resized(dtype, 0, sizeof(T), &datatype)) != MPI_SUCCESS){
                        throw_exception(errcode);
                    }
                    if ((errcode = MPI_Type_free(&dtype)) != MPI_SUCCESS){
                        throw_exception(errcode);
                    }

                }
            }
            ComplexDatatype::commit();
        }

    };



    /**
     * Single value of oldtype surrounded by data gaps
     * lb - gap before the data belonging to the oldtype
     * extent = gap before + data + gap after
     * All gaps have values in bytes
     */
    class ResizedDatatype: public ComplexDatatype{
    public:
        ResizedDatatype() = delete;

        ResizedDatatype(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent): ComplexDatatype(){
            int errcode;
            if ((errcode = MPI_Type_create_resized(oldtype, lb, extent, &datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        const char* name(){
            return "resized datatype";
        }
    };


    /**
     * The datatype is used for filetype to save n-dimensional distributed matrices as a file.
     *
     * In order to save n-dimensional matrix do the following steps:
     * 1. Distribute it in the following way:
     * +---------------+---------------+
     * | Process 0 job | Process 1 job |
     * +---------------+---------------+
     * | Process 2 job | Process 3 job |
     * +---------------+---------------+
     *
     * 2. define the following variables:
     * size - total number of processes participated in writing the matrix.
     * rank - rank of the current process
     * ndims - number of dimensions in the matrix
     * gsizes[k] - number of elements of dtype along k-th dimension
     * distribs[k] - distribution of elements along k-th dimension
     *      MPI_DISTRIBUTE_BLOCK - block distribution (first, items belonging to the first process go, then,
     *      there are items belonging to the second process, then, to the third etc.)
     *      MPI_DISTRIBUTE_CYCLIC - cyclic(M) distribution which means, that the first M values belong to the
     *      first process, the next M values belong to the second process, ..., then again: first M values are for
     *      first process, next M values are from the second, ...
     * dargs[k] - for cyclic distribution, the value of M from the previous description, for block distribution
     * always MPI_DISTRIBUTE_DFLT_ARGS
     * psizes[k] - size of the process grid along each dimension
     * order - use the default value if you create stand-alone C/C++ application
     *
     * 3. Create the file type:
     * mpi::ArrayDatatype ftype(oldtype, size, rank, ndims, psizes, distribs, dargs, psizes);
     *
     * 4. Set an appropriate file view:
     * file.setView(0, oldtype, ftype);
     *
     * 5. Write the array into the file:
     * file.writeAll(matrix, local_size, oldtype);
     */
    class ArrayDatatype: public ComplexDatatype{
    public:
        ArrayDatatype() = delete;

        ArrayDatatype(MPI_Datatype oldtype, int size, int rank, int ndims, const int gsizes[], const int distribs[],
                const int dargs[], const int psizes[], int order = MPI_ORDER_C){
            int errcode;
            if ((errcode = MPI_Type_create_darray(size, rank, ndims, gsizes, distribs, dargs, psizes, order,
                    oldtype, &datatype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        const char* name(){
            return "darray datatype";
        }
    };




}


#endif //MPI2_DATATYPE_H
