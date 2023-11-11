#pragma once

#include "CoreMinimal.h"
#include "MIGICUDAAdapter.h"

struct CUDA_DRIVER_API_FUNCTION_LIST;
class MIGICUDAAdapterD3D12State;

class FMIGICUDAAdapterD3D12 : public IMIGICUDAAdapter
{
public:
	virtual bool InstallRHIConfigurations() override;
	virtual void SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual void SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList) override;
	FMIGICUDAAdapterD3D12 () ;
	virtual ~FMIGICUDAAdapterD3D12() override;
	virtual CUstream GetCUDAStream() const override;
	virtual FRHIBuffer* GetSharedBuffer() const override;
	virtual void AllocateSharedBuffer(size_t InSharedBufferSize) override;
protected:
	virtual bool CanActivate () const override;
	virtual void Activate() override;
	void Initialize_RenderThread (FRHICommandListImmediate & RHICmd) ;
	void ReallocateSharedBuffer_RenderThread (FRHICommandListImmediate & RHICmd) ;
	FDelegateHandle RHIExtensionRegistrationDelegateHandle;
	// Prevent multiple extension requests.
	bool bExtensionsRequested {};
	TUniquePtr<MIGICUDAAdapterD3D12State> State;
};