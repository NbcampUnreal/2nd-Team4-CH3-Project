﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pepccine : ModuleRules
{
	public Pepccine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "UMG", "AnimGraphRuntime", "Slate", "SlateCore", "NavigationSystem" });
		PrivateIncludePaths.Add("Pepccine");
    }
}
