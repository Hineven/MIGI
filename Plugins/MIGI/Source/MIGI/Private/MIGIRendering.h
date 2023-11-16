#pragma once
#include "CoreMinimal.h"
#include "DeferredShadingRenderer.h"

class MIGIRenderingContext
{
public:
	void Initialze_RenderThread () ;
	void Destroy_RenderThread () ;
	inline bool IsInitialized() const {return bInitialized;}
	static MIGIRenderingContext & Get();
	inline TRefCountPtr<FRDGPooledBuffer> GetNNInputBufferRDG () const {return NNInputBufferRDG;}
	inline TRefCountPtr<FRDGPooledBuffer> GetNNOutputBufferRDG () const {return NNOutputBufferRDG;}
protected:
	MIGIRenderingContext () = default;
	TRefCountPtr<FRDGPooledBuffer> NNInputBufferRDG;
	TRefCountPtr<FRDGPooledBuffer> NNOutputBufferRDG;
	bool bInitialized {};
};


void MIGIRenderDiffuseIndirect(const FScene& Scene, const FViewInfo& ViewInfo, FRDGBuilder& GraphBuilder, FGlobalIlluminationPluginResources & RenderResources) ;