#include "MIGIShaders.h"

IMPLEMENT_GLOBAL_SHADER(MIGINNInputShaderCS,
	"/Plugin/MIGI/Private/NNInterface.usf", "NNInput",
	EShaderFrequency::SF_Compute);