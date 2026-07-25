// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectLord.h"
#include "Modules/ModuleManager.h"

#include "Internationalization/StringTableRegistry.h"

void FProjectLordModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	// New StringTables need to be added here
	LOCTABLE_FROMFILE_GAME("Ability", "AnyOldNamespace", "Localization/ST_Ability.csv");
}


void FProjectLordModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE( FProjectLordModule, ProjectLord, "ProjectLord" );
