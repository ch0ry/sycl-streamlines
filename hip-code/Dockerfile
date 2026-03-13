FROM rocm/dev-ubuntu-24.04:7.2-complete

WORKDIR /test-program

# Copy codebase
COPY src/ /test-program/code/

# Copy input data and test script
COPY --chmod=555 ./run.sh /test-program/run.sh 
COPY data/jet_v4.h5 /test-program/data/jet_v4.h5
COPY parse-valgrind.py /test-program/parse-valgrind.py

# Install dependencies
RUN apt-get update &&\
  apt-get install -y --no-install-recommends libhdf5-dev valgrind

WORKDIR /test-program/code

# RUN hipcc streamlines.cu.hip -o ../streamlines -O3 -ffast-math -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial -Wl,-rpath,/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5 -lcrypto -lcurl -lpthread -lsz -lz -ldl -lm
RUN 
WORKDIR /test-program

ENTRYPOINT [ "./run.sh" ]






