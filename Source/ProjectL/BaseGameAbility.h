// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameAbility.generated.h"

class ALuinCharacterBase;

/**
 기존 공격 함수들은 AttackAbility 로 분할.
 */
UCLASS()
class PROJECTL_API UBaseGameAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	// 캐릭터들의 부모 캐릭터로 캐스팅 수행
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player")
	ALuinCharacterBase* GetCharacterBase() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FActiveGameplayEffectHandle ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level = 1.0f);
};
