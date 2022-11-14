 // Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AbilitySystemCourse : ModuleRules
{
	public AbilitySystemCourse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "GameplayAbilities", "GameplayTags", "GameplayTasks","AIModule" });
		
		PublicIncludePaths.AddRange(new string[]
		{
			"AbilitySystemCourse/Characters",
		});
	}
}
