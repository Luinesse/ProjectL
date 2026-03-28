// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFocus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

UEnemyFocus::UEnemyFocus()
{
	NodeName = TEXT("SetFocus");
}

EBTNodeResult::Type UEnemyFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) {
		return EBTNodeResult::Failed;
	}

	// 락온 대상 가져오기
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));

	// 락온 대상이 있다면, 락온
	if (TargetActor) {
		AIC->SetFocus(TargetActor);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
