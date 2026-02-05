// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "LuinCharacterBase.h"

ALuinCharacterBase* UBaseGameAbility::GetCharacterBase() const
{
	// 적 캐릭터와 플레이어 캐릭터의 부모 클래스를 가져온다.
	// 적과 플레이어는 공통적으로 공격을 수행하고, 공격에 맞았는지 검사하는 트레이싱 코드는
	// 부모 클래스에 작성했으므로 ALuinCharacterBase 로 캐스팅한다.
	// GA 호출 시 수행하는 몽타주 또한 ALuinCharacterBase 에 프로퍼티로 작성했으므로
	// Play Montage And Wait 노드를 에디터에서 수행하는데 문제가 없다.
	if (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid()) {
		return Cast<ALuinCharacterBase>(CurrentActorInfo->AvatarActor.Get());
	}

	return nullptr;
}

void UBaseGameAbility::ApplyGameplayEffectToTarget(TArray<AActor*> Targets, TSubclassOf<UGameplayEffect> GameplayEffectClass, float level)
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

void UBaseGameAbility::LaunchTarget(AActor* Target)
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
