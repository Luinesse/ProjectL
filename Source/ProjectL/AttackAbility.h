// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameAbility.h"
#include "AttackAbility.generated.h"

/**
 공격에 사용할 GA로 분리. (26.02.05)
 */
UCLASS()
class PROJECTL_API UAttackAbility : public UBaseGameAbility
{
	GENERATED_BODY()

public:
	// GE 적용.
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyGameplayEffectToTarget(TArray<AActor*> Targets, TSubclassOf<UGameplayEffect> GameplayEffectClass, float level = 1);

	// 넉백
	void LaunchTarget(AActor* Target);
};
