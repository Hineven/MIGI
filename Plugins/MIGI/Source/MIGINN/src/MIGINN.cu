/*
 * Project MIGINN : MIGINN.cpp
 * Created: 2023/11/13
 * This program is unlicensed. See LICENSE for more.
 */
#include <cuda_runtime.h>
#include "MIGINN.h"
#include "MIGINNCUDAHelper.cuh"
#include "MIGINNInternal.cuh"
int MIGIGetCUDAErrorCode() {
    return cudaGetLastError();
}

std::string MIGIGetCUDAErrorString() {
    return cudaGetErrorString(cudaGetLastError());
}

// CUDA context.
cudaStream_t GCUDAStream;
// Declare the external semaphore handle and external input & output memory handles.
cudaExternalSemaphore_t GExternalSemaphoreHandle;
cudaExternalMemory_t GExternalInputMemoryHandle;
cudaExternalMemory_t GExternalOutputMemoryHandle;

CUdeviceptr GInputBufferAddress;
CUdeviceptr GOutputBufferAddress;

std::unique_ptr<MIGINNCacheNetwork> GNetwork;

MIGINNResultType MIGINNInitialize (const MIGINNInitializeParams &Params) {
    // Initialize CUDA context.
    try {
        checkCUDA(cudaInitDevice(Params.InDeviceIndex, 0, 0));
        checkCUDA(cudaSetDevice(Params.InDeviceIndex));
        checkCUDA(cudaStreamCreate(&GCUDAStream));
    } catch(std::runtime_error & e) {
        return MIGINNResultType::eCUDAError;
    }
    // Import D3D handles into CUDA.
    try {
        // Okay, we can't use C++ 20 due to stupid bugs in fmtlib.
//        cudaExternalSemaphoreHandleDesc InExternalSemaphoreHandleDesc{
//                .type = cudaExternalSemaphoreHandleTypeD3D12Fence,
//                .handle = {.win32 = {.handle = Params.Platform.Win_D3D12.InD3D12FenceHandle}},
//                .flags = 0
//        };
        cudaExternalSemaphoreHandleDesc InExternalSemaphoreHandleDesc{};
        InExternalSemaphoreHandleDesc.type = cudaExternalSemaphoreHandleTypeD3D12Fence;
        InExternalSemaphoreHandleDesc.handle.win32.handle = Params.Platform.Win_D3D12.InD3D12FenceHandle;
        InExternalSemaphoreHandleDesc.flags = 0;
        checkCUDA(cudaImportExternalSemaphore(&GExternalSemaphoreHandle, &InExternalSemaphoreHandleDesc));
//        cudaExternalMemoryHandleDesc InExternalInputMemoryHandleDesc{
//                .type = cudaExternalMemoryHandleTypeD3D12Resource,
//                .handle = {.win32 = {.handle = Params.Platform.Win_D3D12.InD3D12InputBufferResourceHandle}},
//                .size = Params.InInputBufferSize,
//                .flags = cudaExternalMemoryDedicated
//        };
        cudaExternalMemoryHandleDesc InExternalInputMemoryHandleDesc{};
        InExternalInputMemoryHandleDesc.type = cudaExternalMemoryHandleTypeD3D12Resource;
        InExternalInputMemoryHandleDesc.handle.win32.handle = Params.Platform.Win_D3D12.InD3D12InputBufferResourceHandle;
        InExternalInputMemoryHandleDesc.size = Params.InInputBufferSize;
        InExternalInputMemoryHandleDesc.flags = cudaExternalMemoryDedicated;
        checkCUDA(cudaImportExternalMemory(&GExternalInputMemoryHandle, &InExternalInputMemoryHandleDesc));
//        cudaExternalMemoryHandleDesc InExternalOutputMemoryHandleDesc{
//                .type = cudaExternalMemoryHandleTypeD3D12Resource,
//                .handle = {.win32 = {.handle = Params.Platform.Win_D3D12.InD3D12OutputBufferResourceHandle}},
//                .size = Params.InOutputBufferSize,
//                .flags = cudaExternalMemoryDedicated
//        };
        cudaExternalMemoryHandleDesc InExternalOutputMemoryHandleDesc{};
        InExternalOutputMemoryHandleDesc.type = cudaExternalMemoryHandleTypeD3D12Resource;
        InExternalOutputMemoryHandleDesc.handle.win32.handle = Params.Platform.Win_D3D12.InD3D12OutputBufferResourceHandle;
        InExternalOutputMemoryHandleDesc.size = Params.InOutputBufferSize;
        InExternalOutputMemoryHandleDesc.flags = cudaExternalMemoryDedicated;

        checkCUDA(cudaImportExternalMemory(&GExternalOutputMemoryHandle, &InExternalOutputMemoryHandleDesc));
        // Get the GPU memory address of the input and output buffers.
//        auto InputBufferDesc = cudaExternalMemoryBufferDesc {
//                .offset = Params.InInputBufferOffset,
//                .size = Params.InInputBufferSize,
//                .flags = 0
//        };
        auto InputBufferDesc = cudaExternalMemoryBufferDesc{};
        InputBufferDesc.offset = Params.InInputBufferOffset;
        InputBufferDesc.size = Params.InInputBufferSize;
        InputBufferDesc.flags = 0;
        checkCUDA(cudaExternalMemoryGetMappedBuffer((void**)&GInputBufferAddress, GExternalInputMemoryHandle, &InputBufferDesc));
//        auto OutputBufferDesc = cudaExternalMemoryBufferDesc {
//                .offset = Params.InOutputBufferOffset,
//                .size = Params.InOutputBufferSize,
//                .flags = 0
//        };
        auto OutputBufferDesc = cudaExternalMemoryBufferDesc{};
        OutputBufferDesc.offset = Params.InOutputBufferOffset;
        OutputBufferDesc.size = Params.InOutputBufferSize;
        OutputBufferDesc.flags = 0;
        checkCUDA(cudaExternalMemoryGetMappedBuffer((void**)&GOutputBufferAddress, GExternalOutputMemoryHandle, &OutputBufferDesc));
    } catch(std::runtime_error & e) {
        return MIGINNResultType::eCUDAError;
    }
    return MIGINNResultType::eSuccess;
}

