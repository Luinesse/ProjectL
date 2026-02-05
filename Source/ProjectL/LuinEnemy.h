// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LuinCharacterBase.h"
#include "LuinEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTL_API ALuinEnemy : public ALuinCharacterBase
{
	GENERATED_BODY()
public:
	ALuinEnemy();
protected:
	virtual void Die(AActor* DamageCauser) override;
};
