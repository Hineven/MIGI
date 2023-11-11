#pragma once

#include "CoreMinimal.h"
#include "MathUtil.h"

bool IsMIGIEnabled ();
void SetMIGIEnabled (bool bEnabled);

FIntPoint GetMIGIDebugPixelCoords ();
void SetMIGIDebugPixelCoords (FIntPoint PixelCoords);

bool IsMIGIDebugEnabled ();
void SetMIGIDebugEnabled (bool bEnabled);

size_t GetMIGISharedBufferSize ();