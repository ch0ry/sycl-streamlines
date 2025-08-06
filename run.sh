#!/bin/bash
icpx -fsycl streamlines.cpp hdf5_field_sycl.cpp -I. -I/home/ch0ry/anaconda3/include /home/ch0ry/anaconda3/lib/libhdf5.a /home/ch0ry/anaconda3/lib/libhdf5_hl.a -L/home/ch0ry/anaconda3/lib -lz -ldl -lpthread -lm -lcurl -lcrypto -o streamlines
./streamlines --dt 0.002 --vtp 1
