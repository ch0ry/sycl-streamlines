#!/bin/bash
# . /opt/intel/oneapi/2025.3/oneapi-vars.sh
# acpp streamlines.cpp hdf5_field_sycl.cpp -w --acpp-targets=hip -I/usr/include/hdf5/serial -DOLD_HEADER_FILENAME -DHDF_NO_NAMESPACE -DNO_STATIC_CAST -L/usr/lib/x86_64-linux-gnu/hdf5/serial /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_hl_cpp.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_cpp.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_hl.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5.a -lcrypto -lcurl -lpthread -lsz -lz -ldl -O2 -lm -o streamlines

acpp streamlines.cpp hdf5_field_sycl.cpp -w --acpp-targets=hip:gfx1200 \
  -I/usr/include/hdf5/serial \
  -L/usr/lib/x86_64-linux-gnu/hdf5/serial \
  -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5 \
  -lcrypto -lcurl -lpthread -lsz -lz -ldl -lm \
  -O3 -DNDEBUG -ffast-math -o streamlines

icpx -fsycl \
  -fsycl-targets=amdgcn-amd-amdhsa \
  -Xsycl-target-backend --offload-arch=gfx1200 \
  -Xsycl-target-backend "-options -cl-fast-relaxed-math" \
  -O3 -DNDEBUG \
  streamlines2.cpp hdf5_field_sycl.cpp \
  -I/usr/include/hdf5/serial \
  -L/usr/lib/x86_64-linux-gnu/hdf5/serial \
  -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5 \
  -lcrypto -lcurl -lpthread -lsz -lz -ldl -lm \
  -o streamlines

# ./streamlines --dt 0.002 --vtp 1
