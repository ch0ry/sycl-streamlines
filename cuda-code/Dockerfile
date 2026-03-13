FROM nvidia/cuda:12.6.1-base-ubuntu24.04

WORKDIR /test-program

COPY ./streamlines /test-program/

COPY --chmod=555 ./run.sh /test-program/run.sh 

COPY ./jet_v4.h5 /test-program/data/jet_v4.h5

RUN apt-get update &&\
  apt-get install -y --no-install-recommends libhdf5-dev

ENTRYPOINT [ "./run.sh" ]






