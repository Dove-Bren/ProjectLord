// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectLord.h"
#include "Modules/ModuleManager.h"

#include "Internationalization/StringTableRegistry.h"

void FProjectLordModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	// New StringTables need to be added here
	LOCTABLE_FROMFILE_GAME("Ability", "AnyOldNamespace", "Localization/ST_Ability.csv");
	LOCTABLE_FROMFILE_GAME("Creature", "ProjectLord", "Localization/ST_Creature.csv");
	LOCTABLE_FROMFILE_GAME("Building", "ProjectLord", "Localization/ST_Building.csv");
	LOCTABLE_FROMFILE_GAME("Item", "ProjectLord", "Localization/ST_Item.csv");
	LOCTABLE_FROMFILE_GAME("UI", "ProjectLord", "Localization/ST_UI.csv");
}


void FProjectLordModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE( FProjectLordModule, ProjectLord, "ProjectLord" );
