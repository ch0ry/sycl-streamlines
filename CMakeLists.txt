cmake_minimum_required( VERSION 3.7 )
project( sycl-streamlines )

set( CMAKE_CXX_STANDARD 17 )

set(CMAKE_CXX_COMPILER acpp)
# required software packages
find_package( HDF5 REQUIRED COMPONENTS C )

# Source files
set(SOURCES
    floatn.hpp
    array3d_sycl.h
    hdf5_field_sycl.cpp
    hdf5_field_sycl.h
    streamlines.cpp
)

# compile main program, both versions

add_executable( streamlines ${SOURCES})

# direcciones para encontrar los incluidos del proyecto
target_include_directories( streamlines PUBLIC ${HDF5_INCLUDE_DIRS} )

# direcciones para encontrar los enlazados del proyecto
target_link_directories( streamlines PUBLIC ${HDF5_LIBRARY_DIRS} )

# ${Teem_LIBRARY_DIRS}

# librerias a enlazar
target_link_libraries( streamlines ${HDF5_LIBRARIES} )

target_compile_options(streamlines PRIVATE -Wall -Wextra)
# Das Ende por ahora

