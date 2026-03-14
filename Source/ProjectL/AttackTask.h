// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AttackTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTL_API UAttackTask : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UAttackTask();

protected:
	// 태스크 실행 시 호출
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
