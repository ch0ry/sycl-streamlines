FROM rocm/dev-ubuntu-24.04:7.2-complete

WORKDIR /test-program/code

COPY src/ /test-program/code/

WORKDIR /test-program

COPY --chmod=555 ./run.sh /test-program/run.sh 
COPY data/jet_v4.h5 /test-program/data/jet_v4.h5
COPY --chmod=555 ./parse-valgrind.py /test-program/parse-valgrind.py


RUN apt-get update &&\
  apt-get install -y --no-install-recommends libhdf5-dev\
  clang-20 libc++-dev libc++1 libc++abi-dev\
  libc++abi1 libclang-20-dev libclang1 liblldb-dev\
  libllvm-ocaml-dev libomp-20-dev libomp5-20 lld-20 lldb\
  llvm-20-dev llvm-20-runtime llvm-20 python3-clang-20 git \
  cmake valgrind ninja-build doxygen

RUN git clone https://github.com/AdaptiveCpp/AdaptiveCpp 

WORKDIR /test-program/AdaptiveCpp

# RUN mkdir build && cd build && cmake .. \
#   -DCMAKE_CXX_COMPILER=/usr/lib/llvm-20/bin/clang++ \
#   && make install
#
RUN cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DCMAKE_C_COMPILER=/usr/lib/llvm-20/bin/clang \
  -DCMAKE_CXX_COMPILER=/usr/lib/llvm-20/bin/clang++ \
  -DLLVM_DIR=/usr/lib/llvm-20/lib/cmake/llvm \
  -DClang_DIR=/usr/lib/llvm-20/lib/cmake/clang \
  -DOpenMP_C_FLAGS="-fopenmp" \
  -DOpenMP_C_LIB_NAMES="omp" \
  -DOpenMP_CXX_FLAGS="-fopenmp" \
  -DOpenMP_CXX_LIB_NAMES="omp" \
  -DOpenMP_omp_LIBRARY=/usr/lib/x86_64-linux-gnu/libomp.so.5

RUN ninja -C build
RUN ninja -C build install

RUN ldconfig

WORKDIR /test-program

ENTRYPOINT [ "./run.sh" ]






