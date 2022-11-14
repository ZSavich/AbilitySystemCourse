// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/TargetData/GATagetActorAround.h"

#include "Abilities/GameplayAbility.h"

void AGATagetActorAround::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	if (!MasterPC)
	{
		MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo());
	}
}

void AGATagetActorAround::ConfirmTargetingAndContinue()
{
	if (!MasterPC) return;
	if (const APawn* MasterPawn = MasterPC->GetPawn())
	{
		const FVector ViewLocation = MasterPawn->GetActorLocation();
		FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();

		TArray<FOverlapResult> OverlapResults;
		TArray<TWeakObjectPtr<AActor>> OverlapActors;

		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = false;
		QueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());

		const bool bTryOverlapObjects = GetWorld()->OverlapMultiByObjectType(
			OverlapResults,
			ViewLocation,
			FQuat::Identity,
			FCollisionObjectQueryParams(ECC_Pawn),
			FCollisionShape::MakeSphere(SelectRadius),
			QueryParams);

		if (bTryOverlapObjects)
		{
			for (const auto& Result : OverlapResults)
			{
				AActor* ResultActor = Result.GetActor();
				if (ResultActor && !OverlapActors.Contains(ResultActor))
				{
					OverlapActors.AddUnique(ResultActor);
				}
			}
		}

		if (!OverlapActors.IsEmpty())
		{
			TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlapActors);
			TargetDataReadyDelegate.Broadcast(TargetData);
			return;
		}
	}

	TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
}
