// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameAbility.generated.h"

class ALuinCharacterBase;

/**
 * 
 */
UCLASS()
class PROJECTL_API UBaseGameAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	// 캐릭터들의 부모 캐릭터로 캐스팅 수행
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player")
	ALuinCharacterBase* GetCharacterBase() const;

	// GE 적용.
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyGameplayEffectToTarget(TArray<AActor*> Targets, TSubclassOf<UGameplayEffect> GameplayEffectClass, float level = 1);

	// 넉백
	void LaunchTarget(AActor* Target);
};
