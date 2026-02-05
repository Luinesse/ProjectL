// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectL : ModuleRules
{
	public ProjectL(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "MotionWarping" });
	}
}
