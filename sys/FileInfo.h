//
// Created by serik1987 on 27.08.19.
//

#ifndef MPI2_FILEINFO_H
#define MPI2_FILEINFO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "exceptions.h"
#include <string>

namespace sys {

    /**
     * Provides file information revealed by the usage of stat command
     * Also, provides some auxiliary operations
     */
    class FileInfo{
    private:
        struct stat info;
        std::string filename;
        bool pathAvailable;
    public:
        /**
         *
         * @param filename - filename to refer to
         * @param linkInfo - true if one need to provide information about the link, false if one need to provide
         * information about the file
         */
        FileInfo(const char* fn, bool linkInfo = false): filename(fn), pathAvailable(true){
            if (linkInfo){
                if (lstat(fn, &info) != 0){
                    create_exception();
                }
            }
            if (!linkInfo){
                if (stat(fn, &info) != 0){
                    create_exception();
                }
            }
        }

        FileInfo(int fd): pathAvailable(false){
            if (fstat(fd, &info) != 0){
                create_exception();
            }
        }

        /**
         *
         * @return ID of device containing file
         */
        dev_t getDeviceId(){
            return info.st_dev;
        }

        /**
         *
         * @return Inode number
         */
        ino_t getInodeNumber(){
            return info.st_ino;
        }

        /**
         *
         * @return file type and mode
         */
        mode_t getMode(){
            return info.st_mode;
        }

        /**
         *
         * @return true if the file is a directory
         */
        bool isDir(){
            return S_ISDIR(info.st_mode);
        }

        /**
         *
         * @return number of hard links
         */
        nlink_t getHardLinkNumber(){
            return info.st_nlink;
        }

        /**
         *
         * @return UID of the file owner
         */
        uid_t getOwnerUid(){
            return info.st_uid;
        }

        /**
         *
         * @return GID of the file owner
         */
        gid_t getOwnerGid(){
            return info.st_gid;
        }

        /**
         *
         * @return returns the device id for the special file
         */
        dev_t getCorrespondingDeviceId(){
            return info.st_rdev;
        }


        /**
         *
         * @return file size in bytes
         */
        off_t getFileSize(){
            return info.st_size;
        }

        /**
         *
         * @return returns the time of the last access
         */
        timespec getLastAccessTime(){
            return info.st_atim;
        }

        /**
         *
         * @return the last modification time
         */
        timespec getLastModificationTime(){
            return info.st_mtim;
        }

        /**
         *
         * @return the time where file has been created
         */
        timespec getCreateTime(){
            return info.st_ctim;
        }

        std::fstream openStream(std::ios_base::openmode mode){
            return std::fstream(filename, mode);
        }

    };
}

#endif //MPI2_FILEINFO_H