MIGINNResultType MIGINNDestroy() {
    // Wait for CUDA idle.
    try {
        checkCUDA(cudaStreamSynchronize(GCUDAStream));
    } catch(std::runtime_error & e) {
        return MIGINNResultType::eCUDAError;
    }
    // Destroy the CUDA context and release all resources.
    try {
        // Clear pointers
        GInputBufferAddress = 0;
        GOutputBufferAddress = 0;
        // Destroy CUDA context
        checkCUDA(cudaStreamDestroy(GCUDAStream));
        GCUDAStream = nullptr;
        checkCUDA(cudaDestroyExternalMemory(GExternalInputMemoryHandle));
        GExternalInputMemoryHandle = nullptr;
        checkCUDA(cudaDestroyExternalMemory(GExternalOutputMemoryHandle));
        GExternalOutputMemoryHandle = nullptr;
        checkCUDA(cudaDestroyExternalSemaphore(GExternalSemaphoreHandle));
        GExternalSemaphoreHandle = nullptr;
        checkCUDA(cudaDeviceReset());
    } catch(std::runtime_error & e) {
        return MIGINNResultType::eCUDAError;
    }
    return MIGINNResultType::eSuccess;
}

MIGINNResultType MIGINNWaitFenceValue(uint64_t InWaitFenceValue) {
    // Queue a fence wait in the CUDA stream.
    try {
        auto WaitParams = cudaExternalSemaphoreWaitParams{};
        WaitParams.params.fence.value = InWaitFenceValue;
        WaitParams.flags = 0;
        checkCUDA(cudaWaitExternalSemaphoresAsync(&GExternalSemaphoreHandle, &WaitParams, 1, GCUDAStream));
    } catch(std::runtime_error & e) {
        return MIGINNResultType::eCUDAError;
    }
    return MIGINNResultType::eSuccess;
}

MIGINNResultType MIGINNSignalFenceValue(uint64_t InSignalFenceValue) {
    // Signal the fence value in the CUDA stream.
    try {
        auto SignalParams = cudaExternalSemaphoreSignalParams{};
        SignalParams.params.fence.value = InSignalFenceValue;
        SignalParams.flags = 0;
        checkCUDA(cudaSignalExternalSemaphoresAsync(&GExternalSemaphoreHandle, &SignalParams, 1, GCUDAStream));
    } catch(std::runtime_error & e) {
        return MIGINNResultType::eCUDAError;
    }
    return MIGINNResultType::eSuccess;
}

MIGINNResultType MIGINNInitializeNeuralNetwork(const MIGINNNetworkConfig &Config) {
    if(Config.Type == MIGINNNetworkType::eMLP) {
        return (GNetwork = MIGINNMLPCacheNetwork::Create(Config)) ? MIGINNResultType::eSuccess : MIGINNResultType::eError;
    } else return MIGINNResultType::eError;
}


MIGINNResultType MIGINNTrainNetwork(const MIGINNTrainNetworkParams &Params) {
    if(GNetwork) {
        return GNetwork->Train(Params);
    } else return MIGINNResultType::eError;
}

MIGINNResultType MIGINNInference(const MIGINNInferenceParams &Params) {
    if(GNetwork) {
        return GNetwork->Inference(Params);
    } else return MIGINNResultType::eError;
}
