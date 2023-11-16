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
	FBufferRHIRef SharedNNInputBufferD3D12 {};
	FBufferRHIRef SharedNNOutputBufferD3D12 {};

	// The next shared fence value we're using. Incremented for one each time it's used.
	uint64 NextFenceValue {1};
	
	void Destroy ()
	{
		if(SharedNNInputBufferD3D12)
		{
			if(SharedNNInputBufferD3D12.GetRefCount() != 1)
			{
				UE_LOG(MIGI, Warning, TEXT("Someone else rather than the adapter is holding the shared input buffer reference!"
					"Just proceeds and ignore leaks."));
			}
			SharedNNInputBufferD3D12 = nullptr;
		}
		if(SharedNNOutputBufferD3D12)
		{
			if(SharedNNOutputBufferD3D12.GetRefCount() != 1)
			{
				UE_LOG(MIGI, Warning, TEXT("Someone else rather than the adapter is holding the shared output buffer reference!"
	"Just proceeds and ignore leaks."));
			}
			SharedNNOutputBufferD3D12 = nullptr;
		}
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
	auto BufferDesc1 = FRHIBufferDesc (SharedInputBufferSize, 4,
		EBufferUsageFlags::Shared | EBufferUsageFlags::UnorderedAccess
		| EBufferUsageFlags::StructuredBuffer | EBufferUsageFlags::ShaderResource);
	auto BufferCreateInfo1 = FRHIResourceCreateInfo (TEXT("MIGI Shared Input Buffer"));
	// Buffer creation without initialization wont use RHICmd, anyway it's required by UE RHI interface.
	// We have to place this logic inside render thread.
	State->SharedNNInputBufferD3D12 = D3D->RHICreateBuffer(RHICmd, BufferDesc1, ERHIAccess::UAVMask, BufferCreateInfo1);
	check(State->SharedNNInputBufferD3D12.IsValid());
	
	auto BufferDesc2 = FRHIBufferDesc (SharedInputBufferSize, 4, 
		EBufferUsageFlags::Shared | EBufferUsageFlags::UnorderedAccess
		| EBufferUsageFlags::StructuredBuffer | EBufferUsageFlags::ShaderResource);
	auto BufferCreateInfo2 = FRHIResourceCreateInfo (TEXT("MIGI Shared Output Buffer"));
	State->SharedNNOutputBufferD3D12 = D3D->RHICreateBuffer(RHICmd, BufferDesc2, ERHIAccess::UAVMask, BufferCreateInfo2);
	check(State->SharedNNOutputBufferD3D12.IsValid());

	// Create a Windows HANDLE of the shared buffer for interop
	HANDLE SharedInputBufferHandle, SharedOutputBufferHandle;
	auto SecurityAttributes = SECURITY_ATTRIBUTES{};
	check(Device->CreateSharedHandle(
		D3D->RHIGetResource(State->SharedNNInputBufferD3D12), &SecurityAttributes, GENERIC_ALL, nullptr,
		&SharedInputBufferHandle) == S_OK);
	check(Device->CreateSharedHandle(
		D3D->RHIGetResource(State->SharedNNOutputBufferD3D12), &SecurityAttributes, GENERIC_ALL, nullptr,
		&SharedOutputBufferHandle) == S_OK);

	// Call the external function to initialize neural networks and the interop layer.
	auto Params = MIGINNInitializeParams {
		.Platform = {
			.Win_D3D12 = {
				.InD3D12FenceHandle = SharedFenceHandle,
				.InD3D12InputBufferResourceHandle = SharedInputBufferHandle,
				.InD3D12OutputBufferResourceHandle = SharedOutputBufferHandle
			},
		},
		.InInputBufferSize = SharedInputBufferSize,
		.InOutputBufferSize = SharedOutputBufferSize,
		.InInputBufferOffset = 0,
		.InOutputBufferOffset = 0,
		.InDeviceIndex = 0,
		.InPlatformType = MIGIPlatformType::eWindowsD3D12
	};
	auto result = MIGINNInitialize(Params);
	check(result == MIGINNResultType::eSuccess);

	// Destroy the Windows HANDLEs.
	CloseHandle(SharedInputBufferHandle);
	CloseHandle(SharedOutputBufferHandle);
	CloseHandle(SharedFenceHandle);
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
	if(!IsRHID3D12())
	{
		return false;
	}
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

void FMIGICUDAAdapterD3D12::SynchronizeFromNN(FRHICommandList& RHICmdList)
{
	auto SyncFenceValue = State->NextFenceValue++;
	check(MIGINNSignalFenceValue(SyncFenceValue) == MIGINNResultType::eSuccess);
	// Stall until NN signals.
	auto D3D = GetID3D12DynamicRHI();
	// It's possible for non-bypass mode RHICmdList to have no ComputeContext.
	// So we queue a RHI lambda command for delayed execution.
	RHICmdList.EnqueueLambda(TEXT("RHIWaitManualFence"),
		[D3D, SyncFenceValue, Fence = State->SharedFenceD3D12](FRHICommandList& RHICmdList){
		D3D->RHIWaitManualFence(RHICmdList, Fence.Get(), SyncFenceValue);
	});
}


void FMIGICUDAAdapterD3D12::SynchronizeToNN(FRHICommandList& RHICmdList)
{
	auto D3D = GetDynamicRHI<ID3D12DynamicRHI>();
	auto SyncFenceValue = State->NextFenceValue++;
	// Signal the shared fence upon computation work done.
	// It's possible for non-bypass mode RHICmdList to have no ComputeContext.
	// So we queue a RHI lambda command for delayed execution.
	RHICmdList.EnqueueLambda(TEXT("RHISignalManualFence"),
		[D3D, SyncFenceValue, Fence = State->SharedFenceD3D12](FRHICommandList& RHICmdList){
			D3D->RHISignalManualFence(RHICmdList, Fence.Get(), SyncFenceValue);
		});	
	check(MIGINNWaitFenceValue(SyncFenceValue) == MIGINNResultType::eSuccess);
}

FRHIBuffer* FMIGICUDAAdapterD3D12::GetSharedInputBuffer() const
{
	return State->SharedNNInputBufferD3D12;
}

FRHIBuffer* FMIGICUDAAdapterD3D12::GetSharedOutputBuffer() const
{
	return State->SharedNNOutputBufferD3D12;
}