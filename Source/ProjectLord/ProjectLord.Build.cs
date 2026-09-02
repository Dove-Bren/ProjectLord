// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectLord : ModuleRules
{
	public ProjectLord(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"AIModule",
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
            "GameplayTasks",
            "InputCore",
			"ModelViewViewModel",
			"UMG",
			"NavigationSystem",
			"Landscape",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
