/*
 * Project MIGINN : MIGINNCudaHelper.h
 * Created: 2023/11/13
 * This program is unlicensed. See LICENSE for more.
 */

#ifndef MIGINN_MIGINNCUDAHELPER_CUH
#define MIGINN_MIGINNCUDAHELPER_CUH

#pragma once
#include <stdexcept>
#include <cuda_runtime.h>

#define checkCUDA(x) do { \
auto CUDAStatus = (x); \
if(CUDAStatus != cudaSuccess) \
{ \
	throw std::runtime_error{"CUDA Error at " + std::string{__FILE__} + ":" + std::to_string(__LINE__)}; \
} \
} while(false)



#endif //MIGINN_MIGINNCUDAHELPER_CUH
