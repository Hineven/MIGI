/*
 * Project MIGINN : MIGINNInternal.h
 * Created: 2023/11/13
 * This program is unlicensed. See LICENSE for more.
 */

#ifndef MIGINN_MIGINNINTERNAL_CUH
#define MIGINN_MIGINNINTERNAL_CUH

#include <cuda.h>
#include <memory>

// CUDA context.
extern cudaStream_t GCUDAStream;
// Declare the external semaphore handle and external input & output memory handles.
extern cudaExternalSemaphore_t GExternalSemaphoreHandle;
extern cudaExternalMemory_t GExternalInputMemoryHandle;
extern cudaExternalMemory_t GExternalOutputMemoryHandle;

extern size_t GInputBufferAddress;
extern size_t GOutputBufferAddress;


class MIGINNCacheNetwork {
public:
    // Delete copy constructors and assignment operators.
    MIGINNCacheNetwork (const MIGINNCacheNetwork &) = delete;
    MIGINNCacheNetwork & operator = (const MIGINNCacheNetwork &) = delete;


    virtual MIGINNResultType Train (const MIGINNTrainNetworkParams &Params) = 0;
    virtual MIGINNResultType Inference (const MIGINNInferenceParams &Params) const = 0;

    // The virtual destructor.
    virtual ~MIGINNCacheNetwork () = default;
protected:
    MIGINNCacheNetwork () = default;
};

class MIGINNMLPCacheNetworkImpl;
class MIGINNMLPCacheNetwork : public MIGINNCacheNetwork {
public:
    MIGINNResultType Train (const MIGINNTrainNetworkParams &Params) override;
    MIGINNResultType Inference (const MIGINNInferenceParams &Params) const override;

    MIGINNMLPCacheNetwork () ;
    // The virtual destructor.
    ~MIGINNMLPCacheNetwork () ;
    static std::unique_ptr<MIGINNCacheNetwork> Create (const MIGINNNetworkConfig &Params);
protected:
    std::unique_ptr<MIGINNMLPCacheNetworkImpl> Impl {};
};

#endif //MIGINN_MIGINNINTERNAL_CUH
