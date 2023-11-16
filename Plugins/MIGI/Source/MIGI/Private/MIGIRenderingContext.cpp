#include "MIGINNAdapter.h"
#include "MIGIRendering.h"

void MIGIRenderingContext::Initialze_RenderThread ()
{
	if(bInitialized) return ;
	auto Adapter = IMIGINNAdapter::GetInstance();
	NNInputBufferRDG = new FRDGPooledBuffer(Adapter->GetSharedInputBuffer(),
		FRDGBufferDesc::CreateStructuredDesc(sizeof(float), IMIGINNAdapter::GetSharedInputBufferSize() / sizeof(float)),
		IMIGINNAdapter::GetSharedInputBufferSize() / sizeof(float), TEXT("MIGINNInputBuffer"));
	NNOutputBufferRDG = new FRDGPooledBuffer(Adapter->GetSharedOutputBuffer(),
		FRDGBufferDesc::CreateStructuredDesc(sizeof(float), IMIGINNAdapter::GetSharedOutputBufferSize() / sizeof(float)),
		IMIGINNAdapter::GetSharedOutputBufferSize() / sizeof(float), TEXT("MIGINNOutputBuffer"));
	bInitialized = true;
}


void MIGIRenderingContext::Destroy_RenderThread ()
{
	NNInputBufferRDG = NNOutputBufferRDG = nullptr;
	bInitialized = false;
}

MIGIRenderingContext & MIGIRenderingContext::Get ()
{
	static MIGIRenderingContext Context;
	return Context;
}