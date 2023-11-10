#include "MIGICUDAAdapterD3D12.h"

#include "MIGILogCategory.h"
#include "MIGIModule.h"
#include "ID3D12DynamicRHI.h"
#include "D3D12Resources.h"

#include "CudaWrapper.h"
#include "D3D12RHIPrivate.h"

bool FMIGICUDAAdapterD3D12::InstallRHIConfigurations()
{
	// No need to install anything. Just check if the platform supports D3D
#ifdef WIN32
	return true;
#else
	return false;
#endif
}

class MIGICUDAAdapterD3D12State
{
public:

	// Fence of the DX / CUDA versions
	TUniquePtr<ID3D12Fence> SharedFenceD3D12 {};
	CUexternalSemaphore SharedFenceCUDA {};

	// Shared memory
	CUexternalMemory SharedMemoryCUDA {};
	// Use the RHI wrapper instead of raw D3D12 resource handle.
	//ID3D12Resource * SharedMemoryD3D12;
	TRefCountPtr<FD3D12Buffer> SharedMemoryD3D12 {};

	// The next shared fence value we're using. Incremented for one each time it's used.
	uint64 CurrentFenceValue {};

	// Non-blocking CUDA strea,
	CUstream CUDAStream {};
	

	void Initialize (size_t InSharedBufferSize)
	{
		auto D3D = GetDynamicRHI<ID3D12DynamicRHI>();
		auto Device = D3D->RHIGetDevice(0);

		// Shared fence initialization.
		{
			// Create a shared fence
			ID3D12Fence * TempFencePtr {};
			check(Device->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&TempFencePtr)) == S_OK);
			SharedFenceD3D12.Reset(TempFencePtr);
			
			// TODO do we need a windows HANDLE for interop?
			
			// Export this fence tu CUDA
			auto CUDAExternalSemaphoreHandleDesc = CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC {
				.type = CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE,
				.handle = {.win32 = {.handle = SharedFenceD3D12.Get(), .name = nullptr}},
				.flags = 0
			};
			check(cuImportExternalSemaphore(&SharedFenceCUDA, &CUDAExternalSemaphoreHandleDesc) == CUDA_SUCCESS);
		}

		// Shared memory allocation
		{

			auto FD3D = dynamic_cast<FD3D12DynamicRHI*>(D3D);
			check(FD3D);
			// We need this buffer to be a storage buffer and shared.
			auto BufferDesc = FRHIBufferDesc (InSharedBufferSize, 0, EBufferUsageFlags::Shared | EBufferUsageFlags::UnorderedAccess);
			auto BufferCreateInfo = FRHIResourceCreateInfo (nullptr);
			SharedMemoryD3D12 = FD3D->CreateD3D12Buffer(nullptr, BufferDesc, ERHIAccess::UAVMask, BufferCreateInfo);
			check(SharedMemoryD3D12.IsValid());
			
			// TODO do we need a windows HANDLE for interop?
			
			// CUDA: Import external memory from D3D12.
			CUDA_EXTERNAL_MEMORY_HANDLE_DESC CUDAExternalMemoryHandleDesc = {
				.type =  CU_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE,
				// ID3DResource is wrapped 2 times.
				.handle = { .win32 =  {.handle = SharedMemoryD3D12->GetResource()->GetResource(), .name =  nullptr}},
				.size = SharedMemoryD3D12->GetSize(),
				// TODO what is a dedicated allocation
				// Is this a Vulkan concept??
				.flags = 0 //CUDA_EXTERNAL_MEMORY_DEDICATED
			};
			check(cuImportExternalMemory(&SharedMemoryCUDA, &CUDAExternalMemoryHandleDesc) == CUDA_SUCCESS);
		}
		// CUDA stream
		{
			// Non blocking stream will not be affected by the NULL stream.
			check(cuStreamCreate(&CUDAStream, CU_STREAM_NON_BLOCKING) == CUDA_SUCCESS);
		}
	}
	void Destroy ()
	{
		if(CUDAStream) {
			cuStreamSynchronize(CUDAStream);
			check(cuStreamDestroy_v2(CUDAStream) == CUDA_SUCCESS);
			CUDAStream = nullptr;
		}
		// Destroy CUDA resources.
		if(SharedMemoryCUDA)
		{
			check(cuDestroyExternalMemory(SharedMemoryCUDA) == CUDA_SUCCESS);
			SharedMemoryCUDA = nullptr;
		}
		if(SharedFenceCUDA)
		{
			check(cuDestroyExternalSemaphore(SharedFenceCUDA) == CUDA_SUCCESS);
			SharedFenceCUDA = nullptr;
		}
		// Destroy D3D12 resources.
		if(SharedMemoryD3D12)
		{
			if(SharedMemoryD3D12.GetRefCount() != 1)
			{
				UE_LOG(MIGI, Warning, TEXT("Someone else rather than the adapter is holding the shared buffer reference!"
					"Just proceeds and ignore leaks."));
			}
		}
		SharedMemoryD3D12 = nullptr;
		SharedFenceD3D12.Reset();
	}

	~MIGICUDAAdapterD3D12State ()
	{
		Destroy();
	}
	
};

