// Copyright Berkeley Bidwell

#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

enum class ECharacterClass : uint8;
class AActor;
class USkeletalMeshComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UDebuffNiagaraComponent;
class UAnimMontage;
class UNiagaraSystem;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class USoundBase;
class UObject;

UCLASS(MinimalAPI, Abstract)
class AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AURA_API AAuraCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	// ~IAbilitySystemInterface

	AURA_API virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ICombatInterface
	AURA_API virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	AURA_API virtual UAnimMontage* GetHitReactMontage_Implementation() const override;
	AURA_API virtual void Die(const FVector& DeathImpulse) override;
	AURA_API virtual bool IsDead_Implementation() const override;
	AURA_API virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override { return AttackMontages; }
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override { return BloodEffect; }
	AURA_API virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) const override;
	virtual int32 GetMinionCount_Implementation() const override { return MinionCount; }
	virtual void IncrementMinionCount_Implementation(int32 Amount) override { MinionCount += Amount; }
	virtual ECharacterClass GetCharacterClass_Implementation() const override { return CharacterClass; }
	virtual FOnDeath& GetOnDeathDelegate() override { return OnDeath; }
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override { return Weapon; }
	// ~ICombatInterface

	UFUNCTION(NetMulticast, reliable)
	AURA_API virtual void MulticastHandleDeath(const FVector& DeathImpulse);
	
	UFUNCTION(BlueprintPure)
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(BlueprintAssignable, Category="Aura|Death")
	FOnDeath OnDeath;
	
	// Not sure why this needs to be replicated
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;
	
protected:
	AURA_API virtual void BeginPlay() override;

	/** Call to set initial primary and secondary attribute values */
	virtual void InitializeDefaultAttributes() const {};
	
	AURA_API virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UFUNCTION()
	AURA_API virtual void OnRep_Stunned() {};
	
	AURA_API void ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& InGameplayEffectClass, float Level) const;

	// Could potentially override in aura character class for this functionality
	AURA_API virtual void AddCharacterAbilities() const;

	/** Dissolves the materials when character dies */
	AURA_API void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	AURA_API void StartDissolveTimeline(const TArray<UMaterialInstanceDynamic*>& DynamicMaterialInstances);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	
	bool bDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 600.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	/** Dissolve Effects */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> BodyDissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;

	// Server used variable only
	int32 MinionCount = 0;

private:
	/** Initializes the ability system component with owner actor/avatar */
	virtual void InitAbilityActorInfo() {};
	
private:
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	/** Passive abilities to give once at start of game */
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
	
	/** Montage to play when reacting to being hit */
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
};
