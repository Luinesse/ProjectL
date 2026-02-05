// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameAbility.h"
#include "AbilitySystemComponent.h"
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

// 자기 자신에게 GE를 적용할 때 사용될 함수.
// 범용성이 높으므로 Base에 배치
FActiveGameplayEffectHandle UBaseGameAbility::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level)
{
	if (!GameplayEffectClass)	return FActiveGameplayEffectHandle();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)	return FActiveGameplayEffectHandle();

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, level, EffectContext);
	if (SpecHandle.IsValid()) {
		return ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	return FActiveGameplayEffectHandle();
}
