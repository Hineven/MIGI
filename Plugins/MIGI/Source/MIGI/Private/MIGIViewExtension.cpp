#include "MIGIViewExtension.h"

static TSharedPtr<FMIGIViewExtension> Instance = nullptr;

void FMIGIViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	// Do nothing.
}

void FMIGIViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	// Do nothing.
}

void FMIGIViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	// Do nothing.
}

void FMIGIViewExtension::Set(TSharedPtr<FMIGIViewExtension> InMIGIViewExtension)
{
	Instance = InMIGIViewExtension;
}


FMIGIViewExtension* FMIGIViewExtension::Get()
{
	// Use FViewExtensions::NewExtension<FMIGIViewExtension>() to create one,
	// call FMIGIViewExtension::Set(SharedPtr) to set the unique extension and
	// later get it from elsewhere.
	check(Instance != nullptr);
	return Instance.Get();
}

void FMIGIViewExtension::Release()
{
	Instance = nullptr;
}