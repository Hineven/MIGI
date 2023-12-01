#include "MIGIViewUniformBufferExtension.h"

static FMIGIViewUniformBufferExtension * Instance = nullptr;

FMIGIViewUniformBufferExtension* FMIGIViewUniformBufferExtension::Get()
{
	if(Instance == nullptr)
		Instance = new FMIGIViewUniformBufferExtension();
	return Instance;
}

void FMIGIViewUniformBufferExtension::Release()
{
	if(Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
