// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "ASCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGameplayAbility;
class UASGameplayAbilityBase;
class UAttributeSetBase;

UCLASS()
class ABILITYSYSTEMCOURSE_API AASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> OverlapExclude;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities;

	UPROPERTY(BlueprintReadWrite, Category = "Abilities")
	bool bIsTargeting = false;

protected:
	UPROPERTY(EditAnywhere, Category = "Team")
	uint8 TeamID = 255;

	UPROPERTY(BlueprintReadWrite, Category = "Combat Properties")
	uint8 ComboCount = 1;

private:
	/** Components **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	UCapsuleComponent* SwordCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	UAttributeSetBase* AttributeComponent;

	/** Gameplay Abilities **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayAbility> MeleeAttackAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayAbility> HealthRegenAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayAbility> DashAbility;

	/** Gameplay Tags **/
	UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
	FGameplayTag MeleeDealDamageTag;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
	FGameplayTag DashDealDamageTag;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
	FGameplayTag FullHealthTag;

	/** Properties **/
	bool bIsDeath;
	float DefaultGroundFriction;

	/** Montages **/
	UPROPERTY(EditDefaultsOnly, Category = "Montages", meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages", meta = (AllowPrivateAccess = true))
	TMap<uint8, UAnimMontage*> MeleeComboMontages;

public:
	AASCharacterBase();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	void AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire);

	UFUNCTION(BlueprintCallable)
	void AcquireAbilities(TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToAcquire);

	UFUNCTION(BlueprintCallable)
	void MeleeAttack();

	UFUNCTION(BlueprintCallable)
	void CancelAbility();

	UFUNCTION(BlueprintCallable)
	bool IsHostile(const AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void SetCollisionResponseToPawn(const ECollisionResponse CollisionResponse);

	UFUNCTION(BlueprintCallable)
	void PushCharacter(const FVector Direction, const float Strength, const float StunDuration);

	UFUNCTION(BlueprintCallable)
	void HitStun(const float Duration);

	UFUNCTION(BlueprintCallable)
	void MakeCameraControlRotation(const bool bShouldControl);

	UFUNCTION(BlueprintCallable)
	bool DetermineCombo(UAnimMontage*& OutComboMontage, uint8& OutComboCount);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void EndMeleeCombo() { ComboCount = 1; };
	
	void SetSwordCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	void AddGameplayTag(const FGameplayTag& TagToAdd) const;
	void RemoveGameplayTag(const FGameplayTag& TagToRemove) const;

	FORCEINLINE uint8 GetTeamID() const { return TeamID; }
	FORCEINLINE const FGameplayTag& GetFullHealthTag() const { return FullHealthTag; }
	FORCEINLINE const UAttributeSetBase* GetAttributeComponent() const { return AttributeComponent; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                               const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                 const FHitResult& SweepResult);

	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void TurnAtRate(const float Rate);
	void LookUpRate(const float Rate);

	void SwitchInputControl(const bool bIsEnable);

	void AddAbilityToUI(TSubclassOf<UASGameplayAbilityBase> AbilityToAdd) const;

	/** Abilities **/
	void Death();

	/** Callback functions **/
	UFUNCTION(BlueprintNativeEvent)
	void HandleOnHealthChanged(float Health, float MaxHealth);

	UFUNCTION(BlueprintNativeEvent)
	void HandleOnManaChanged(float Mana, float MaxMana);

	UFUNCTION(BlueprintNativeEvent)
	void HandleOnStrengthChanged(float Strength, float MaxStrength);
};
