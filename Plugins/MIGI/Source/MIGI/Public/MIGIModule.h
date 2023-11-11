#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMIGIModule : public IModuleInterface
{
public:
	virtual void StartupModule () override;
	virtual void ShutdownModule () override;
	bool isActive () const;
	
	static inline FMIGIModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FMIGIModule >("MIGI");
	}
	
protected:
	bool ActivateMIGI ();
	
	bool bModuleActive {false};
	bool bAdapterActive {false};
	bool bCUDAActive {false};
	FDelegateHandle DiffuseIndirectDelegateHandle {};
};
