#ifndef __cudautil_h
#define __cudautil_h

#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include <stdexcept>

// -------------------------------------------------------------------------

/// throw exception if a CUDA error was generated in the wrapped call
inline void cuda_check(dpct::err0 code)
{
    /*
    DPCT1000:5: Error handling if-stmt was detected but could not be rewritten.
    */
    if (code != 0)
        /*
        DPCT1009:6: SYCL reports errors using exceptions and does not use error
        codes. Please replace the "get_error_string_dummy(...)" with a real
        error-handling function.
        */
        /*
        DPCT1001:4: The statement could not be removed.
        */
        throw std::runtime_error(std::string("CUDA error: ") +
                                 dpct::get_error_string_dummy(code));
}

/// throw exception if a CUDA error occurred in the past
inline void cuda_check()
{
    /*
    DPCT1010:7: SYCL uses exceptions to report errors and does not use the error
    codes. The cudaGetLastError function call was replaced with 0. You need to
    rewrite this code.
    */
    cuda_check(0);
}

// -------------------------------------------------------------------------

#endif // __cudautil_h