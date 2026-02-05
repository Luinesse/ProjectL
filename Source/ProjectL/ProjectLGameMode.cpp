// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectLGameMode.h"
#include "ProjectLCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectLGameMode::AProjectLGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
