#include "MIGIConfig.h"
#include "MIGIRendering.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"

// Okay, an necessary invasion into the engine source for easy raytracing impl.
#include "Lumen/LumenReflections.h"
#include "Lumen/LumenHardwareRayTracingCommon.h"

#include "MIGINNAdapter.h"
#include "MIGINN.h"
#include "MIGIConstants.h"
#include "MIGIPT.h"
#include "ScenePrivate.h"


BEGIN_SHADER_PARAMETER_STRUCT(FMIGINNCommonShaderParameters,)
	SHADER_PARAMETER(FVector4f, TestParam)
	SHADER_PARAMETER(unsigned, NNMaxInferenceSampleSize)
	SHADER_PARAMETER(unsigned, NNTrainSampleSize)
END_SHADER_PARAMETER_STRUCT()

class FMIGINNParameters final
{
public:

	static void ModifyCompilationEnvironment(FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE_1D"), C::ThreadGroupSize1D);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE_2D"), C::ThreadGroupSize2D);
		OutEnvironment.SetDefine(TEXT("NN_INPUT_WIDTH"), C::NNInputWidth);
		OutEnvironment.SetDefine(TEXT("NN_OUTPUT_WIDTH"), C::NNOutputWidth);
	}
};

class FMIGINNInputShaderCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FMIGINNInputShaderCS);
	SHADER_USE_PARAMETER_STRUCT(FMIGINNInputShaderCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_INCLUDE(FMIGINNCommonShaderParameters, CommonParameters)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float>, NNInputBuffer)
	END_SHADER_PARAMETER_STRUCT()

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FMIGINNParameters::ModifyCompilationEnvironment(OutEnvironment);
	}
};

IMPLEMENT_GLOBAL_SHADER(FMIGINNInputShaderCS,
	"/Plugin/MIGI/Private/NNInterface.usf", "NNInput",
	EShaderFrequency::SF_Compute);

class FMIGINNOutputShaderCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FMIGINNOutputShaderCS);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_INCLUDE(FMIGINNCommonShaderParameters, CommonParameters)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, NNOutputBuffer)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, ColorBuffer)
	END_SHADER_PARAMETER_STRUCT()


	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FMIGINNParameters::ModifyCompilationEnvironment(OutEnvironment);
	}
};

IMPLEMENT_GLOBAL_SHADER(FMIGINNOutputShaderCS,
	"/Plugin/MIGI/Private/NNInterface.usf", "NNOutput",
	EShaderFrequency::SF_Compute);

class FMIGISimpleDiffuseRayTracing : public FLumenHardwareRayTracingShaderBase
{
	DECLARE_LUMEN_RAYTRACING_SHADER(FMIGISimpleDiffuseRayTracing, Lumen::ERayTracingShaderDispatchSize::DispatchSize2D)

	// We only have 1 shader permutation! No need to complicate things for compatibility.

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FLumenHardwareRayTracingShaderBase::FSharedParameters, SharedParameters)
		RDG_BUFFER_ACCESS(HardwareRayTracingIndirectArgs, ERHIAccess::IndirectArgs | ERHIAccess::SRVCompute)
		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<uint>, CompactedTraceTexelAllocator)
		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<uint>, CompactedTraceTexelData)
		SHADER_PARAMETER_STRUCT_INCLUDE(FLumenHZBScreenTraceParameters, HZBScreenTraceParameters)
		SHADER_PARAMETER(float, RelativeDepthThickness)
		SHADER_PARAMETER(float, SampleSceneColorNormalTreshold)
		SHADER_PARAMETER(int32, SampleSceneColor)

		SHADER_PARAMETER(int, NearFieldLightingMode)
		SHADER_PARAMETER(uint32, UseReflectionCaptures)
		SHADER_PARAMETER(float, FarFieldBias)
		SHADER_PARAMETER(float, PullbackBias)
		SHADER_PARAMETER(uint32, MaxTraversalIterations)
		SHADER_PARAMETER(int, ApplySkyLight)
		SHADER_PARAMETER(int, HitLightingForceEnabled)
		SHADER_PARAMETER(FVector3f, FarFieldReferencePos)

		// Reflection-specific includes (includes output targets)
		SHADER_PARAMETER_STRUCT_INCLUDE(FLumenReflectionTracingParameters, ReflectionTracingParameters)
		SHADER_PARAMETER_STRUCT_INCLUDE(FLumenReflectionTileParameters, ReflectionTileParameters)
		SHADER_PARAMETER_STRUCT_INCLUDE(LumenRadianceCache::FRadianceCacheInterpolationParameters, RadianceCacheParameters)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FVirtualVoxelParameters, HairStrandsVoxel)
	END_SHADER_PARAMETER_STRUCT()

	static ERayTracingPayloadType GetRayTracingPayloadType(const int32 PermutationId)
	{
		return ERayTracingPayloadType::RayTracingMaterial;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, Lumen::ERayTracingShaderDispatchType ShaderDispatchType, FShaderCompilerEnvironment& OutEnvironment)
	{
		FLumenHardwareRayTracingShaderBase::ModifyCompilationEnvironment(Parameters, ShaderDispatchType, Lumen::ESurfaceCacheSampling::HighResPages, OutEnvironment);
	}
};
IMPLEMENT_LUMEN_RAYGEN_RAYTRACING_SHADER(FMIGISimpleDiffuseRayTracing)

