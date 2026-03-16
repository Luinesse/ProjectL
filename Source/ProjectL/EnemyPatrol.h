// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyPatrol.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTL_API UEnemyPatrol : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UEnemyPatrol();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
