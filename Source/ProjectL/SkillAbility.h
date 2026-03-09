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
	// 다음 타겟탐색
	void NextAttack();

	// 스킬 종료
	void FinishAttack();

	// 데미지 적용
	void ApplyDamageToTargets();

	// 공격 몽타주 종료 시 호출될 콜백함수
	UFUNCTION()
	void OnAttackMontageEnded();

	// 스킬 종료 몽타주 종료 시 호출될 콜백함수
	UFUNCTION()
	void OnFinishMontageEnded();

	// 모션워프를 통해 워프할 위치
	FVector GetAttackWarpLocation(AActor* Target, float Dist = 100.0f);

private:
	// 스킬 사거리 내 타겟을 저장할 배열
	UPROPERTY()
	TArray<AActor*> TargetActors;

	// 현재 공격 중인 타겟
	int32 CurrentTargetIndex;

	// 다시 돌아갈 위치
	FVector StartPos;

	// 스킬 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> SkillMontage;

	// 스킬 종료 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> FinishMontage;

	// 데미지 GE
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> KillDamageEffect;
};
