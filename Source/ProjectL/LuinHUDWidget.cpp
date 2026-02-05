// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Components/ProgressBar.h"
#include "LuinAttributeSet.h"

void ULuinHUDWidget::BindToAttribte(UAbilitySystemComponent* ASC)
{
	if (!ASC)	return;

	// GetNumericAttribute 를 통해 속성의 값을 바로가져옴.
	CurrentHealth = ASC->GetNumericAttribute(ULuinAttributeSet::GetHealthAttribute());
	MaxHealth = ASC->GetNumericAttribute(ULuinAttributeSet::GetMaxHealthAttribute());
	CurrentMana = ASC->GetNumericAttribute(ULuinAttributeSet::GetManaAttribute());
	MaxMana = ASC->GetNumericAttribute(ULuinAttributeSet::GetMaxManaAttribute());
	CurrentStamina = ASC->GetNumericAttribute(ULuinAttributeSet::GetStaminaAttribute());
	MaxStamina = ASC->GetNumericAttribute(ULuinAttributeSet::GetMaxStaminaAttribute());

	// Percent 용 변수를 초기화했으니, UI 업데이트
	UpdateHealthBar();
	UpdateManaBar();
	UpdateStaminaBar();

	// 값이 변경됐을 때 콜백함수를 델리게이트 통해 바인드
	ASC->GetGameplayAttributeValueChangeDelegate(ULuinAttributeSet::GetHealthAttribute()).AddUObject(this, &ULuinHUDWidget::OnHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(ULuinAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ULuinHUDWidget::OnMaxHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(ULuinAttributeSet::GetManaAttribute()).AddUObject(this, &ULuinHUDWidget::OnManaChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(ULuinAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ULuinHUDWidget::OnMaxManaChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(ULuinAttributeSet::GetStaminaAttribute()).AddUObject(this, &ULuinHUDWidget::OnStaminaChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(ULuinAttributeSet::GetMaxStaminaAttribute()).AddUObject(this, &ULuinHUDWidget::OnMaxStaminaChanged);
}

void ULuinHUDWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	UpdateHealthBar();
}

void ULuinHUDWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
	UpdateHealthBar();
}

void ULuinHUDWidget::OnManaChanged(const FOnAttributeChangeData& Data)
{
	CurrentMana = Data.NewValue;
	UpdateManaBar();
}

void ULuinHUDWidget::OnMaxManaChanged(const FOnAttributeChangeData& Data)
{
	MaxMana = Data.NewValue;
	UpdateManaBar();
}

void ULuinHUDWidget::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	CurrentStamina = Data.NewValue;
	UpdateStaminaBar();
}

void ULuinHUDWidget::OnMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	MaxStamina = Data.NewValue;
	UpdateStaminaBar();
}

void ULuinHUDWidget::UpdateHealthBar()
{
	if (HealthBar && MaxHealth > 0.0f) {
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void ULuinHUDWidget::UpdateManaBar()
{
	if (ManaBar && MaxMana > 0.0f) {
		ManaBar->SetPercent(CurrentMana / MaxMana);
	}
}

void ULuinHUDWidget::UpdateStaminaBar()
{
	if (StaminaBar && MaxStamina > 0.0f) {
		StaminaBar->SetPercent(CurrentStamina / MaxStamina);
	}
}