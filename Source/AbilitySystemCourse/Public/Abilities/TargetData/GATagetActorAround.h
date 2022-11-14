// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATagetActorAround.generated.h"

UCLASS()
class ABILITYSYSTEMCOURSE_API AGATagetActorAround : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true), Category = "Ground Select Properties")
	float SelectRadius;

public:
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
};
