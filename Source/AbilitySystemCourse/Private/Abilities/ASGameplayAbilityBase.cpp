// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/ASGameplayAbilityBase.h"
#include "Characters/ASCharacterBase.h"
#include "Components/AttributeSetBase.h"

void UASGameplayAbilityBase::GetPlayerInfo(AASCharacterBase*& OutCharacter, APlayerController*& OutController)
{
	if (const auto OwningCharacter = Cast<AASCharacterBase>(GetOwningActorFromActorInfo()))
	{
		OutCharacter = OwningCharacter;
		if (const auto OwningController = OwningCharacter->GetController<APlayerController>())
		{
			OutController = OwningController;
		}
	}
}

bool UASGameplayAbilityBase::IsCostAvailable()
{
	if (const auto OwningCharacter = Cast<AASCharacterBase>(GetOwningActorFromActorInfo()))
	{
		if (const auto AttributeComponent = OwningCharacter->GetAttributeComponent())
		{
			const FGameplayAbilityInfo& AbilityInfo = GetAbilityInfo();
			const float AbilityCost = FMath::Abs(AbilityInfo.Cost);
			float CurrentValue = 0;

			switch (AbilityInfo.CostType)
			{
				case EAbilityCostType::EACT_Health:
					CurrentValue = AttributeComponent->Health.GetCurrentValue();
					break;
				case EAbilityCostType::EACT_Mana:
					CurrentValue = AttributeComponent->Mana.GetCurrentValue();
					break;
				case EAbilityCostType::EACT_Strength:
					CurrentValue = AttributeComponent->Strength.GetCurrentValue();
					break;
				default: ;
			}

			return CurrentValue > AbilityCost;
		}
	}
	return false;
}

FGameplayAbilityInfo UASGameplayAbilityBase::GetAbilityInfo()
{
	const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();
	EAbilityCostType CostType = EAbilityCostType::EACT_Health;
	if (CooldownEffect && CostEffect)
	{
		float CooldownDuration = 0;
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);

		float Cost = 0;
		if (CostEffect->Modifiers.Num() > 0)
		{
			CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
			const FString AttributeName = CostEffect->Modifiers[0].Attribute.AttributeName;
			if (AttributeName == "Health")
			{
				CostType = EAbilityCostType::EACT_Health;
			}
			else if (AttributeName == "Mana")
			{
				CostType = EAbilityCostType::EACT_Mana;
			}
			else if (AttributeName == "Strength")
			{
				CostType = EAbilityCostType::EACT_Strength;
			}
		} 
		return FGameplayAbilityInfo(CooldownDuration, Cost, CostType, UIMaterial, GetClass());
	}
	return FGameplayAbilityInfo();
}
