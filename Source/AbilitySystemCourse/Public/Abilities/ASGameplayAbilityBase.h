// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASAbilityTypes.h"
#include "Abilities/GameplayAbility.h"
#include "ASGameplayAbilityBase.generated.h"

class AASCharacterBase;

UCLASS()
class ABILITYSYSTEMCOURSE_API UASGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	FGameplayAbilityInfo GetAbilityInfo();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Info")
	UMaterialInstance* UIMaterial;
	
	UFUNCTION(BlueprintCallable)
	void GetPlayerInfo(AASCharacterBase*& OutCharacter, APlayerController*& OutController);

	UFUNCTION(BlueprintCallable)
	bool IsCostAvailable();
};
