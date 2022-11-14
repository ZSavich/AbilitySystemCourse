// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/ASCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/ASPlayerControllerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeSetBase.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/ASGameplayAbilityBase.h"

AASCharacterBase::AASCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetRootComponent());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);

	SwordCollision = CreateDefaultSubobject<UCapsuleComponent>("SwordCollision");
	SwordCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SwordCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SwordCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordCollision->SetupAttachment(GetMesh(), FName("hand_r"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");

	AttributeComponent = CreateDefaultSubobject<UAttributeSetBase>("AttributeComponent");

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bIsDeath = false;
}

void AASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	if (SwordCollision)
	{
		SwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AASCharacterBase::HandleOnSwordBeginOverlap);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AASCharacterBase::HandleOnCapsuleBeginOverlap);
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnHealthChanged.AddDynamic(this, &AASCharacterBase::HandleOnHealthChanged);
		AttributeComponent->OnManaChanged.AddDynamic(this, &AASCharacterBase::HandleOnManaChanged);
		AttributeComponent->OnStrengthChanged.AddDynamic(this, &AASCharacterBase::HandleOnStrengthChanged);
	}

	AcquireAbilities(InitialAbilities);
	AddGameplayTag(FullHealthTag);
}

void AASCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AASCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AASCharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("TurnRight", this, &AASCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AASCharacterBase::LookUpRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AASCharacterBase::MeleeAttack);
	PlayerInputComponent->BindAction("Cancel", IE_Pressed, this, &AASCharacterBase::CancelAbility);
}

void AASCharacterBase::MoveForward(const float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AASCharacterBase::MoveRight(const float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AASCharacterBase::TurnAtRate(const float Rate)
{
	AddControllerYawInput(Rate);
}

void AASCharacterBase::LookUpRate(const float Rate)
{
	AddControllerPitchInput(Rate);
}

void AASCharacterBase::SwitchInputControl(const bool bIsEnable)
{
	if (const auto PlayerController = GetController<APlayerController>())
	{
		if (!bIsEnable)
		{
			PlayerController->DisableInput(PlayerController);	
		}
		else if (bIsEnable && !bIsDeath)
		{
			PlayerController->EnableInput(PlayerController);
		}
	}
	else if (const auto AIController = GetController<AAIController>())
	{
		if (!bIsEnable)
		{
			AIController->GetBrainComponent()->StopLogic("Death");
		}
		else if (bIsEnable && !bIsDeath)
		{
			AIController->GetBrainComponent()->RestartLogic();
		}
	}
}

void AASCharacterBase::AddAbilityToUI(TSubclassOf<UASGameplayAbilityBase> AbilityToAdd) const
{
	if (const auto PlayerController = GetController<AASPlayerControllerBase>())
	{
		if (UASGameplayAbilityBase* AbilityInstance = AbilityToAdd.Get()->GetDefaultObject<UASGameplayAbilityBase>())
		{
			const FGameplayAbilityInfo AbilityInfo = AbilityInstance->GetAbilityInfo();
			PlayerController->AddAbilityToUI(AbilityInfo);
		}
	}
}

void AASCharacterBase::MeleeAttack()
{
	if (!AbilitySystemComponent) return;
	
	if (bIsTargeting)
	{
		AbilitySystemComponent->TargetConfirm();
	}
	else if (MeleeAttackAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(MeleeAttackAbility);
	}
}

void AASCharacterBase::CancelAbility()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TargetCancel();
		bIsTargeting = false;
	}
}

bool AASCharacterBase::IsHostile(const AActor* OtherActor)
{
	if (const auto OtherCharacter = Cast<AASCharacterBase>(OtherActor))
	{
		return TeamID != OtherCharacter->GetTeamID();
	}
	return false;
}

void AASCharacterBase::SetCollisionResponseToPawn(const ECollisionResponse CollisionResponse)
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, CollisionResponse);
	}
}

void AASCharacterBase::PushCharacter(const FVector Direction, const float Strength, const float StunDuration)
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		EndMeleeCombo();
		DefaultGroundFriction = MovementComponent->GroundFriction;
		MovementComponent->AddImpulse(Direction * Strength, true);
		HitStun(StunDuration);

		FTimerHandle PushCharacter_Timer;
		GetWorldTimerManager().SetTimer(PushCharacter_Timer, FTimerDelegate::CreateLambda(
			[&]() {GetCharacterMovement()->GroundFriction = DefaultGroundFriction;}
			), 1.f, false, 1.f);
	}
}

