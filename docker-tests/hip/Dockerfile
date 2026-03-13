FROM rocm/dev-ubuntu-24.04:7.2-complete

WORKDIR /test-program

# Copy codebase
COPY ../../hip-code/ /test-program/code/

# Copy input data and test script
COPY --chmod=555 ./run.sh /test-program/run.sh 
COPY ./jet_v4.h5 /test-program/data/jet_v4.h5

# Install dependencies
RUN apt-get update &&\
  apt-get install -y --no-install-recommends libhdf5-dev

WORKDIR /test-program/code/build
RUN hipcc ../streamlines.cu.hip -o ../../streamlines -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial -O3 -lhdf5 -lhdf5_cpp

WORKDIR /test-program

ENTRYPOINT [ "./run.sh" ]






