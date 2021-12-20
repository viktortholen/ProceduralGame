// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGameMode.h"
#include "ProceduralPlayerController.h"
#include "ProceduralCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProceduralGameMode::AProceduralGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AProceduralPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}