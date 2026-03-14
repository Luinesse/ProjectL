// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTask.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"

UAttackTask::UAttackTask()
{
	// 비헤이비어 트리 상에서 노출될 노드명
	NodeName = TEXT("GAS_Attack");
}

EBTNodeResult::Type UAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIC를 가져옴
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)	return EBTNodeResult::Failed;

	// AIC가 빙의하고 있는 캐릭터를 가져옴.
	ACharacter* AICharacter = Cast<ACharacter>(AIC->GetPawn());
	if (!AICharacter)	return EBTNodeResult::Failed;

	// 해당 캐릭터가 가지고 있는 ASC를 가져옴.
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AICharacter);
	if (ASC) {
		// Ability.Attack 의 태그를 가지는 GA를 활성화함.
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")));
		bool bSuccess = ASC->TryActivateAbilitiesByTag(TagContainer);

		return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
