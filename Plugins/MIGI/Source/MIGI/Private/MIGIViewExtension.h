#pragma once
#include "CoreMinimal.h"

class FMIGIViewExtension : public IPersistentViewUniformBufferExtension, public FNoncopyable
{
public:
	// Uhmm, nothing here currently.

	static FMIGIViewExtension * Get ();
	// Release the unique instance.
	static void Release () ;

	virtual ~FMIGIViewExtension() {}
protected:
	FMIGIViewExtension () = default;
};