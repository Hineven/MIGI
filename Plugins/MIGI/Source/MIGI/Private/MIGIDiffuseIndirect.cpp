#include "MIGIRendering.h"

#include "MIGIConfig.h"
#include "MIGIConstants.h"
#include "MIGINNAdapter.h"
#include "MIGILogCategory.h"
#include "MIGINN.h"
#include "MIGIShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"

void MIGIRenderDiffuseIndirect(const FScene& Scene, const FViewInfo& ViewInfo, FRDGBuilder& GraphBuilder, FGlobalIlluminationPluginResources &  RenderResources)
{
	// Try to initialize RDG buffers when possible.
	MIGIRenderingContext::Get().Initialze_RenderThread();
	
	auto Adapter = IMIGINNAdapter::GetInstance();
	// The adapter is not ready for some reason (reloading, etc). Render nothing.
	if(!Adapter->IsReady()) return;

	
	// This function is running on the rendering thread, so it's okay to create FRDGBuffers outside of passes.
	// Register NN external buffers.
	
	FRDGBufferRef NNInputBufferRDG = GraphBuilder.RegisterExternalBuffer(
		MIGIRenderingContext::Get().GetNNInputBufferRDG(), TEXT("MIGINNInputBuffer"));
	FRDGBufferRef NNOutputBufferRDG = GraphBuilder.RegisterExternalBuffer(
		MIGIRenderingContext::Get().GetNNOutputBufferRDG(), TEXT("MIGINNOutputBuffer"));

	// Input & inference & training
	{
		auto ComputeShader = ViewInfo.ShaderMap->GetShader<MIGINNInputShaderCS>();
        auto PassParameters = GraphBuilder.AllocParameters<MIGINNInputShaderCS::FParameters>();
        PassParameters->TestParam = FVector4f{0.1, 0.4, 0.7, 1.f};
        auto UAV = GraphBuilder.CreateUAV(NNInputBufferRDG, PF_R32_FLOAT);
        PassParameters->NNInputBuffer = UAV;
		GraphBuilder.AddPass( RDG_EVENT_NAME("MIGIRenderDiffuseIndirectNNInput"), PassParameters,
			ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
			// Be VERY VERY CAREFUL when capturing parameters! Especially by REFERENCE!
			[ComputeShader, PassParameters](FRHICommandListImmediate& RHICmdList)
			{
				// Dispatch the compute shader to produce NN inputs.
				auto ParameterMetadata = MIGINNInputShaderCS::FParameters::FTypeInfo::GetStructMetadata();
				FComputeShaderUtils::Dispatch(
					RHICmdList, ComputeShader, ParameterMetadata,
					*PassParameters, {1, 1, 1});
				// Synchronize the NN input buffer.
				auto Adapter = IMIGINNAdapter::GetInstance();
				Adapter->SynchronizeToNN(RHICmdList);
				// Schedule NN inference.
				auto InferenceParams = MIGINNInferenceParams {
					.InInputBufferOffset = 0,
					.InOutputBufferOffset = 0,
					.InNumElements =  256
				};
				MIGINNInference(InferenceParams);
				// TODO input training data and train at the same time.
			}
		);
	}
	// Output
	{
		auto PassParameters = GraphBuilder.AllocParameters<MIGINNOutputShaderCS::FParameters>();
		PassParameters->NNOutputBuffer = GraphBuilder.CreateSRV(NNOutputBufferRDG, PF_R32_FLOAT);
		PassParameters->ColorBuffer = GraphBuilder.CreateUAV(RenderResources.SceneColor);
		auto ComputeShader = ViewInfo.ShaderMap->GetShader<MIGINNOutputShaderCS>();
		GraphBuilder.AddPass( RDG_EVENT_NAME("MIGIRenderDiffuseIndirectNNOutput"), PassParameters,
			ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
			[ComputeShader, PassParameters] (FRHICommandListImmediate& RHICmdList)
			{
				// Synchronize from the NN output buffer.
             	auto Adapter = IMIGINNAdapter::GetInstance();
             	Adapter->SynchronizeFromNN(RHICmdList);
				
				// Dispatch the compute shader to produce NN inputs.
				auto ParameterMetadata = MIGINNOutputShaderCS::FParameters::FTypeInfo::GetStructMetadata();
				FComputeShaderUtils::Dispatch(
					RHICmdList, ComputeShader, ParameterMetadata,
					*PassParameters, {16, 16, 1});
			}
		);
	}
}
