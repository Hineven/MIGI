#include "MIGIPathTracingCVar_Hack.h"

TConsoleVariableData<int32> * CVarPathTracing;
TConsoleVariableData<int32> * CVarPathTracingCompaction;
TConsoleVariableData<int32> * CVarPathTracingIndirectDispatch;
TConsoleVariableData<int32> * CVarPathTracingFlushDispatch;
TConsoleVariableData<int32> * CVarPathTracingDispatchSize;
TConsoleVariableData<int32> * CVarPathTracingMaxBounces;
TConsoleVariableData<int32> * CVarPathTracingSamplesPerPixel;
TConsoleVariableData<float> * CVarPathTracingFilterWidth;
TConsoleVariableData<int32> * CVarPathTracingMISMode;
TConsoleVariableData<int32> * CVarPathTracingVolumeMISMode;
TConsoleVariableData<int32> * CVarPathTracingMaxRaymarchSteps;
TConsoleVariableData<int32> * CVarPathTracingMISCompensation;
TConsoleVariableData<int32> * CVarPathTracingSkylightCaching;
TConsoleVariableData<int32> * CVarPathTracingVisibleLights;
TConsoleVariableData<int32> * CVarPathTracingMaxSSSBounces;
TConsoleVariableData<float> * CVarPathTracingSSSGuidingRatio;
TConsoleVariableData<float> * CVarPathTracingMaxPathIntensity;
TConsoleVariableData<int32> * CVarPathTracingApproximateCaustics;
TConsoleVariableData<int32> * CVarPathTracingEnableEmissive;
TConsoleVariableData<int32> * CVarPathTracingEnableCameraBackfaceCulling;
TConsoleVariableData<int32> * CVarPathTracingAtmosphereOpticalDepthLutResolution;
TConsoleVariableData<int32> * CVarPathTracingAtmosphereOpticalDepthLutNumSamples;
TConsoleVariableData<int32> * CVarPathTracingFrameIndependentTemporalSeed;
// See PATHTRACER_SAMPLER_* defines
TConsoleVariableData<int32> * CVarPathTracingSamplerType;
TConsoleVariableData<int32> * CVarPathTracingWiperMode;
TConsoleVariableData<int32> * CVarPathTracingProgressDisplay;
TConsoleVariableData<int32> * CVarPathTracingLightGridResolution;
TConsoleVariableData<int32> * CVarPathTracingLightGridMaxCount;
TConsoleVariableData<int32> * CVarPathTracingLightGridVisualize;
TConsoleVariableData<int32> * CVarPathTracingDecalGridVisualize;
TConsoleVariableData<int32> * CVarPathTracingUseDBuffer;
TConsoleVariableData<float> * CVarPathTracingDecalRoughnessCutoff;
TConsoleVariableData<float> * CVarPathTracingMeshDecalRoughnessCutoff;
TConsoleVariableData<float> * CVarPathTracingMeshDecalBias;
TConsoleVariableData<int32> * CVarPathTracingLightFunctionColor;
TConsoleVariableData<int32> * CVarPathTracingHeterogeneousVolumes;
TConsoleVariableData<int32> * CVarPathTracingHeterogeneousVolumesRebuildEveryFrame;
TConsoleVariableData<int32> * CVarPathTracingCameraMediumTracking;
TConsoleVariableData<int32> * CVarPathTracingOutputPostProcessResources;
TConsoleVariableData<int32> * CVarPathTracingSubstrateUseSimplifiedMaterial;
TConsoleVariableData<int32> * CVarPathTracingSubstrateCompileSimplifiedMaterial;
TConsoleVariableData<int32> * CVarpathTracingOverrideDepth;
TConsoleVariableData<int32> * CVarPathTracingUseAnalyticTransmittance;

void MIGILoadPathTracingCommonCVars () {
    CVarPathTracing =  IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing"));
    CVarPathTracingCompaction = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.Compaction"));
    CVarPathTracingIndirectDispatch = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.IndirectDispatch"));
    
    CVarPathTracingFlushDispatch = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.FlushDispatch"));
    
    CVarPathTracingDispatchSize = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.DispatchSize"));
    
    CVarPathTracingMaxBounces = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.MaxBounces"));
    
    CVarPathTracingSamplesPerPixel = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.SamplesPerPixel"));
    
    CVarPathTracingFilterWidth = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.PathTracing.FilterWidth"));

    CVarPathTracingMISMode = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.MISMode"));

    CVarPathTracingVolumeMISMode = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.VolumeMISMode"));
    
    CVarPathTracingMaxRaymarchSteps = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.MaxRaymarchSteps"));

    CVarPathTracingMISCompensation = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.MISCompensation"));

    CVarPathTracingSkylightCaching = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.SkylightCaching"));

    CVarPathTracingVisibleLights = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.VisibleLights"));

    CVarPathTracingMaxSSSBounces = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.MaxSSSBounces"));

    CVarPathTracingSSSGuidingRatio = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.PathTracing.SSSGuidingRatio"));

    CVarPathTracingMaxPathIntensity = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.PathTracing.MaxPathIntensity"));

    CVarPathTracingApproximateCaustics = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.ApproximateCaustics"));

    CVarPathTracingEnableEmissive = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.EnableEmissive"));

    CVarPathTracingEnableCameraBackfaceCulling = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.EnableCameraBackfaceCulling"));

    CVarPathTracingAtmosphereOpticalDepthLutResolution = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.AtmosphereOpticalDepthLUTResolution"));

    CVarPathTracingAtmosphereOpticalDepthLutNumSamples = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.AtmosphereOpticalDepthLUTNumSamples"));

    CVarPathTracingFrameIndependentTemporalSeed = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.FrameIndependentTemporalSeed"));

    // See PATHTRACER_SAMPLER_* defines
    CVarPathTracingSamplerType = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.SamplerType"));

    CVarPathTracingWiperMode = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.WiperMode"));

    CVarPathTracingProgressDisplay = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.ProgressDisplay"));

    CVarPathTracingLightGridResolution = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.LightGridResolution"));

    CVarPathTracingLightGridMaxCount = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.LightGridMaxCount"));

    
    CVarPathTracingLightGridVisualize = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.LightGridVisualize"));
    CVarPathTracingDecalGridVisualize = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.DecalGrid.Visualize"));

    CVarPathTracingUseDBuffer = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.UseDBuffer"));
    
    CVarPathTracingDecalRoughnessCutoff = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.PathTracing.DecalRoughnessCutoff"));
    
    CVarPathTracingMeshDecalRoughnessCutoff = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.PathTracing.MeshDecalRoughnessCutoff"));

    CVarPathTracingMeshDecalBias = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.PathTracing.MeshDecalBias"));

    CVarPathTracingLightFunctionColor = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.LightFunctionColor"));

    
    CVarPathTracingHeterogeneousVolumes = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.HeterogeneousVolumes"));

    CVarPathTracingHeterogeneousVolumesRebuildEveryFrame = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.HeterogeneousVolumes.RebuildEveryFrame"));

    CVarPathTracingCameraMediumTracking = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.CameraMediumTracking"));

    CVarPathTracingOutputPostProcessResources = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.OutputPostProcessResources"));

    CVarPathTracingSubstrateUseSimplifiedMaterial = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.Substrate.UseSimplifiedMaterials"));
    CVarPathTracingSubstrateCompileSimplifiedMaterial = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.Substrate.CompileSimplifiedMaterials"));
    CVarpathTracingOverrideDepth = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.Override.Depth"));
    CVarPathTracingUseAnalyticTransmittance = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PathTracing.UseAnalyticTransmittance"));
}