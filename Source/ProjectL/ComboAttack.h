// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackAbility.h"
#include "ComboAttack.generated.h"

/**
 AttackAbility 로 공격 부분을 분할하였으므로, 부모 클래스 변경
 */
UCLASS()
class PROJECTL_API UComboAttack : public UAttackAbility
{
	GENERATED_BODY()
public:
	UComboAttack();

protected:
	// GA 활성화 시 실행
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// GA 종료 시 실행
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// GA 실행 중 키 입력 시 호출
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// 몽타주 종료 시 호출될 콜백 함수
	UFUNCTION()
	void OnMontageEnded();

	// 이벤트 태그를 받았을 때, 호출될 콜백함수
	UFUNCTION()
	void OnGameplayEventReceived(FGameplayEventData Payload);

	// OnGameplyEventReceived 를 통해 호출할 함수.
	// 블루프린트에서 구현 (GE를 에디터에서 적절하게 수정하기 위해)
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void HitCheck();

private:
	// 현재 실행 중인 콤보
	UPROPERTY()
	int32 CurrentCombo = 0;

	// 최대 콤보 횟수
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	int32 MaxCombo = 3;

	// 실행할 콤보 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TObjectPtr<UAnimMontage> ComboMontage;
};
