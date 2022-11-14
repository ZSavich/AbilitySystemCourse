// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystemCourseGameMode.h"
#include "AbilitySystemCourseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAbilitySystemCourseGameMode::AAbilitySystemCourseGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
