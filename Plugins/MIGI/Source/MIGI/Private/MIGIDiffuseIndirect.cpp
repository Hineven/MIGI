﻿#include "MIGIDiffuseIndirect.h"

#include "MIGILogCategory.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"

BEGIN_SHADER_PARAMETER_STRUCT(EmptyShaderParameterStruct, MIGI_API)

END_SHADER_PARAMETER_STRUCT()

void MIGIRenderDiffuseIndirect(const FScene& Scene, const FViewInfo& ViewInfo, FRDGBuilder& GraphBuilder, FGlobalIlluminationPluginResources& RenderResources)
{
	// RDG_EVENT_SCOPE(GraphBuilder, TEXT("MIGIRenderDiffuseIndirect"));
	UE_LOG(MIGI, Display, TEXT("MIGI: MIGIRenderDiffuseIndirect"));
	EmptyShaderParameterStruct EmptyShaderParameter;
	GraphBuilder.AddPass( RDG_EVENT_NAME("MIGIRenderDiffuseIndirect"), &EmptyShaderParameter,
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		[&Scene, &ViewInfo, &RenderResources](FRHICommandListImmediate& RHICmdList)
		{
			UE_LOG(MIGI, Display, TEXT("MIGI: MIGIRenderDiffuseIndirect lambda"));
			// Try to flush the RHI command list.
			// If this is successful, it's possible to implement synchronization easily. 
			RHICmdList.SubmitCommandsAndFlushGPU(); 
			RHICmdList.BlockUntilGPUIdle();
			// Generate a vulkan semaphore.
			// RHICmdList.DrawPrimitive()
		}
	);
}