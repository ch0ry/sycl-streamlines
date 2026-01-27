set breakpoint pending on
set pagination off
set print pretty on
set print elements 0


# set amdgpu precise-memory on
maint set internal-error backtrace on
maint set internal-warning backtrace on


set environment HIP_LAUNCH_BLOCKING=1
set environment HIP_ENABLE_DEFERRED_LOADING=0

set args --dt 0.002 --vtp 1

break streamlines.cpp:297
break streamlines.cpp:108
# break hdf5_field_sycl.cpp:59
# break array3d_sycl.h:124
