#pragma once

#ifndef MIGI_SYNC_UTILS_H
#define MIGI_SYNC_UTILS_H
#include "CoreMinimal.h"

class IMIGINNAdapter : public FNoncopyable
{
public:
	static void Install (size_t InSharedInputBufferSize, size_t InSharedOutputBufferSize) ;
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
	virtual void SynchronizeToNN (FRHICommandList & RHICmdList) = 0;
	// Insert a semaphore to wait for (on GPU) for the succeeding commands.
	virtual void SynchronizeFromNN (FRHICommandList & RHICmdList) = 0;

	// Get the shared memory among RHI and CUDA.
	virtual FRHIBuffer * GetSharedInputBuffer () const = 0;
	virtual FRHIBuffer * GetSharedOutputBuffer () const = 0;

	inline static size_t GetSharedInputBufferSize () {return SharedInputBufferSize;}
	inline static size_t GetSharedOutputBufferSize () {return SharedOutputBufferSize;}

	// Also disable move semantics.
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

	static size_t SharedInputBufferSize;
	static size_t SharedOutputBufferSize;
	bool bReady {};
};
#endif // MIGI_SYNC_UTILS_H