void AASCharacterBase::Death()
{
	if (DeathMontage && !bIsDeath)
	{
		const float DeathDelay = PlayAnimMontage(DeathMontage);
		SetLifeSpan(DeathDelay);
		bIsDeath = true;

		SwitchInputControl(false);
	}
}

void AASCharacterBase::HitStun(const float Duration)
{
	if (!bIsDeath && Duration > 0.f)
	{
		SwitchInputControl(false);

		FTimerHandle HitStun_Timer;
		GetWorldTimerManager().SetTimer(HitStun_Timer, FTimerDelegate::CreateLambda(
			[&](){SwitchInputControl(true);}
			), Duration, false);
	}
}

void AASCharacterBase::MakeCameraControlRotation(const bool bShouldControl)
{
	bUseControllerRotationYaw = bShouldControl;
	GetCharacterMovement()->bUseControllerDesiredRotation = !bShouldControl;
}

bool AASCharacterBase::DetermineCombo(UAnimMontage*& OutComboMontage, uint8& OutComboCount)
{
	if (MeleeComboMontages.Contains(ComboCount))
	{
		OutComboMontage = MeleeComboMontages[ComboCount];
		ComboCount++;
		OutComboCount = FMath::Clamp(ComboCount - 1, 0, MeleeComboMontages.Num());
		return true;
	}
	else if (!MeleeComboMontages.IsEmpty())
	{
		EndMeleeCombo();
		OutComboMontage = MeleeComboMontages[ComboCount];
		OutComboCount = FMath::Clamp(ComboCount - 1, 0, MeleeComboMontages.Num());
		return true;
	}
	return false;
}

void AASCharacterBase::HandleOnManaChanged_Implementation(float Mana, float MaxMana)
{
	// ToDo: Implementation of mana changing
}

void AASCharacterBase::HandleOnStrengthChanged_Implementation(float Strength, float MaxStrength)
{
	// ToDo: Implementation of strength changing
}

void AASCharacterBase::HandleOnHealthChanged_Implementation(float Health, float MaxHealth)
{
	if (Health <= 0.f)
	{
		Death();
	}
}

void AASCharacterBase::HandleOnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsHostile(OtherActor) && OtherActor != this)
	{
		FGameplayEventData PayloadData;
		PayloadData.Target = OtherActor;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, MeleeDealDamageTag, PayloadData);
	}
}

void AASCharacterBase::HandleOnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsHostile(OtherActor) && !OverlapExclude.Contains(OtherActor))
	{
		FGameplayEventData PayloadData;
		PayloadData.Target = OtherActor;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, DashDealDamageTag, PayloadData);
		OverlapExclude.AddUnique(OtherActor);
	}
}

UAbilitySystemComponent* AASCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AASCharacterBase::SetSwordCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (SwordCollision)
	{
		SwordCollision->SetCollisionEnabled(CollisionEnabled);
	}
}

void AASCharacterBase::AddGameplayTag(const FGameplayTag& TagToAdd) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(TagToAdd);
		AbilitySystemComponent->SetTagMapCount(TagToAdd, 1);
	}
}

void AASCharacterBase::RemoveGameplayTag(const FGameplayTag& TagToRemove) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(TagToRemove);
	}
}

void AASCharacterBase::AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire)
{
	if (AbilitySystemComponent && HasAuthority() && AbilityToAcquire)
	{
		FGameplayAbilitySpecDef AbilitySpecDef;
		AbilitySpecDef.Ability = AbilityToAcquire;

		FGameplayAbilitySpec AbilitySpec(AbilitySpecDef, 0);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
	}
}

void AASCharacterBase::AcquireAbilities(TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToAcquire)
{
	for (auto& Ability: AbilitiesToAcquire)
	{
		AcquireAbility(Ability);
		if (Ability->IsChildOf(UASGameplayAbilityBase::StaticClass()))
		{
			if (const TSubclassOf<UASGameplayAbilityBase> AbilityBaseClass = *Ability)
			{
				AddAbilityToUI(AbilityBaseClass);
			}
		}
	}
}
