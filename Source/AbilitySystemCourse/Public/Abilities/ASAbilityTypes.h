#pragma once

#include "CoreMinimal.h"
#include "ASAbilityTypes.generated.h"

class UASGameplayAbilityBase;

UENUM(BlueprintType)
enum class EAbilityCostType : uint8
{
	EACT_Health		UMETA(DisplayName = "Health"),
	EACT_Mana		UMETA(DisplayName = "Mana"),
	EACT_Strength	UMETA(DisplayName = "Strength"),

	EACT_MAX		UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FGameplayAbilityInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Info")
	float CooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Info")
	float Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Info")
	EAbilityCostType CostType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Info")
	UMaterialInstance* UIMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Info")
	TSubclassOf<UASGameplayAbilityBase> AbilityClass;

	FGameplayAbilityInfo();
	FGameplayAbilityInfo(const float InCooldownDuration, const float InCost, const EAbilityCostType InCostType
		, UMaterialInstance* InUIMaterial, TSubclassOf<UASGameplayAbilityBase> InAbilityClass);
};