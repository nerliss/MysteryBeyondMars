// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNGameMode.h"
#include "ProjectNCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectNGameMode::AProjectNGameMode()
{
	// set default pawn class to our Blueprinted character
// 	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("ProjectNCharacter.h"));
// 	if (PlayerPawnBPClass.Class != NULL)
// 	{
// 		DefaultPawnClass = PlayerPawnBPClass.Class;
// 	}
}
