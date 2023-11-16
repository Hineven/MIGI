#include "MIGIViewExtension.h"

static FMIGIViewExtension * Instance = nullptr;

FMIGIViewExtension* FMIGIViewExtension::Get()
{
	if(Instance == nullptr)
		Instance = new FMIGIViewExtension();
	return Instance;
}

void FMIGIViewExtension::Release()
{
	if(Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