bool FMIGICUDAAdapterD3D12::TryActivate()
{
	// D3D12 needs no activation, so we just do some checking here.
	
	// This function is bound to the PostConfiguration loading stage.
	// Check the validity of RHI
	if(GDynamicRHI == nullptr)
	{
		return false;
	}
	auto RHIName = GDynamicRHI->GetName();
	if(!IsRHID3D12())
	{
		return false;
	}

	// Create states required for synchronization.
	State = MakeUnique<MIGICUDAAdapterD3D12State>();
	State->Initialize(FMIGIModule::Get().GetSharedBufferSize());
	
	UE_LOG(MIGI, Display, TEXT("Successfully activated RHI-CUDA synchronization utilities for D3D12: %s"), RHIName);
	return true;
}

FMIGICUDAAdapterD3D12::~FMIGICUDAAdapterD3D12()
{
	FModuleManager::Get().OnModulesChanged().Remove(
		RHIExtensionRegistrationDelegateHandle
	);
	RHIExtensionRegistrationDelegateHandle.Reset();
}

void FMIGICUDAAdapterD3D12::SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList)
{
	auto D3D = GetDynamicRHI<ID3D12DynamicRHI>();
	State->CurrentFenceValue ++;
	// Queue a D3D command to wait for the fence.
	D3D->RHIWaitManualFence(RHICmdList, State->SharedFenceD3D12.Get(), State->CurrentFenceValue);
	// Submit a CUDA command to signal the fence.
	auto SignalParams = CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS {.params = {.fence = {.value = State->CurrentFenceValue}}};
	cuSignalExternalSemaphoresAsync(
		&State->SharedFenceCUDA, &SignalParams,
		1, State->CUDAStream
	);
	// It's free to submit any synchronized D3D commands now.
	// TODO: No memory barriers required?
}


void FMIGICUDAAdapterD3D12::SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList)
{
	auto D3D = GetDynamicRHI<ID3D12DynamicRHI>();
	// Signal the shared fence upon computation work done.
	State->CurrentFenceValue ++;
	D3D->RHISignalManualFence(RHICmdList, State->SharedFenceD3D12.Get(), State->CurrentFenceValue);
	auto WaitParams = CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS {.params = {.fence = {.value =  State->CurrentFenceValue}}};
	// Stall the CUDA stream until D3D semaphore is shot.
	cuWaitExternalSemaphoresAsync(&State->SharedFenceCUDA, &WaitParams, 1, State->CUDAStream);
	// It's free to submit any synchronized CUDA commands now.
}

CUstream FMIGICUDAAdapterD3D12::GetCUDAStream() const
{
	return State->CUDAStream;
}

FRHIBuffer* FMIGICUDAAdapterD3D12::GetSharedBuffer() const
{
	return State->SharedMemoryD3D12;
}