// Only the HWRT version is implemented.
IMPLEMENT_GLOBAL_SHADER(FMIGISimpleDiffuseRayTracingRGS, "/Plugin/MIGI/Private/MIGISimpleDiffuseRayTracing.usf", "MIGISimpleDiffuseRayTracingRGS", SF_RayGen);

void MIGIPrepareRayTracingDiffuseIndirect (const FViewInfo& View, TArray<FRHIRayTracingShader*>& OutRayGenShaders)
{
	if(IsMIGIEnabled())
	{
		for (int32 HairOcclusion = 0; HairOcclusion < 2; HairOcclusion++)
		{
			TShaderRef<FMIGISimpleDiffuseRayTracingRGS> RayGenerationShader = View.ShaderMap->GetShader<FMIGISimpleDiffuseRayTracingRGS>();
			OutRayGenShaders.Add(RayGenerationShader.GetRayTracingShader());
		}
	}
}

void MIGIRenderDiffuseIndirect(const FScene& Scene, const FViewInfo& ViewInfo, FRDGBuilder& GraphBuilder, FGlobalIlluminationPluginResources &  RenderResources)
{
	// Try to initialize RDG buffers when possible.
	MIGIRenderingContext::Get().Initialze_RenderThread();
	
	auto Adapter = IMIGINNAdapter::GetInstance();
	// The adapter is not ready for some reason (reloading, etc). Render nothing.
	if(!Adapter->IsReady()) return;


	// Let's check if the path tracing rendering works.

	// RenderPathTracing(GraphBuilder, View, SceneTextures.UniformBuffer, SceneTextures.Color.Target, SceneTextures.Depth.Target,PathTracingResources);
	FSceneRenderer * SceneRenderer = dynamic_cast<FDeferredShadingSceneRenderer*>(ViewInfo.Family->GetSceneRenderer());

	// MIGI only supports the deferred render path.
	check(SceneRenderer);
	
	MIGIRenderPathTracing(
		&Scene, GraphBuilder, ViewInfo, SceneRenderer->SceneTextures.UniformBuffer,
		RenderResources.SceneColor, RenderResources.SceneDepth, RenderResources.PathTracingResources
		
	);

	return ;
	
	// This function is running on the rendering thread, so it's okay to create FRDGBuffers outside of passes.
	// Register NN external buffers.
	
	FRDGBufferRef NNInputBufferRDG = GraphBuilder.RegisterExternalBuffer(
		MIGIRenderingContext::Get().GetNNInputBufferRDG(), TEXT("MIGINNInputBuffer"));
	FRDGBufferRef NNOutputBufferRDG = GraphBuilder.RegisterExternalBuffer(
		MIGIRenderingContext::Get().GetNNOutputBufferRDG(), TEXT("MIGINNOutputBuffer"));

	// Input & inference & training
	{
		auto ComputeShader = ViewInfo.ShaderMap->GetShader<FMIGINNInputShaderCS>();
        auto PassParameters = GraphBuilder.AllocParameters<FMIGINNInputShaderCS::FParameters>();
		// Generate a random float number between 0 and 1
		PassParameters->CommonParameters.TestParam = FVector4f{FMath::FRand(), FMath::FRand(), FMath::FRand(), FMath::FRand()};
        auto UAV = GraphBuilder.CreateUAV(FRDGBufferUAVDesc{NNInputBufferRDG});
        PassParameters->NNInputBuffer = UAV;
		// TODO set to real inference sample count
		PassParameters->CommonParameters.NNMaxInferenceSampleSize = ViewInfo.ViewRect.Area();
		// TODO set to actual train sample count
		PassParameters->CommonParameters.NNTrainSampleSize = FMath::Floor((float)ViewInfo.ViewRect.Area() * 0.03f);

		// Check the total size of the NN input buffer. Make sure it doesn't exceeds the buffer limit.
		{
			auto MaxInputBufferSize = C::DefaultSharedBufferSize;
			auto TrainDataSize = PassParameters->CommonParameters.NNTrainSampleSize * (C::NNInputWidth + C::NNOutputWidth) * sizeof(float);
			auto InferenceDataSize = PassParameters->CommonParameters.NNMaxInferenceSampleSize * C::NNInputWidth * sizeof(float);
			auto TotalSize = TrainDataSize + InferenceDataSize;
			check(TotalSize <= MaxInputBufferSize);
		}
		
		GraphBuilder.AddPass( RDG_EVENT_NAME("MIGIRenderDiffuseIndirectNNInput"), PassParameters,
			ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
			// Be VERY VERY CAREFUL when capturing parameters! Especially by REFERENCE!
			[ComputeShader, PassParameters, ViewRect = ViewInfo.ViewRect](FRHICommandListImmediate& RHICmdList)
			{
				// Dispatch the compute shader to produce NN queries & training data.
				auto ParameterMetadata = FMIGINNInputShaderCS::FParameters::FTypeInfo::GetStructMetadata();
				auto NumGroups = FIntVector::DivideAndRoundUp(
					FIntVector{ViewRect.Width(), ViewRect.Height(), 1},
					FIntVector{C::ThreadGroupSize2D, C::ThreadGroupSize2D, 1});
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, ParameterMetadata, *PassParameters, NumGroups);
				
				// Synchronize the NN input buffer.
				auto Adapter = IMIGINNAdapter::GetInstance();
				Adapter->SynchronizeToNN(RHICmdList);
				// IMPORTANT: Flush queued RHI commands to the GPU.
				// MIGINNInference() possibly allocates GPU memory, which results in the calling of cudaDeviceWaitIdle()
				// Thus it's possible to run into a deadlock if the signal RHI command has not been submitted yet.
				RHICmdList.SubmitCommandsHint();
				// Schedule NN inference.
				auto InferenceParams = MIGINNInferenceParams {
					.InInputBufferOffset = 0,
					.InOutputBufferOffset = 0,
					.InNumElements = PassParameters->CommonParameters.NNMaxInferenceSampleSize
				};
				// Requires a NN inference
				MIGINNInference(InferenceParams);
				// Requires a NN training step
				auto TrainInputBufferOffset = PassParameters->CommonParameters.NNMaxInferenceSampleSize * C::NNInputWidth * sizeof(float);
				auto TrainTargetBufferOffset = TrainInputBufferOffset + PassParameters->CommonParameters.NNTrainSampleSize * C::NNInputWidth * sizeof(float);
				auto TrainParams = MIGINNTrainNetworkParams {
					.InInputBufferOffset = TrainInputBufferOffset,
					.InInputBufferTargetOffset = TrainTargetBufferOffset,
					.InNumElements = PassParameters->CommonParameters.NNTrainSampleSize
				};
				MIGINNTrainNetwork(TrainParams);
			}
		);
	}
	// Output
	{
		auto PassParameters = GraphBuilder.AllocParameters<FMIGINNOutputShaderCS::FParameters>();
		PassParameters->View = ViewInfo.GetShaderParameters().View;
		PassParameters->NNOutputBuffer = GraphBuilder.CreateSRV(NNOutputBufferRDG, PF_R32_FLOAT);
		PassParameters->ColorBuffer = GraphBuilder.CreateUAV(FRDGTextureUAVDesc{RenderResources.SceneColor});
		auto ComputeShader = ViewInfo.ShaderMap->GetShader<FMIGINNOutputShaderCS>();
		GraphBuilder.AddPass( RDG_EVENT_NAME("MIGIRenderDiffuseIndirectNNOutput"), PassParameters,
			ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
			[ComputeShader, PassParameters, ViewRect = ViewInfo.ViewRect] (FRHICommandListImmediate& RHICmdList)
			{
				// Synchronize from the NN output buffer.
             	auto Adapter = IMIGINNAdapter::GetInstance();
             	Adapter->SynchronizeFromNN(RHICmdList);
				
				// Dispatch the compute shader to produce NN inputs.
				auto ParameterMetadata = FMIGINNOutputShaderCS::FParameters::FTypeInfo::GetStructMetadata();
				auto ViewRect3 = FIntVector {ViewRect.Size().X, ViewRect.Size().Y, 1};
				auto NumThreadGroups = FIntVector::DivideAndRoundUp(
						ViewRect3, FIntVector{C::ThreadGroupSize2D, C::ThreadGroupSize2D, 1});
				FComputeShaderUtils::Dispatch(
					RHICmdList, ComputeShader, ParameterMetadata,
					*PassParameters, NumThreadGroups);
			}
		);
	}
}
