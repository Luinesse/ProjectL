// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LuinHUDWidget.generated.h"

// 프로그레스 바
class UProgressBar;
// ASC
class UAbilitySystemComponent;
// 델리게이트. (속성 변경 시 호출)
struct FOnAttributeChangeData;

/**
 *
 */
UCLASS()
class PROJECTL_API ULuinHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 속성 값 초기화 및 콜백함수 바인드 수행
	void BindToAttribte(UAbilitySystemComponent* ASC);

protected:
	// meta = (BindWidget) 을 사용하면, 변수명과 똑같은 프로퍼티에 알아서 바인드됨.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ManaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

private:
	// 값이 변했을 때 호출될 콜백함수.
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnMaxManaChanged(const FOnAttributeChangeData& Data);
	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	void OnMaxStaminaChanged(const FOnAttributeChangeData& Data);

	// SetPercent 에서 사용할 변수
	float CurrentHealth = 0.0f;
	float MaxHealth = 1.0f;
	float CurrentMana = 0.0f;
	float MaxMana = 1.0f;
	float CurrentStamina = 0.0f;
	float MaxStamina = 1.0f;

	// UI Percent 갱신
	void UpdateHealthBar();
	void UpdateManaBar();
	void UpdateStaminaBar();
};
