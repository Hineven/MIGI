#pragma once

#ifndef MIGI_SYNC_UTILS_H
#define MIGI_SYNC_UTILS_H
#include "CoreMinimal.h"

class IMIGINNAdapter
{
public:
	static void Install (size_t InSharedBufferSize) ;
	static IMIGINNAdapter * GetInstance ();
	// Called when the module shuts down.
	static void Clear ();
	// This function can only be called from the render thread.
	inline bool IsReady () const
	{
		check(IsInRenderingThread());
		return bReady;
	}
	// Modify RHI configurations to allow fine-grained CUDA synchronization.
	// Called before the initialization of GDynamicRHI creation, but after the loading phase of RHI module.
	virtual bool InstallRHIConfigurations () = 0;
	
	// Insert a semaphore here. Signal CUDA when the commands submitted are completed.
	virtual void SynchronizeToCUDA (FRHICommandListImmediate & RHICmdList) = 0;
	// Insert a semaphore to wait for (on GPU) for the succeeding commands.
	virtual void SynchronizeFromCUDA (FRHICommandListImmediate & RHICmdList) = 0;

	// Get the shared memory among RHI and CUDA.
	virtual FRHIBuffer * GetSharedBuffer () const = 0;

	IMIGINNAdapter (const IMIGINNAdapter &) = delete;
	IMIGINNAdapter & operator= (const IMIGINNAdapter &) = delete;
	IMIGINNAdapter (IMIGINNAdapter &&) = delete;
	IMIGINNAdapter & operator= (IMIGINNAdapter &&) = delete;
	
	virtual ~IMIGINNAdapter () = default;

	static FSimpleMulticastDelegate OnAdapterActivated;
	
protected:

	// This function is installed before PostInit
	static void TryActivate () ;
	
	virtual bool CanActivate () const = 0;
	
	// Activate the RHI-CUDA synchronization utility object for the active RHI.
	// Ensures that CUDA is loaded and CanActive returns true.
	virtual void Activate () = 0;
	IMIGINNAdapter () = default;

	inline static size_t SharedBufferSize {};
	bool bReady {};
private:
	static void TryActivatePostCUDAInit ();
};
#endif // MIGI_SYNC_UTILS_H