#pragma once

#ifndef MIGI_DIFFUSE_INDIRECT_H
#define MIGI_DIFFUSE_INDIRECT_H

#include "CoreMinimal.h"
#include "DeferredShadingRenderer.h"

void MIGIRenderDiffuseIndirect (const FScene& , const FViewInfo& , FRDGBuilder& , FGlobalIlluminationPluginResources& );

#endif // MIGI_DIFFUSE_INDIRECT_H