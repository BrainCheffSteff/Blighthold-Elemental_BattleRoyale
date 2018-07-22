// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UWOT_PREALPHATarget : TargetRules
{
	public UWOT_PREALPHATarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bForceEnableExceptions = true; 
		ExtraModuleNames.Add("UWOT_PREALPHA");
	}
}
