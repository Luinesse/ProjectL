// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyClearFocus.h"
#include "AIController.h"

UEnemyClearFocus::UEnemyClearFocus()
{
	NodeName = TEXT("ClearFocus");
}

EBTNodeResult::Type UEnemyClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) {
		return EBTNodeResult::Failed;
	}

	// 락온 대상이 존재한다면, 락온 해제
	if (AIC->GetFocusActor() != nullptr) {
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}

	return EBTNodeResult::Succeeded;
}
