#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMIGIModule : public IModuleInterface
{
public:
	virtual void StartupModule () override;
	virtual void ShutdownModule () override;
	void InitializeMIGI ();
protected:
	bool bModuleFunctional {false};
};
