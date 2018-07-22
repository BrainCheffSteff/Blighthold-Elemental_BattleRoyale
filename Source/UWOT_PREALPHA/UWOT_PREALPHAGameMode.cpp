// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UWOT_PREALPHAGameMode.h"
#include "UWOT_PREALPHACharacter.h"
#include "UObject/ConstructorHelpers.h"

AUWOT_PREALPHAGameMode::AUWOT_PREALPHAGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
