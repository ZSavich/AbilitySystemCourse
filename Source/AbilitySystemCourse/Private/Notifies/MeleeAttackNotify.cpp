// Fill out your copyright notice in the Description page of Project Settings.

#include "Notifies/MeleeAttackNotify.h"

#include "Characters/ASCharacterBase.h"

void UMeleeAttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration)
{
	if (MeshComp)
	{
		if (auto Character = MeshComp->GetOwner<AASCharacterBase>())
		{
			Character->SetSwordCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UMeleeAttackNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp)
	{
		if (auto Character = MeshComp->GetOwner<AASCharacterBase>())
		{
			Character->SetSwordCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	Super::NotifyEnd(MeshComp, Animation);
}
