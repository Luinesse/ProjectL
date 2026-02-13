// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LuinCharacterBase.h"
// Enhanced Input 처리용
#include "InputActionValue.h"
#include "LuinPlayer.generated.h"

/**
 * 
 */

 // 입력
class UInputMappingContext;
class UInputAction;
// 카메라
class USpringArmComponent;
class UCameraComponent;
// 모션 워핑
class UMotionWarpingComponent;
// UI
class ULuinHUDWidget;

// 델리게이트. (속성 변경 시 호출)
struct FOnAttributeChangeData;

UCLASS()
class PROJECTL_API ALuinPlayer : public ALuinCharacterBase
{
	GENERATED_BODY()
public:
	ALuinPlayer();
protected:
	virtual void BeginPlay() override;

	// 스프링 암 && 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LockOnAction;

	// Speed
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AActor* CurrentTargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComp;

	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<ULuinHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<ULuinHUDWidget> HUDWidget;

	// 스태미너 리젠
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> StaminaRegenEffect;

	// 수동 락온
	void ToggleLockOn();

	// 키다운 시 실행될 콜백함수
	void Input_Attack(const FInputActionValue& Value);
	void Input_AttackReleased(const FInputActionValue& Value);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);

	void SprintStart();
	void SprintStop();

	// 스태미나 변경 시 호출되는 콜백함수
	void OnStaminaChanged(const FOnAttributeChangeData& Data);

	// 이동속도 변경 시 호출되는 콜백함수
	void OnSpeedChanged(const FOnAttributeChangeData& Data);

	// 락온 대상 찾기
	AActor* FindLockOnTarget(float SearchRange = 1000.0f);

	virtual void Die(AActor* DamageCauser) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 공격 시 워핑 타겟 업데이트
	void UpdateMotionWarpTarget();
};
