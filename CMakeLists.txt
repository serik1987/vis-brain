cmake_minimum_required(VERSION 3.14)
project(mpi2)

set(CMAKE_CXX_STANDARD 17)

add_executable(mpi2 main.cpp mpi/App.h mpi/exceptions.h mpi/Communicator.h mpi/Group.h mpi/Group.cpp mpi/Communicator.cpp mpi/CartesianCommunicator.cpp mpi/CartesianCommunicator.h mpi/Datatype.cpp mpi/Datatype.h mpi/Request.h mpi/Operation.h mpi/Intercommunicator.cpp mpi/Intercommunicator.h mpi/SuperIntercommunicator.h mpi/GraphCommunicator.cpp mpi/GraphCommunicator.h mpi/AbstractGraphItem.cpp mpi/AbstractGraphItem.h mpi/Graph.cpp mpi/Graph.h mpi/GraphItem.cpp mpi/GraphItem.h mpi/App.cpp sys/Semaphore.h sys/exceptions.h sys/system.cpp)

target_link_libraries(mpi2 pthread)

# These lines were added for compatibility with MS MPI
# include_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Include")
# link_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64")
# target_link_libraries(mpi2 "C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64\\msmpi.lib")