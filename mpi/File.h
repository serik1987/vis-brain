//
// Created by serik1987 on 13.07.19.
//

#ifndef MPI2_FILE_H
#define MPI2_FILE_H

#include <string>

#include "Communicator.h"
#include "exceptions.h"
#include "Status.h"
#include "Info.h"


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
         * The same as read(...) but does it non-blockingly
         *
         * @param buf the same as in read(...)
         * @param count the same as in read(...)
         * @param dtype the same as in read(...)
         * @return substitution into mpi::Request and mpi::Requests
         */
        MPI_Request iread(void* buf, int count, MPI_Datatype dtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_File_iread(handle, buf, count, dtype, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }


        /**
         * The same as read(...) but moves the file pointer for all processes simultaneously while
         * reading by the certain process
         *
         * @param buf the same as in read(...)
         * @param count the same as in read(...)
         * @param dtype the same as in read(...)
         * @return the same as in read(...)
         */
        mpi::Status readShared(void* buf, int count, MPI_Datatype dtype){
            mpi::Status status;
            int errcode;
            if ((errcode = MPI_File_read_shared(handle, buf, count, dtype, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
        }


        /**
         * The same as readShared(...) but does it non-blockingly
         *
         * @param buf the same as readShared(...)
         * @param count the same as readShared(...)
         * @param dtype the same as readShared(...)
         * @return the substitution into mpi::Request or mpi::Requests
         */
        MPI_Request ireadShared(void* buf, int count, MPI_Datatype dtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_File_iread_shared(handle, buf, count, dtype, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }



        /**
         *  Collective implementation of read(...). 'Collective' means that the program execution at this point
         *  may be suspended in order to decline number of readouts
         *
         * @param buf see read(...) for details
         * @param count see read(...) for details
         * @param dtype see read(...) for details
         * @return see read(...) for details
         */
        mpi::Status readAll(void* buf, int count, MPI_Datatype dtype){
            mpi::Status result;
            int errcode;
            if ((errcode = MPI_File_read_all(handle, buf, count, dtype, &result)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return result;
        }


        /**
         * The same as readAll(...) but just starts the collective reading and prepares for this
         *
         * @param buf see readAll(...) for details
         * @param count see readAll(...) for details
         * @param dtype see readAll(...) for details
         */
        void readAllBegin(void* buf, int count, MPI_Datatype dtype){
            int errcode;
            if ((errcode = MPI_File_read_all_begin(handle, buf, count, dtype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }


        /**
         * Finish the processes started by readAllBegin and may suspend the process execution for such
         * a purpose
         *
         * @param buf the buffer passed during the readAllBegin(...) execution
         * @return the mpi::Status object
         */
        Status readAllEnd(void* buf){
            int errcode;
            Status status;
            if ((errcode = MPI_File_read_all_end(handle, buf, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
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
         * The same as readAt(...) but does it non-blockingly
         *
         * @param offset the same as in readAt(...)
         * @param buf the same as in readAt(...)
         * @param count the same as in readAt(...)
         * @param dtype the same as in readAt(...)
         * @return substitution into mpi::Request or mpi::Requests
         */
        MPI_Request ireadAt(MPI_Offset offset, void* buf, int count, MPI_Datatype dtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_File_iread_at(handle, offset, buf, count, dtype, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
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
         * The same as write(...) but does it non-blockingly
         *
         * @param buf the same as in write(...)
         * @param count the same as in write(...)
         * @param dtype the same as in write(...)
         * @return the data to be substituted into mpi::Request or mpi::Requests (see reference on these classes
         * for details)
         */
        MPI_Request iwrite(const void* buf, int count, MPI_Datatype dtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_File_iwrite(handle, buf, count, dtype, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }





        /**
         * The same as write(...) but provides non-blocking option of it. 'Nonblocking" means that the function
         * nay suspend the process execution at this line in order to decline total number of requests to the
         * memory storage
         *
         * @param buf see write(...) for details
         * @param count see write(...) for details
         * @param dtype see write(...) for details
         * @return see write(...) for details
         */
        mpi::Status writeAll(const void* buf, int count, MPI_Datatype dtype){
            mpi::Status status;
            int errcode;
            if ((errcode = MPI_File_write_all(handle, buf, count, dtype, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
        }



        /**
         * The same as writeAll(...) but just starts the job performed in parallel and doesn't suspend the process
         *
         * @param buf the same as in writeAll(...)
         * @param count the same as in writeAll(...)
         * @param dtype the same as in writeAll(...)
         */
        void writeAllBegin(const void* buf, int count, MPI_Datatype dtype){
            int errcode;
            if ((errcode = MPI_File_write_all_begin(handle, buf, count, dtype)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }



        /**
         * Finishes the process started by writeAllEnd
         *
         * @param buf the argument passed to writeAllBegin(...)
         * @return the mpi::Status object
         */
        Status writeAllEnd(const void* buf){
            Status status;
            int errcode;
            if ((errcode = MPI_File_write_all_end(handle, buf, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
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
         * The same as writeAt(...) but does it non-blockingly
         *
         * @param offset the same as in writeAt(...)
         * @param buf the same as in writeAt(...)
         * @param count the same as in writeAt(...)
         * @param dtype the same as in writeAt(...)
         * @return the substitution into mpi::Request or mpi::Requests
         */
        MPI_Request iwriteAt(MPI_Offset offset, const void* buf, int count, MPI_Datatype dtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_File_iwrite_at(handle, offset, buf, count, dtype, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }


        /**
         * the same as write but each new call to the file shifts the pointer by a certain value
         *
         * @param buf the same as in write(...)
         * @param count the same as in write(...)
         * @param dtype the same as in write(...)
         * @return the same as in write(...)
         */
        mpi::Status writeShared(const void* buf, int count, MPI_Datatype dtype){
            mpi::Status status;
            int errcode;
            if ((errcode = MPI_File_write_shared(handle, buf, count, dtype, &status)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return status;
        }




        /**
         * the same as writeShared(...) but does it non-blockingly
         *
         * @param buf the same as writeShared(...)
         * @param count the same as writeShared(...)
         * @param dtype the same as writeShared(...)
         * @return a substitution into mpi::Request or mpi::Requests
         */
        MPI_Request iwriteShared(const void* buf, int count, MPI_Datatype dtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_File_iwrite_shared(handle, buf, count, dtype, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
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

        /**
         *
         * @return the info for the file
         */
        Info getInfo(){
            Info info;
            int errcode;
            if ((errcode = MPI_File_get_info(handle, &info.getContent())) != MPI_SUCCESS){
                return errcode;
            }
            return info;
        }


        /**
         * Sets the file operation as 'atomic'. The operations are 'atomic' if the data written by some process
         * are guaranteed to be read by another process
         */
        void setAtomic(){
            int errcode;
            if ((errcode = MPI_File_set_atomicity(handle, 1)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * Finishes all writing processes done by all files. Shall be overlapped by mpi::Communicator::barrier function
         */
        void sync(){
            int errcode;
            if ((errcode = MPI_File_sync(handle)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }
    };

}


#endif //MPI2_FILE_H
