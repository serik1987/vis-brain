//
// Created by serik1987 on 13.07.19.
//

#ifndef MPI2_FILE_H
#define MPI2_FILE_H

#include <string>

#include "Communicator.h"
#include "exceptions.h"
#include "Status.h"


namespace mpi {


/**
 * A basic class for providing I/O operations using MPI routines
 *
 * Opening modes:
 * MPI_MODE_RDONLY              read only
 * MPI_MODE_WRONLY              write only
 * MPI_MODE_CREATE              create the file if it doesn't exist
 * MPI_MODE_EXCL                make a error on trying to create a file that has already exists
 * MPI_MODE_DELETE_ON_CLOSE     delete the file on close
 * MPI_MODE_APPEND              the initial position of all file pointers is at the end of the file
 * MPI_MODE_SEQUENTIAL          file will be accessed sequentially
 */
    class File {
    private:
        MPI_File handle;
        const Communicator *comm;
        MPI_Offset fileSize;
        bool fileSizeDefined = false;
    public:

        enum Datarep {native, internal, external32};


        /**
         * Creates an empty object. An empty object can't be used for reading or writing
         * but it can be used for assigning the rvalue
         */
        File() : handle(MPI_FILE_NULL), comm(nullptr) {};

        /**
         *  Opens a file and creates an object
         *
         * @param c communicator assigned to the file
         * @param filename filename (char* or std::string instance)
         * @param amode mode (see below)
         * @param info MPI_Info instance
         *
         * MPI_MODE_RDONLY              read only
         * MPI_MODE_WRONLY              write only
         * MPI_MODE_CREATE              create the file if it doesn't exist
         * MPI_MODE_EXCL                make a error on trying to create a file that has already exists
         * MPI_MODE_DELETE_ON_CLOSE     delete the file on close
         * MPI_MODE_APPEND              the initial position of all file pointers is at the end of the file
         * MPI_MODE_SEQUENTIAL          file will be accessed sequentially
         */
        File(const Communicator &c, std::string filename, int amode, MPI_Info info = MPI_INFO_NULL)
        : handle(MPI_FILE_NULL), comm(nullptr){
            int errcode;
            if ((errcode = MPI_File_open(*c, filename.c_str(), amode, info, &handle)) != MPI_SUCCESS) {
                std::cout << errcode << "\n";
                throw_exception(errcode);
            }
            comm = &c;
        }

        File(const File &other) = delete;

        File(File&& other) noexcept{
            std::cout << "MOVE CONSTRUCTOR\n";
            handle = other.handle;
            comm = other.comm;
            other.handle = MPI_FILE_NULL;
            other.comm = nullptr;
        }

        ~File(){
            if (handle != MPI_FILE_NULL){
                if ((MPI_File_close(&handle)) != MPI_SUCCESS){
                    std::cerr << "[ERROR] Closing MPI file was failed\n";
                }
            }
        }

        File& operator=(const File& other) = delete;

        File& operator=(File&& other) noexcept{
            handle = other.handle;
            comm = other.comm;
            other.handle = MPI_FILE_NULL;
            other.comm = nullptr;
            return *this;
        }

        /**
         * Turns the file pointer to a certain position
         *
         * @param offset pointer position
         * @param whence reference for the pointer position
         * MPI_SEEK_SET (default) offset is a position relatively to the beginning
         * MPI_SEEK_CUR           offset is a position relatively to the current position
         * MPI_SEEK_END           offset is a position relatively to the end
         */
        void seek(MPI_Offset offset, int whence = MPI_SEEK_SET){
            int errcode;
            if ((errcode = MPI_File_seek(handle, offset, whence)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         *
         * @return the file size in bytes
         */
        MPI_Offset getSize(){
            if (!fileSizeDefined){
                int errcode;
                if ((errcode = MPI_File_get_size(handle, &fileSize)) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
                fileSizeDefined = true;
            }
            return fileSize;
        }

        /**
         *
         * @return the current position of the file pointer
         */
        MPI_Offset getPosition(){
            MPI_Offset position;
            int errcode;
            if ((errcode = MPI_File_get_position(handle, &position)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return position;
        }

        /**
         * Reads the data from the file
         *
         * @param buf the buffer where all read data will be placed
         * @param count number of items to read
         * @param dtype datatype of read items
         * @return reading status (see mpi::Status for details)
         */
        mpi::Status read(void* buf, int count, MPI_Datatype dtype){
            mpi::Status result;
            int errcode;
            if ((errcode = MPI_File_read(handle, buf, count, dtype, &result)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return result;
        }

        /**
         * Reads the data from the certain position
         *
         * @param offset this certain position
         * @param buf (see read(...) for details)
         * @param count (see read(...) for details)
         * @param dtype (see read(...) for details)
         * @return (see read(...) for details)
         */
        mpi::Status readAt(MPI_Offset offset, void* buf, int count, MPI_Datatype dtype){
            mpi::Status status;
            int errcode;
            if ((errcode = MPI_File_read_at(handle, offset, buf, count, dtype, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
        }

        /**
         * Writes the data to the file
         *
         * @param buf information to write
         * @param count number of items to write
         * @param dtype datatype of written items
         * @return mpi::Status object
         */
        mpi::Status write(const void* buf, int count, MPI_Datatype dtype){
            mpi::Status result;
            int errcode;
            if ((errcode = MPI_File_write(handle, buf, count, dtype, &result)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return result;
        }

        /**
         * Wties the data to the file at a certain position
         *
         * @param offset this certain position
         * @param buf buffer containing data to write
         * @param count number of items to write
         * @param dtype type of the written items
         * @return writing status
         */
        mpi::Status writeAt(MPI_Offset offset, const void* buf, int count, MPI_Datatype dtype){
            mpi::Status status;
            int errcode;
            if ((errcode = MPI_File_write_at(handle, offset, buf, count, dtype, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
        }



        /**
         * Sets the file view. This means that it sets the following options
         *
         * @param disp size of the beginning of the file that is unread by the process
         * @param etype datatype used to calculate the item size. Please not the getPosition and seek deal with
         * values expressed in terms of number of items. etype defines the type of such numbers
         * @param filetype type that defines the scope of the file available to the read by this process
         * @param datarep on of the following values:
         * File::native - native implementation. No transformation will be occured. This will work properly
         * on homogeneous clusters only
         * File::internal - internal representation. The values were transformed to the format dependent on
         * a certain MPI implementation.
         * File::external32 - external representation. The values will be transformed to implementation-independent
         * format
         * @param info hints for the MPI implementation
         */
        void setView(MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype,
                Datarep datarep = native, MPI_Info info = MPI_INFO_NULL){
            std::string datarep_name;
            switch (datarep){
                case native:
                    datarep_name = "native";
                    break;
                case internal:
                    datarep_name = "internal";
                    break;
                case external32:
                    datarep_name = "external32";
                    break;
            }
            int errcode;
            if ((errcode = MPI_File_set_view(handle, disp, etype, filetype,
                    datarep_name.c_str(), info)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }
    };

}


#endif //MPI2_FILE_H
