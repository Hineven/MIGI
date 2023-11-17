// The public library header file for MIGINN

#include <string>

constexpr size_t MIGINN_DETAILS_JSON_STRING_SIZE = 16384;

enum class MIGINNResultType {
    eSuccess = 0,
    eInternalError = 1,
    eError = 2,
    eCUDAError = 3,
    eNum
} ;

enum class MIGIPlatformType {
    eWindowsD3D12 = 0,
    eNum
};

enum class MIGINNNetworkType {
    eMLP = 0,
    eNum
};

struct MIGINNInitializeParams {
    // Used for Windows & D3D Platform
    union {
        struct {
            // winrt.h can cause trouble when compiling with NVCC.
            // We use native pointers to replace HANDLEs.
//            HANDLE InD3D12FenceHandle{};
//            HANDLE InD3D12InputBufferResourceHandle{};
//            HANDLE InD3D12OutputBufferResourceHandle{};
            void* InD3D12FenceHandle{};
            void* InD3D12InputBufferResourceHandle{};
            void* InD3D12OutputBufferResourceHandle{};
        } Win_D3D12;
    } Platform;
    size_t InInputBufferSize {};
    size_t InOutputBufferSize {};
    size_t InInputBufferOffset {};
    size_t InOutputBufferOffset {};
    uint32_t InDeviceIndex {};
    MIGIPlatformType InPlatformType {};
};

struct MIGINNDetailsMLP {
    uint32_t InNumInputDimensions {};
    uint32_t InNumOutputDimensions {};
    char InExtraOptionsJson[MIGINN_DETAILS_JSON_STRING_SIZE];
//    char * InEncodingOptionsJson[MIGINN_DETAILS_JSON_STRING_SIZE];
//    char * InNetworkOptionsJson[MIGINN_DETAILS_JSON_STRING_SIZE];
//    char * InLossOptionsJson[MIGINN_DETAILS_JSON_STRING_SIZE];
//    char * InOptimizerOptionsJson[MIGINN_DETAILS_JSON_STRING_SIZE];
};

struct MIGINNNetworkConfig {
    union {
        MIGINNDetailsMLP MLP;
    } Details {};
    MIGINNNetworkType Type {};
};

int MIGIGetCUDAErrorCode ();
std::string MIGIGetCUDAErrorString ();

MIGINNResultType MIGINNInitialize (const MIGINNInitializeParams & Params);

MIGINNResultType MIGINNInitializeNeuralNetwork (const MIGINNNetworkConfig & Config);

MIGINNResultType MIGINNDestroy ();

// Queue a barrier in the CUDA stream waiting for a certain fence value.
MIGINNResultType MIGINNWaitFenceValue (uint64_t InWaitFenceValue) ;
// Queue a fence value signal in the CUDA stream, this signal should be waited on by other processes.
MIGINNResultType MIGINNSignalFenceValue (uint64_t InSignalFenceValue) ;

struct MIGINNTrainNetworkParams {
    // The actual training data offset inside the input buffer.
    size_t InInputBufferOffset {};
    // The actual training target offset inside the output buffer.
    size_t InInputBufferTargetOffset {};
    // Number of elements in the input buffer.
    uint32_t InNumElements {};
    // Learning rate for the training.
    float InLearningRate {};
};
struct MIGINNInferenceParams {
    // The actual inference input offset inside the input buffer.
    size_t InInputBufferOffset {};
    // The actual inference output offset inside the output buffer.
    size_t InOutputBufferOffset {};
    // Number of elements in the input buffer.
    uint32_t InNumElements {};
};

MIGINNResultType MIGINNTrainNetwork (const MIGINNTrainNetworkParams & Params);
MIGINNResultType MIGINNInference (const MIGINNInferenceParams & Params);