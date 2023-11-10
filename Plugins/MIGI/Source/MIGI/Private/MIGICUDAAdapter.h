#pragma once

#ifndef MIGI_SYNC_UTILS_H
#define MIGI_SYNC_UTILS_H
#include "CoreMinimal.h"
#include "cuda.h"

class IMIGICUDAAdapter
{
public:
	static void InstallForAllRHIs () ;
	static IMIGICUDAAdapter * GetInstance ();
	// Called when the module shuts down.
	static void Clear ();
	// Modify RHI configurations to allow fine-grained CUDA synchronization.
	// Called before the initialization of GDynamicRHI creation, but after the loading phase of RHI module.
	virtual bool InstallRHIConfigurations () = 0;
	// Insert a semaphore here. Signal CUDA when the commands submitted are completed.
	virtual void SynchronizeToCUDA (FRHICommandListImmediate & RHICmdList) = 0;
	// Insert a semaphore to wait for (on GPU) for the succeeding commands.
	virtual void SynchronizeFromCUDA (FRHICommandListImmediate & RHICmdList) = 0;

	// Get the synchronized CUDA stream.
	virtual CUstream GetCUDAStream () const = 0;
	// Get the shared memory among RHI and CUDA.
	virtual FRHIBuffer * GetSharedBuffer () const = 0;

	virtual ~IMIGICUDAAdapter () = default;
protected:
	// Activate the RHI-CUDA synchronization utility object for the active RHI. Return true if successful.
	virtual bool TryActivate () = 0;
	IMIGICUDAAdapter () = default;
};
#endif // MIGI_SYNC_UTILS_H