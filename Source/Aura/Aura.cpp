// Copyright Berkeley Bidwell

#include "Aura.h"
#include "Modules/ModuleManager.h"

/**
 * FAura
 */
class FAura : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAura, Aura, "Aura");