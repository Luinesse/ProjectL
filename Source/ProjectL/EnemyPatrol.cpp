// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPatrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UEnemyPatrol::UEnemyPatrol()
{
	NodeName = TEXT("Find Patrol Position");
}

EBTNodeResult::Type UEnemyPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIC 가져오기
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)	return EBTNodeResult::Failed;

	// AIC가 빙의하고있는 폰 가져오기
	APawn* AIPawn = AIC->GetPawn();
	if (!AIPawn)	return EBTNodeResult::Failed;

	// 월드의 내비게이션 시스템 가져오기
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)	return EBTNodeResult::Failed;

	// 내비게이션 시스템에서 특정 위치를 저장할 수 있는 구조체
	FNavLocation RandomLocation;

	// 내비게이션 시스템에서 500.0f 내부에 이동가능한 위치를 가져와 RandomLocation 에 저장
	if (NavSystem->GetRandomPointInNavigableRadius(AIPawn->GetActorLocation(), 500.0f, RandomLocation)) {
		// 가져온 랜덤 위치인 RandomLocation을 블랙보드의 PatrolPos 에 입력
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("PatrolPos"), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
