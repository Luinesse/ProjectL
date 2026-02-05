// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// ASC 를 쓰기위함.
#include "AbilitySystemInterface.h"
#include "LuinCharacterBase.generated.h"

// ASC
class UAbilitySystemComponent;
// 플레이어 속성
class ULuinAttributeSet;
// GA (적도 스킬을 쓸거니까 베이스에 남겨둠.)
class UGameplayAbility;

// 입력 ID - GAS가 스킬 구분하는 표
UENUM(BlueprintType)
enum class ELuinAbilityInputID : uint8 {
	None,
	Confirm,
	Cancel,
	Attack,
	Sprint
};

// GA가 어떤 타입인지 확인하기 위한 구조체
USTRUCT(BlueprintType)
struct FStartingAbilityInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELuinAbilityInputID InputID = ELuinAbilityInputID::None;
};

UCLASS()
class PROJECTL_API ALuinCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALuinCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// AttributeSet 변수 선언
	UPROPERTY()
	TObjectPtr<ULuinAttributeSet> AttributeSet;

	// GA 배열
	// GA와 GA 타입을 담는 구조체로 변경(26.02.05)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TArray<FStartingAbilityInfo> DefaultAbilities;

	// GE를 담음. (GE 테스트용으로 사용했음. 불필요)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultDamageEffect;

	// 무기 스태틱 메시를 담을 프로퍼티
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	// 무기가 붙을 소켓 이름
	FName WeaponSocketName;

	// 사망 여부 플래그 (Bit Field 사용)
	uint8 bIsDead : 1 = 0;

	// 공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 블루프린트에서 구현될 디졸브 함수.
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void StartDissolve();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 실제 사망 처리 함수
	virtual void Die(AActor* DamageCauser);

	// 공격 판정 함수.
	// AttackRange = 사거리
	// AttackRadius = 공격 반지름 (Sphere Trace 사용하므로)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	TArray<AActor*> GetEnemiesInAttackRange(float AttackRange, float AttackRadius);

	// 블루프린트에서 호출할 몽타주 게터
	UFUNCTION(BlueprintCallable, Category = "Combat")
	inline UAnimMontage* GetAttackMontage() const { return AttackMontage; }

	// 사망 정보 게터
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsDead() const { return bIsDead; }

	// 피아식별 함수
	bool IsHostile(AActor* TargetActor) const;
};
