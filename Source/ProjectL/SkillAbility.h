// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackAbility.h"
#include "SkillAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTL_API USkillAbility : public UAttackAbility
{
	GENERATED_BODY()
	
public:
	USkillAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void NextAttack();

	void FinishAttack();

	void ApplyDamageToTargets();

	UFUNCTION()
	void OnAttackMontageEnded();

	UFUNCTION()
	void OnFinishMontageEnded();

	FVector GetAttackWarpLocation(AActor* Target, float Dist = 100.0f);

private:
	UPROPERTY()
	TArray<AActor*> TargetActors;

	int32 CurrentTargetIndex;
	FVector StartPos;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> SkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> FinishMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> KillDamageEffect;
};
