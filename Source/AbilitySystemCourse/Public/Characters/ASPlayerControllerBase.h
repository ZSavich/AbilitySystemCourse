// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/ASAbilityTypes.h"
#include "GameFramework/PlayerController.h"
#include "ASPlayerControllerBase.generated.h"

UCLASS()
class ABILITYSYSTEMCOURSE_API AASPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability UI")
	void AddAbilityToUI(FGameplayAbilityInfo AbilityInfo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Ability")
	void TryActivateAbilityInUISlot(const int32 SlotNumber);
};
