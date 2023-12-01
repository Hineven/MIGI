#pragma once

void MIGIRenderPathTracing(
	FScene * Scene,
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTexturesUniformBuffer,
	FRDGTextureRef SceneColorOutputTexture,
	FRDGTextureRef SceneDepthOutputTexture,
	FPathTracingResources& PathTracingResources) ;


void MIGIPreparePathTracing(const FViewInfo & ViewInfo, TArray<FRHIRayTracingShader*>& OutRayGenShaders);