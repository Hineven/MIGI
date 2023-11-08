#include "MIGISyncAdapterD3D12.h"

#include "ID3D12DynamicRHI.h"
#include "D3D12Resources.h"
#include "MIGILogCategory.h"

#include "CudaModule.h"
#include "CudaWrapper.h"
#include "D3D12RHIPrivate.h"
#include "MIGIModule.h"
#include "../../../../../../../UE_5.3/Engine/Plugins/Importers/USDImporter/Source/ThirdParty/USD/include/pxr/base/tf/refPtr.h"

// #include "cuda_runtime.h"

bool FMIGISyncUtilsD3D12::InstallRHIConfigurations()
{
	// No need to install anything. Just check if the platform supports D3D
#ifdef WIN32
	return true;
#else
	return false;
#endif
}

class MIGISyncUtilsD3D12State
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

	unsigned CurrentFenceValue {};
	

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
		
	}
	void Destroy ()
	{
		// Destroy CUDA resources.
		check(cuDestroyExternalMemory(SharedMemoryCUDA) == CUDA_SUCCESS);
		check(cuDestroyExternalSemaphore(SharedFenceCUDA) == CUDA_SUCCESS);
		// Destroy D3D12 resources.
		if(SharedMemoryD3D12)
		{
			if(SharedMemoryD3D12->Release() != S_OK)
			{
				UE_LOG(MIGI, Warning, TEXT("Unsuccessful shared buffer release, just try to proceed."));
			}
			if(SharedMemoryD3D12.GetRefCount() != 1)
			{
				UE_LOG(MIGI, Warning, TEXT("Someone else rather than the adapter is holding the shared buffer reference!"
					"Just proceeds and ignore leaks."));
			}
			SharedMemoryD3D12 = nullptr;
		}
		if(SharedFenceD3D12)
		{
			// TODO Will this be redundant?
			SharedFenceD3D12->Release();
			SharedFenceD3D12.Reset();
		}
	}

	~MIGISyncUtilsD3D12State ()
	{
		Destroy();
	}
	
};

bool FMIGISyncUtilsD3D12::TryActivate()
{
	// D3D12 needs no activation, so we just do some checking here.
	
	// This function is bound to the PostPreStartScreen.
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
	State = MakeUnique<MIGISyncUtilsD3D12State>();
	State->Initialize(FMIGIModule::Get().GetSharedBufferSize());
	
	UE_LOG(MIGI, Display, TEXT("Successfully activated RHI-CUDA synchronization utilities for D3D12: %s"), RHIName);
	return true;
}

FMIGISyncUtilsD3D12::~FMIGISyncUtilsD3D12()
{
	FModuleManager::Get().OnModulesChanged().Remove(
		RHIExtensionRegistrationDelegateHandle
	);
	RHIExtensionRegistrationDelegateHandle.Reset();
}

void FMIGISyncUtilsD3D12::SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList)
{
	auto D3D = GetDynamicRHI<ID3D12DynamicRHI>();
	// Signal the shared fence upon computation work done.
	State->CurrentFenceValue ++;
	D3D->RHISignalManualFence(RHICmdList, State->SharedFenceD3D12.Get(), State->CurrentFenceValue);
	auto CUDAContext = FModuleManager::Get().GetModuleChecked<FCUDAModule>("CUDA").GetCudaContext();
	// TODO
	check(false && "Unimplemented");
}


void FMIGISyncUtilsD3D12::SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList)
{
	// TODO
	check(false && "Unimplemented");
}