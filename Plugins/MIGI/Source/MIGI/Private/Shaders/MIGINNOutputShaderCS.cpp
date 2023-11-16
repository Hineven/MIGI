#include "MIGIShaders.h"

IMPLEMENT_GLOBAL_SHADER(MIGINNOutputShaderCS,
	"/Plugin/MIGI/Private/NNInterface.usf", "NNOutput", EShaderFrequency::SF_Compute);