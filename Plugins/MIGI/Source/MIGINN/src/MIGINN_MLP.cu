/*
 * Project MIGINN : MIGINN_MLP.cpp
 * Created: 2023/11/13
 * This program is unlicensed. See LICENSE for more.
 */
#include "MIGINN.h"
#include "MIGINNInternal.cuh"

#include "tiny-cuda-nn/network_with_input_encoding.h"
#include "tiny-cuda-nn/loss.h"
#include "tiny-cuda-nn/optimizer.h"
#include "tiny-cuda-nn/trainer.h"
#include <tiny-cuda-nn/common.h>
#include <tiny-cuda-nn/network.h>

class MIGINNMLPCacheNetworkImpl {
public:
    MIGINNMLPCacheNetworkImpl () = default;
    MIGINNResultType Initialize (const MIGINNNetworkConfig &Params) {
        try {
            auto MLP = Params.Details.MLP;
            auto ExtraOptions = nlohmann::json::parse(MLP.InExtraOptionsJson);
            auto EncodingOptions = ExtraOptions["encoding"];
            auto NetworkOptions = ExtraOptions["network"];
            auto LossOptions = ExtraOptions["loss"];
            auto OptimizerOptions = ExtraOptions["optimizer"];
            Loss.reset(tcnn::create_loss<PrecisionClass>(LossOptions));
            Optimizer.reset(tcnn::create_optimizer<PrecisionClass>(OptimizerOptions));
            Network = std::make_shared<NetworkClass>(MLP.InNumInputDimensions, MLP.InNumOutputDimensions, EncodingOptions,
                                                     NetworkOptions);
            Trainer = std::make_shared<decltype(Trainer)::element_type>(Network, Optimizer, Loss);
        } catch(std::runtime_error & e) {
            return MIGINNResultType::eInternalError;
        }
        return MIGINNResultType::eSuccess;
    }

    [[nodiscard]] MIGINNResultType Inference (const MIGINNInferenceParams & Params) const {
        using namespace tcnn;
        // Retarget inputs & outputs to the shared input & output buffer
        GPUMatrix<float> InputMatrix(
                (float*)((std::byte*)GInputBufferAddress + Params.InInputBufferOffset),
                Network->input_width(), Params.InNumElements
        );
        GPUMatrix<float> OutputMatrix(
                (float*)((std::byte*)GOutputBufferAddress + Params.InOutputBufferOffset),
                Network->output_width(), Params.InNumElements
        );
        Network->inference(GCUDAStream, InputMatrix, OutputMatrix);
        return MIGINNResultType::eSuccess;
    }

    MIGINNResultType Train (const MIGINNTrainNetworkParams & Params) {
        using namespace tcnn;
        // Retarget inputs to the shared input buffer
        GPUMatrix<float> InputMatrix(
                (float*)((std::byte*)GInputBufferAddress + Params.InInputBufferOffset),
                Network->input_width(), Params.InNumElements
        );
        GPUMatrix<float> TargetMatrix(
                (float*)((std::byte*)GInputBufferAddress + Params.InInputBufferTargetOffset),
                Network->output_width(), Params.InNumElements
        );
        auto TrainContext = Trainer->training_step(InputMatrix, TargetMatrix);
        return MIGINNResultType::eSuccess;
    }


protected:
    typedef tcnn::network_precision_t PrecisionClass;
    typedef tcnn::NetworkWithInputEncoding<tcnn::network_precision_t> NetworkClass;
    std::shared_ptr<NetworkClass> Network;
    std::shared_ptr<tcnn::Loss<PrecisionClass> > Loss;
    std::shared_ptr<tcnn::Optimizer<PrecisionClass> > Optimizer;
    std::shared_ptr<tcnn::Trainer<float, PrecisionClass, PrecisionClass> > Trainer;
};

// Make sure the unique_ptr is compilable.
MIGINNMLPCacheNetwork::MIGINNMLPCacheNetwork() {}
MIGINNMLPCacheNetwork::~MIGINNMLPCacheNetwork() = default;

MIGINNResultType MIGINNMLPCacheNetwork::Inference(const MIGINNInferenceParams &Params) const {return Impl->Inference(Params);}
MIGINNResultType MIGINNMLPCacheNetwork::Train(const MIGINNTrainNetworkParams &Params) {return Impl->Train(Params);}


std::unique_ptr<MIGINNCacheNetwork> MIGINNMLPCacheNetwork::Create (const MIGINNNetworkConfig &Params) {
    auto Network = std::make_unique<MIGINNMLPCacheNetwork>();
    Network->Impl = std::make_unique<MIGINNMLPCacheNetworkImpl>();
    if(Network->Impl->Initialize(Params) != MIGINNResultType::eSuccess) return nullptr;
    return Network;
}