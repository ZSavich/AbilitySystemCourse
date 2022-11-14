// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATargetDataGroundSelect.generated.h"

UCLASS(Blueprintable)
class ABILITYSYSTEMCOURSE_API AGATargetDataGroundSelect : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true), Category = "Ground Select Properties")
	float SelectRadius;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true), Category = "Ground Select Properties")
	bool bUseTraceComplex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* GroundDecal;

public:
	AGATargetDataGroundSelect();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	/// Searches enemies that within the SelectRadius
	virtual void ConfirmTargetingAndContinue() override;

	/// Draw Debug Sphere for Select Area
	virtual void Tick(float DeltaSeconds) override;

	/// Returns the location where the player is looking
	/// and where the line tracing is being blocked by the enemy
	UFUNCTION(BlueprintCallable, Category = "Ground Select")
	bool GetGroundLocation(FVector& OutGroundLocation);
};
