FROM rocm/dev-ubuntu-24.04:7.2-complete

WORKDIR /test-program

COPY ./streamlines /test-program/
COPY --chmod=555 ./run.sh /test-program/run.sh 
COPY ./jet_v4.h5 /test-program/data/jet_v4.h5

RUN apt-get update &&\
  apt-get install -y --no-install-recommends libhdf5-dev\
  clang-format clang-tidy clang-tools\
  clang clangd libc++-dev libc++1 libc++abi-dev\
  libc++abi1 libclang-dev libclang1 liblldb-dev\
  libllvm-ocaml-dev libomp-dev libomp5 lld lldb\
  llvm-dev llvm-runtime llvm python3-clang git \
  cmake

RUN git clone https://github.com/AdaptiveCpp/AdaptiveCpp 

WORKDIR /test-program/AdaptiveCpp

RUN mkdir build && cd build && cmake .. && make install

RUN ldconfig

WORKDIR /test-program

ENTRYPOINT [ "./run.sh" ]






