// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UWOT_PREALPHA : ModuleRules
{
	public UWOT_PREALPHA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Voxel" });
        PrivateIncludePathModuleNames.AddRange(new string[] { "Voxel" });
        PrivateIncludePaths.AddRange(new string[] { "Voxel/Public", "Voxel/Classes", "Voxel/Private" });
    }
}
