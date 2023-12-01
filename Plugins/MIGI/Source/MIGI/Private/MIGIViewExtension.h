#pragma once
#include "CoreMinimal.h"
#include "SceneViewExtension.h"


// This class is used to gather view info for MIGI.
// It can be seen as a hack, however it's the only way I found.
class FMIGIViewExtension : public ISceneViewExtension, public FNoncopyable
{
public:
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

	static FMIGIViewExtension * Get ();
	static void Set(TSharedPtr<FMIGIViewExtension> InMIGIViewExtension);
	// Release the unique instance.
	static void Release () ;

	virtual ~FMIGIViewExtension() {}
protected:
	FMIGIViewExtension () = default;
};