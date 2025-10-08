project( cuda-streamlines )
cmake_minimum_required( VERSION 3.7 )

set( CMAKE_CXX_STANDARD 17 )

# required software packages
find_package( CUDA 8.0 REQUIRED )
find_package( HDF5 REQUIRED COMPONENTS C )

# set CUDA NVCC options for modern C++
list( APPEND CUDA_NVCC_FLAGS "--expt-extended-lambda" )
list( APPEND CUDA_NVCC_FLAGS "-arch=sm_86" )

# compile main program, both versions

cuda_add_executable( streamlines_stream
    nrrd_field.cu
    streamlines-stream_test.cu
)

cuda_add_executable( streamlines 
    nrrd_field.cu
    streamlines.cu 
)

# direcciones para encontrar los incluidos del proyecto
target_include_directories( streamlines PUBLIC ${HDF5_INCLUDE_DIRS} )
target_include_directories( streamlines_stream PUBLIC ${HDF5_INCLUDE_DIRS} )

# direcciones para encontrar los enlazados del proyecto
target_link_directories( streamlines PUBLIC ${HDF5_LIBRARY_DIRS} )
target_link_directories( streamlines_stream PUBLIC ${HDF5_LIBRARY_DIRS} )

# ${Teem_LIBRARY_DIRS}

# librerias a enlazar
target_link_libraries( streamlines ${HDF5_LIBRARIES} )
target_link_libraries( streamlines_stream ${HDF5_LIBRARIES} )

# Das Ende por ahora

