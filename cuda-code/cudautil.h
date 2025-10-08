#ifndef __cudautil_h
#define __cudautil_h

#include <stdexcept>

// -------------------------------------------------------------------------

/// throw exception if a CUDA error was generated in the wrapped call
inline void cuda_check( cudaError_t code )
{
    if( code != cudaSuccess )
        throw std::runtime_error( std::string( "CUDA error: " ) + cudaGetErrorString(code) );
}

/// throw exception if a CUDA error occurred in the past
inline void cuda_check()
{
    cuda_check( cudaGetLastError() );
}

// -------------------------------------------------------------------------

#endif // __cudautil_h