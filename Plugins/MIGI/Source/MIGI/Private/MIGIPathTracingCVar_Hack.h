#pragma once

#include "CoreMinimal.h"

// This is a hack to get references to CVars defined inside PathTracing.cpp
// The plugin is loaded in very early stages of the engine, so we can't simply load them with static constructors.


// MIGILoadPathTracingCommonCVars is pending on later stages of the engine loading.

void MIGILoadPathTracingCommonCVars();

extern TConsoleVariableData<int32> * CVarPathTracing;

extern TConsoleVariableData<int32> * CVarPathTracingCompaction;

extern TConsoleVariableData<int32> * CVarPathTracingIndirectDispatch;

extern TConsoleVariableData<int32> * CVarPathTracingFlushDispatch;

extern TConsoleVariableData<int32> * CVarPathTracingDispatchSize;

extern TConsoleVariableData<int32> * CVarPathTracingMaxBounces;

extern TConsoleVariableData<int32> * CVarPathTracingSamplesPerPixel;

extern TConsoleVariableData<float> * CVarPathTracingFilterWidth;

extern TConsoleVariableData<int32> * CVarPathTracingMISMode;

extern TConsoleVariableData<int32> * CVarPathTracingVolumeMISMode;

extern TConsoleVariableData<int32> * CVarPathTracingMaxRaymarchSteps;

extern TConsoleVariableData<int32> * CVarPathTracingMISCompensation;

extern TConsoleVariableData<int32> * CVarPathTracingSkylightCaching;

extern TConsoleVariableData<int32> * CVarPathTracingVisibleLights;

extern TConsoleVariableData<int32> * CVarPathTracingMaxSSSBounces;

extern TConsoleVariableData<float> * CVarPathTracingSSSGuidingRatio;

extern TConsoleVariableData<float> * CVarPathTracingMaxPathIntensity;

extern TConsoleVariableData<int32> * CVarPathTracingApproximateCaustics;

extern TConsoleVariableData<int32> * CVarPathTracingEnableEmissive;

extern TConsoleVariableData<int32> * CVarPathTracingEnableCameraBackfaceCulling;

extern TConsoleVariableData<int32> * CVarPathTracingAtmosphereOpticalDepthLutResolution;

extern TConsoleVariableData<int32> * CVarPathTracingAtmosphereOpticalDepthLutNumSamples;

extern TConsoleVariableData<int32> * CVarPathTracingFrameIndependentTemporalSeed;

// See PATHTRACER_SAMPLER_* defines
extern TConsoleVariableData<int32> * CVarPathTracingSamplerType;

extern TConsoleVariableData<int32> * CVarPathTracingWiperMode;

extern TConsoleVariableData<int32> * CVarPathTracingProgressDisplay;

extern TConsoleVariableData<int32> * CVarPathTracingLightGridResolution;

extern TConsoleVariableData<int32> * CVarPathTracingLightGridMaxCount;

extern TConsoleVariableData<int32> * CVarPathTracingLightGridVisualize;

extern TConsoleVariableData<int32> * CVarPathTracingDecalGridVisualize;

extern TConsoleVariableData<int32> * CVarPathTracingUseDBuffer;

extern TConsoleVariableData<float> * CVarPathTracingDecalRoughnessCutoff;

extern TConsoleVariableData<float> * CVarPathTracingMeshDecalRoughnessCutoff;

extern TConsoleVariableData<float> * CVarPathTracingMeshDecalBias;

extern TConsoleVariableData<int32> * CVarPathTracingLightFunctionColor;

extern TConsoleVariableData<int32> * CVarPathTracingHeterogeneousVolumes;

extern TConsoleVariableData<int32> * CVarPathTracingHeterogeneousVolumesRebuildEveryFrame;

extern TConsoleVariableData<int32> * CVarPathTracingCameraMediumTracking;

extern TConsoleVariableData<int32> * CVarPathTracingOutputPostProcessResources;

extern TConsoleVariableData<int32> * CVarPathTracingSubstrateUseSimplifiedMaterial;

extern TConsoleVariableData<int32> * CVarPathTracingSubstrateCompileSimplifiedMaterial;

extern TConsoleVariableData<int32> * CVarpathTracingOverrideDepth;

extern TConsoleVariableData<int32> * CVarPathTracingUseAnalyticTransmittance;