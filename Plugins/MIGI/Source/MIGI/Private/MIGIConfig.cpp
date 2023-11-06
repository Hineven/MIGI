#include "MIGIConfig.h"

TAutoConsoleVariable<bool> CVarMIGIEnabled(TEXT("r.MIGI.Enabled"), 0, TEXT("Enable MIGI. 0: Disable, 1: Enable"), ECVF_RenderThreadSafe);
TAutoConsoleVariable<bool> CVarMIGIDebugEnabled(TEXT("r.MIGI.DebugEnabled"), 0, TEXT("Enable MIGI Debug. 0: Disable, 1: Enable"), ECVF_RenderThreadSafe);
TAutoConsoleVariable<int> CVarMIGIDebugPixelCoordsX(TEXT("r.MIGI.DebugPixelCoordsX"), 0, TEXT("X coordinate of the pixel to debug MIGI"), ECVF_RenderThreadSafe);
TAutoConsoleVariable<int> CVarMIGIDebugPixelCoordsY(TEXT("r.MIGI.DebugPixelCoordsY"), 0, TEXT("Y coordinate of the pixel to debug MIGI"), ECVF_RenderThreadSafe);
bool IsMIGIEnabled() {
    return CVarMIGIEnabled.GetValueOnRenderThread();
}
inline void SetMIGIEnabled(bool bEnabled)
{
    CVarMIGIEnabled->Set(bEnabled);
}
// Implement the getter and setter for the debug pixel coordinates
FIntPoint GetMIGIDebugPixelCoords ()
{
    return FIntPoint(CVarMIGIDebugPixelCoordsX.GetValueOnRenderThread(), CVarMIGIDebugPixelCoordsY.GetValueOnRenderThread());
}
void SetMIGIDebugPixelCoords(FIntPoint PixelCoords)
{
    CVarMIGIDebugPixelCoordsX->Set(PixelCoords.X);
    CVarMIGIDebugPixelCoordsY->Set(PixelCoords.Y);
}
bool IsMIGIDebugEnabled()
{
    return CVarMIGIDebugEnabled.GetValueOnRenderThread();
}
void SetMIGIDebugEnabled(bool bEnabled)
{
    CVarMIGIDebugEnabled->Set(bEnabled);
}