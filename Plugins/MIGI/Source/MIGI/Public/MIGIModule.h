#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMIGIModule : public IModuleInterface
{
public:
	virtual void StartupModule () override;
	virtual void ShutdownModule () override;
	
	static inline FMIGIModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FMIGIModule >("MIGI");
	}
	
	void InitializeMIGI ();
protected:
	bool bModuleFunctional {false};
};
