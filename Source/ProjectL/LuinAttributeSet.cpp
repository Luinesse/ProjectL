// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

ULuinAttributeSet::ULuinAttributeSet()
{
}

void ULuinAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 네트워크 동기화 대상으로 등록.
	// COND_None이 뭐고 종류가 몇개지 ? REPNOTIFY_Always 는 뭐고 종류가 몇개지 ?
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULuinAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
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
