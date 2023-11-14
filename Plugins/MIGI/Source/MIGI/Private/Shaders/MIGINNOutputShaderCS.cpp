#include "MIGIShaders.h"

IMPLEMENT_GLOBAL_SHADER(MIGINNInputShaderCS, "/Plugin/MIGI/Shaders/Private/NNInterface.usf", "NNOutput", EShaderFrequency::SF_Compute);