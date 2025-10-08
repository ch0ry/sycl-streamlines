#ifndef __cudautil_h
#define __cudautil_h

#include <stdexcept>

// -------------------------------------------------------------------------

/// throw exception if a CUDA error was generated in the wrapped call
inline void cuda_check( hipError_t code )
{
    if( code != hipSuccess )
        throw std::runtime_error( std::string( "CUDA error: " ) + hipGetErrorString(code) );
}

/// throw exception if a CUDA error occurred in the past
inline void cuda_check()
{
    cuda_check( hipGetLastError() );
}

// -------------------------------------------------------------------------

#endif // __cudautil_h