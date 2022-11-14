// Fill out your copyright notice in the Description page of Project Settings.

#include "Private/Abilities/TargetData/GATargetDataGroundSelect.h"
#include "Abilities/GameplayAbility.h"
#include "Components/DecalComponent.h"

AGATargetDataGroundSelect::AGATargetDataGroundSelect()
{
	PrimaryActorTick.bCanEverTick = true;

	GroundDecal = CreateDefaultSubobject<UDecalComponent>("GroundDecalComponent");
	GroundDecal->DecalSize = FVector(SelectRadius);
	GroundDecal->SetupAttachment(GetRootComponent());

	SelectRadius = 200.f;
	bUseTraceComplex = false;
}

void AGATargetDataGroundSelect::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	if (!MasterPC)
	{
		MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo());
	}
	if (GroundDecal)
	{
		GroundDecal->DecalSize = FVector(SelectRadius);
	}
}

void AGATargetDataGroundSelect::ConfirmTargetingAndContinue()
{
	FVector ViewLocation;
	FGameplayAbilityTargetData_LocationInfo* CenterLocation = new FGameplayAbilityTargetData_LocationInfo();
	if (GroundDecal)
	{
		CenterLocation->TargetLocation.LiteralTransform = GroundDecal->GetComponentTransform();
		CenterLocation->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	}
	
	FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();
	if (GetGroundLocation(ViewLocation))
	{
		TArray<FOverlapResult> OverlapResults;
		TArray<TWeakObjectPtr<AActor>> OverlapActors;

		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = bUseTraceComplex;
		QueryParams.bReturnPhysicalMaterial = false;
		if (MasterPC && MasterPC->GetPawn())
		{
			const AActor* MasterActor = MasterPC->GetPawn();
			QueryParams.AddIgnoredActor(MasterActor->GetUniqueID());
		}

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
			TargetData.Add(CenterLocation);
			TargetDataReadyDelegate.Broadcast(TargetData);
			return;
		}
	}

	TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle(CenterLocation));
}

void AGATargetDataGroundSelect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector ViewLocation;
	GetGroundLocation(ViewLocation);
	if (GroundDecal)
	{
		GroundDecal->SetWorldLocation(ViewLocation);
	}
}

bool AGATargetDataGroundSelect::GetGroundLocation(FVector& OutGroundLocation)
{
	if (!MasterPC) return false;

	FVector ViewLocation;
	FRotator ViewRotation;
	MasterPC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	if (const AActor* MasterPawn = MasterPC->GetPawn())
	{
		QueryParams.AddIgnoredActor(MasterPawn);
	}

	const bool bIsTraceSuccessful = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ViewLocation,
		ViewLocation + ViewRotation.Vector() * 10000.f,
		ECC_Visibility,
		QueryParams);

	if (bIsTraceSuccessful)
	{
		OutGroundLocation = HitResult.ImpactPoint;
		return true;
	}
	else
	{
		OutGroundLocation = FVector::Zero();
	}
	return false;
}
