#pragma once

#include "CoreMinimal.h"
#include "Shader.h"
#include "ShaderParameterStruct.h"

class MIGINNInputShaderCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(MIGINNInputShaderCS);
	SHADER_USE_PARAMETER_STRUCT(MIGINNInputShaderCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(MIGINNInputShaderCS, NNInputBuffer)
		SHADER_PARAMETER(FVector4f, TestParam)
	END_SHADER_PARAMETER_STRUCT()
};

class MIGINNOutputShaderCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(MIGINNOutputShaderCS);
	SHADER_USE_PARAMETER_STRUCT(MIGINNOutputShaderCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(MIGINNOutputShaderCS, NNOutputBuffer)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(MIGINNOutputShaderCS, ColorBuffer)
	END_SHADER_PARAMETER_STRUCT()
};