#pragma once

#include "CudaModule.h"

#define checkCUDA(x, y) do { \
auto CUDAStatus = (x.y); \
if(CUDAStatus != CUDA_SUCCESS) \
{ \
	const char * ErrorString; \
	const char * ErrorName; \
	x.cuGetErrorString(CUDAStatus, &ErrorString); \
	x.cuGetErrorName(CUDAStatus, &ErrorName); \
	UE_LOG(MIGI, Error, TEXT("CUDA Error [%s]: %s"), ANSI_TO_TCHAR(ErrorName), ANSI_TO_TCHAR(ErrorString)); \
	check(false); \
} \
} while(false)

class FCUDAContextGuard
{
public:
	inline FCUDAContextGuard (CUcontext ctx)
	{
		FCUDAModule::CUDA().cuCtxPushCurrent(ctx);
	}
	inline ~FCUDAContextGuard()
	{
		FCUDAModule::CUDA().cuCtxPopCurrent(nullptr);
	}
};
