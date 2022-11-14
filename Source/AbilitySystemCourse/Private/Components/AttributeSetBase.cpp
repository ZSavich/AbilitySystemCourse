// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Characters/ASCharacterBase.h"

UAttributeSetBase::UAttributeSetBase() :
	Health(200.f), MaxHealth(200.f),
	Mana(200.f), MaxMana(200.f),
	Strength(200.f), MaxStrength(200.f)
{
}

void UAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if(Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(),GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Health)))
	{
		Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, MaxHealth.GetCurrentValue()));
		Health.SetBaseValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, MaxHealth.GetCurrentValue()));
		OnHealthChanged.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());

		if (const auto Owner = Cast<AASCharacterBase>(GetOwningActor()))
		{
			if (Health.GetCurrentValue() == MaxHealth.GetCurrentValue())
			{
				Owner->AddGameplayTag(Owner->GetFullHealthTag());
			}
			else
			{
				Owner->RemoveGameplayTag(Owner->GetFullHealthTag());
			}
		}
	}

	if(Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(),GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Mana)))
	{
		Mana.SetCurrentValue(FMath::Clamp(Mana.GetCurrentValue(), 0.f, MaxMana.GetCurrentValue()));
		Mana.SetBaseValue(FMath::Clamp(Mana.GetCurrentValue(), 0.f, MaxMana.GetCurrentValue()));
		OnManaChanged.Broadcast(Mana.GetCurrentValue(), MaxMana.GetCurrentValue());
	}

	if(Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(),GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Strength)))
	{
		Strength.SetCurrentValue(FMath::Clamp(Strength.GetCurrentValue(), 0.f, MaxStrength.GetCurrentValue()));
		Strength.SetBaseValue(FMath::Clamp(Strength.GetCurrentValue(), 0.f, MaxStrength.GetCurrentValue()));
		OnStrengthChanged.Broadcast(Strength.GetCurrentValue(), MaxStrength.GetCurrentValue());
	}
}
