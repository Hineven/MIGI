#include "MIGINNAdapterD3D12.h"

#include "MIGILogCategory.h"
#include "ID3D12DynamicRHI.h"
#include "MIGINN.h"

#include "Microsoft/COMPointer.h"

bool FMIGICUDAAdapterD3D12::InstallRHIConfigurations()
{
	// No need to install anything. Just check if the platform supports D3D
#ifdef WIN32
	return true;
#else
	return false;
#endif
}

class MIGICUDAAdapterD3D12State {
public:
	// Fence of the DX / CUDA versions
	// Remember to release this.
	TComPtr<ID3D12Fence> SharedFenceD3D12 {};

	// Shared memory
	// Use the RHI wrapper instead of raw D3D12 resource handle.
	//ID3D12Resource * SharedMemoryD3D12;
	FBufferRHIRef SharedMemoryD3D12 {};

	// The next shared fence value we're using. Incremented for one each time it's used.
	uint64 CurrentFenceValue {};
	
	void Destroy ()
	{
		if(SharedMemoryD3D12)
		{
			if(SharedMemoryD3D12.GetRefCount() != 1)
			{
				UE_LOG(MIGI, Warning, TEXT("Someone else rather than the adapter is holding the shared buffer reference!"
					"Just proceeds and ignore leaks."));
			}
			SharedMemoryD3D12 = nullptr;
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
	}

	~MIGICUDAAdapterD3D12State ()
	{
		Destroy();
	}
};

void FMIGICUDAAdapterD3D12::Initialize_RenderThread (FRHICommandListImmediate & RHICmd)
{
	auto D3D = GetDynamicRHI<ID3D12DynamicRHI>();
	auto Device = D3D->RHIGetDevice(0);

	// The Windows HANDLE of the shared fence for interop
	HANDLE SharedFenceHandle;
	// Shared fence initialization.
	{
		// Create a shared fence
		check(Device->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&State->SharedFenceD3D12)) == S_OK);

		auto SecurityAttributes = SECURITY_ATTRIBUTES{};
		check(Device->CreateSharedHandle(
			State->SharedFenceD3D12, &SecurityAttributes, GENERIC_ALL, nullptr,
			&SharedFenceHandle) == S_OK);

	}
	// Shared memory allocation
	
	// We need this buffer to be a storage buffer and shared.
	auto BufferDesc = FRHIBufferDesc (SharedBufferSizeToAllocate, 0, EBufferUsageFlags::Shared | EBufferUsageFlags::UnorderedAccess);
	auto BufferCreateInfo = FRHIResourceCreateInfo (TEXT("MIGI Shared Buffer"));
	// Buffer creation without initialization wont use RHICmd, anyway it's required.
	State->SharedMemoryD3D12 = D3D->RHICreateBuffer(RHICmd, BufferDesc, ERHIAccess::UAVMask, BufferCreateInfo);
	check(State->SharedMemoryD3D12.IsValid());

	// Create a Windows HANDLE of the shared buffer for interop
	HANDLE SharedMemoryHandle;
	auto SecurityAttributes = SECURITY_ATTRIBUTES{};
	check(Device->CreateSharedHandle(
		D3D->RHIGetResource(State->SharedMemoryD3D12), &SecurityAttributes, GENERIC_ALL, nullptr,
		&SharedMemoryHandle) == S_OK);

	MIGINNInitialize()

	// Destroy the Windows HANDLE.
	CloseHandle(SharedMemoryHandle);

	SharedBufferSize = SharedBufferSizeToAllocate;
	
	// Destroy the Windows HANDLE
	CloseHandle(SharedFenceHandle);
}

void FMIGICUDAAdapterD3D12::AllocateSharedBuffer(size_t InSharedBufferSize)
{
	SharedBufferSizeToAllocate = InSharedBufferSize;
	ENQUEUE_RENDER_COMMAND(MIGICUDAAdapterD3D12ReallocateSharedBuffer)(
		[this](FRHICommandListImmediate & RHICmd){ReallocateSharedBuffer_RenderThread(RHICmd);}
	);
}


bool FMIGICUDAAdapterD3D12::CanActivate() const
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

	// Check for the presence of CUDA module.
	auto bCUDAModule = FModuleManager::Get().IsModuleLoaded("CUDA");
	if(!bCUDAModule) return false;
	return true;
}
void FMIGICUDAAdapterD3D12::Activate()
{
	State = MakeUnique<MIGICUDAAdapterD3D12State>();
	// Enqueue the render command.
	ENQUEUE_RENDER_COMMAND(MIGICUDAAdapterD3D12Initialize)(
		[this](FRHICommandListImmediate & RHICmd)
		{
			Initialize_RenderThread(RHICmd);
			bReady = true;
		}
	);
	
	UE_LOG(MIGI, Display, TEXT("Successfully initialized CUDA adapter for D3D12."));
}

FMIGICUDAAdapterD3D12::FMIGICUDAAdapterD3D12() = default;

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
	auto CUDA = FCUDAModule::CUDA();
	checkCUDA(CUDA, cuSignalExternalSemaphoresAsync(
		&State->SharedFenceCUDA, &SignalParams,
		1, State->CUDAStream
	));
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
	auto CUDA = FCUDAModule::CUDA();
	checkCUDA(CUDA, cuWaitExternalSemaphoresAsync(&State->SharedFenceCUDA, &WaitParams, 1, State->CUDAStream));
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