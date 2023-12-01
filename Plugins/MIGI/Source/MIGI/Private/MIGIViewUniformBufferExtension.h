#pragma once
#include "CoreMinimal.h"


// This class is used to gather view info for MIGI.
// It can be seen as a hack, however it's the only way I found.
class FMIGIViewUniformBufferExtension : public IPersistentViewUniformBufferExtension, public FNoncopyable
{
public:
	TArray<FViewInfo> * SceneRendererViewsArrayCopy;

	static FMIGIViewUniformBufferExtension * Get ();
	// Release the unique instance.
	static void Release () ;
	virtual void PrepareView(const FSceneView* View) override;

	virtual ~FMIGIViewUniformBufferExtension() {}
protected:
	FMIGIViewUniformBufferExtension () = default;
};