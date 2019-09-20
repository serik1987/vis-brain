cmake_minimum_required(VERSION 3.14)
project(mpi2)

set(CMAKE_CXX_STANDARD 17)

add_executable(vis-brain main.cpp mpi/App.h mpi/exceptions.h mpi/Communicator.h mpi/Group.h mpi/Group.cpp mpi/Communicator.cpp mpi/CartesianCommunicator.cpp mpi/CartesianCommunicator.h mpi/Datatype.cpp mpi/Datatype.h mpi/Request.h mpi/Operation.h mpi/Intercommunicator.cpp mpi/Intercommunicator.h mpi/SuperIntercommunicator.h mpi/GraphCommunicator.cpp mpi/GraphCommunicator.h mpi/AbstractGraphItem.cpp mpi/AbstractGraphItem.h mpi/Graph.cpp mpi/Graph.h mpi/GraphItem.cpp mpi/GraphItem.h mpi/App.cpp sys/Semaphore.h sys/exceptions.h sys/system.cpp mpi/exceptions.cpp mpi/File.cpp mpi/File.h mpi/Status.h mpi/Info.cpp mpi/Info.h param/Engine.cpp param/Engine.h param/exceptions.h compile_options.h sys/FileInfo.h sys/auxiliary.cpp sys/auxiliary.h param/Object.cpp param/Object.h param/Loadable.h Application.cpp Application.h exceptions.h sys/Folder.cpp sys/Folder.h log/Logger.h log/UserLogger.cpp log/UserLogger.h log/Logger.cpp log/SystemLogger.cpp log/SystemLogger.h log/exceptions.h log/Engine.cpp log/Engine.h log/output.h)

target_link_libraries(vis-brain pthread /home/serik1987/v8/v8/out.gn/x64.release.sample/obj/libv8_monolith.a)

include_directories(/home/serik1987/v8/v8/include)

# These lines were added for compatibility with MS MPI
# include_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Include")
# link_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64")
# target_link_libraries(mpi2 "C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64\\msmpi.lib")