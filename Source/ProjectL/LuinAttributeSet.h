// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "LuinAttributeSet.generated.h"

// 게터, 세터, 이니셜라이저를 자동 생성하는 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// 델리게이트 선언.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfHealthDelegate, AActor*);

/**
 * 
 */
UCLASS()
class PROJECTL_API ULuinAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	ULuinAttributeSet();

	// 플레이어의 사망을 알리는 델리게이트.
	// 여기서 mutable 이 왜 붙어야 하는가 ? -> AttributeSet을 추후 const 포인터로 가지고 있다면 ?
	// 그렇게 가져온 AttributeSet은 상수이기 때문에, 내부 변수인 델리게이트 또한 건드릴 수 없게됨.
	// 따라서, 항상 델리게이트를 사용할 수 있도록 해당 변수만 mutable로 사용해서 const일 때도 건드릴 수 있도록 함.
	// const_cast 와 비슷하다고 생각하는 중.(mutable)
	mutable FOnOutOfHealthDelegate OnOutOfHealth;

	// 리플리케이션을 위한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// GE가 적용된 후 실행되는 함수
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// 체력
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, MaxHealth);

	// 마나
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, MaxMana);

	// 스태미나
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, MaxStamina);

	// 공격력
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Attributes")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(ULuinAttributeSet, AttackPower);

protected:
	// ReplicatedUsing 옵션으로 인해 값이 네트워크를 타고 갱신됐을 때 호출되는 콜백 함수
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
};
