// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeSetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaChanged, float, Mana, float, MaxMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStrengthChanged, float, Strength, float, MaxStrength);

UCLASS()
class ABILITYSYSTEMCOURSE_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAttributeSetBase();

	/** Health params **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayAttributeData Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayAttributeData MaxHealth;

	/** Mana params **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayAttributeData Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayAttributeData MaxMana;

	/** Strength params **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayAttributeData Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayAttributeData MaxStrength;

	FOnHealthChanged OnHealthChanged;
	FOnManaChanged OnManaChanged;
	FOnStrengthChanged OnStrengthChanged;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
