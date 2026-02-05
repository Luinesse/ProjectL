// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "LuinCharacterBase.h"


void UAttackAbility::ApplyGameplayEffectToTarget(TArray<AActor*> Targets, TSubclassOf<UGameplayEffect> GameplayEffectClass, float level)
{
	// 피격 대상이 있었는지, GE가 존재하는지
	if (Targets.Num() == 0 || !GameplayEffectClass)	return;

	// GA를 호출한 대상의 ASC
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)	return;

	// GE 생성을 위한 EffectContext 생성
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// GE 적용을 위한 Spec 생성
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, level, EffectContext);

	if (SpecHandle.IsValid()) {
		for (AActor* TargetActor : Targets) {
			// 피격당한 대상의 ASC를 가져온다.
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

			if (TargetASC) {
				// 대상에게 Spec Handle을 토대로 GE 적용
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				// 넉백
				LaunchTarget(TargetActor);
			}
		}
	}
}

void UAttackAbility::LaunchTarget(AActor* Target)
{
	if (!Target)	return;

	ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	if (TargetCharacter && GetCharacterBase()) {
		FVector Dir = TargetCharacter->GetActorLocation() - GetCharacterBase()->GetActorLocation();

		Dir.Z = 0.0f;
		Dir.Normalize();

		FVector LaunchVelocity = (Dir * 500.0f) + FVector(0.0f, 0.0f, 300.0f);

		TargetCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
}