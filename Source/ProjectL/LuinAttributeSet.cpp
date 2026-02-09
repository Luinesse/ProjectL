// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectExtension.h"

ULuinAttributeSet::ULuinAttributeSet()
{
}

void ULuinAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 네트워크 동기화 대상으로 등록.

	/*
		- 해당 함수는 매개변수로 들어온 값을 동기화 대상으로 등록하는 함수입니다.
		- 이때, 조건과 언제 알릴건지를 매개변수를 통해 정할 수 있습니다.
		- 조건의 경우는 크게 다음 4가지를 사용합니다.
			- COND_None (데이터를 모두에게 보냄)
			- COND_OwnerOnly (데이터를 변수의 소유자에게만 보냄)
			- COND_SkipOwner (데이터를 변수의 소유자를 제외한 모두에게 보냄)
			- COND_InitialOnly (데이터를 최초 한번만 보냄)
		- 언제 알릴지의 경우는 크게 2가지를 사용합니다.
			- REPNOTIFY_OnChanged (값이 변했을 때만 알림)
			- REPNOTIFY_Always (값이 변하지 않아도 항상 알림)
		- 누가 데이터를 보아야하는지, 값을 항상 볼지에 따라서 적절히 사용하는 것이 좋습니다.
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void ULuinAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 현재 바뀐 속성이 체력 속성이 맞는지 ?
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		// 체력 클램핑
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		// 죽었다면 ?
		if (GetHealth() <= 0.0f) {
			AActor* DamageCauser = Data.EffectSpec.GetContext().GetEffectCauser();
			// 브로드캐스트
			OnOutOfHealth.Broadcast(DamageCauser);
		}
	}
}

void ULuinAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	// 값이 갱신됐을 때, GAS 시스템에 변경을 알려줌.
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, Health, OldHealth);
}

void ULuinAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, MaxHealth, OldMaxHealth);
}

void ULuinAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, Mana, OldMana);
}

void ULuinAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, MaxMana, OldMaxMana);
}

void ULuinAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, Stamina, OldStamina);
}

void ULuinAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, MaxStamina, OldMaxStamina);
}

void ULuinAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, AttackPower, OldAttackPower);
}

void ULuinAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULuinAttributeSet, MovementSpeed, OldMovementSpeed);
}
