// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RED : ModuleRules
{
	public RED(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"Slate",
			"SlateCore",
            "UMG",
			"Niagara",
            "FogOfWar",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Sockets",
            "GameplayTags",
            "AIModule",
        });

		PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
