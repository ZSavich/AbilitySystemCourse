#include "Abilities/ASAbilityTypes.h"
#include "Abilities/ASGameplayAbilityBase.h"

FGameplayAbilityInfo::FGameplayAbilityInfo() : CooldownDuration(0.f), Cost(0.f), CostType(EAbilityCostType::EACT_Health),
                                               UIMaterial(nullptr), AbilityClass(nullptr)
{
}

FGameplayAbilityInfo::FGameplayAbilityInfo(const float InCooldownDuration, const float InCost,
	const EAbilityCostType InCostType, UMaterialInstance* InUIMaterial,
	TSubclassOf<UASGameplayAbilityBase> InAbilityClass) :
	CooldownDuration(InCooldownDuration), Cost(InCost), CostType(InCostType),
	UIMaterial(InUIMaterial), AbilityClass(InAbilityClass)
{
}
