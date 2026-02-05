// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "LuinAttributeSet.h"
#include "GameplayAbilitySpec.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ALuinCharacterBase::ALuinCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// ASC 생성.
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// AttributeSet 생성.
	AttributeSet = CreateDefaultSubobject<ULuinAttributeSet>(TEXT("AttributeSet"));

	// 무기 달기.
	WeaponSocketName = TEXT("Weapon");

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ALuinCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ALuinCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// ASC 가 정상적으로 생성됐다면, ASC를 초기화.
	// 매개 변수로 Owner, Avatar 를 받음. 각각 데이터의 주인, 화면에 보이는 클래스를 의미.
	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// AttributeSet 가 정상적으로 생성됐다면, AttibuteSet을 초기화.
		// 매크로로 등록한 변수들의 이니셜라이저 호출.
		if (AttributeSet) {
			AttributeSet->InitHealth(100.0f);
			AttributeSet->InitMaxHealth(100.0f);
			AttributeSet->InitMana(50.0f);
			AttributeSet->InitMaxMana(50.0f);
			AttributeSet->InitStamina(100.0f);
			AttributeSet->InitMaxStamina(100.0f);
			AttributeSet->InitAttackPower(10.0f);

			AttributeSet->OnOutOfHealth.AddUObject(this, &ALuinCharacterBase::Die);

			UE_LOG(LogTemp, Warning, TEXT("GAS Init Complete. Health : %f"), AttributeSet->GetHealth());

			if (GEngine) {
				FString DebugMsg = FString::Printf(TEXT("My HP : %f"), AttributeSet->GetHealth());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMsg);
			}
		}

		// 스킬 부여
		// 멀티플레이 환경에선 서버 권한이 있을 때만 부여.
		if (HasAuthority()) {
			// DefaultAbilities 에 담아둔 GA를 가져옴.
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities) {
				if (AbilityClass) {
					// GA의 스펙을 결정. 레벨 1 수준으로 GAS는 이를 Attack으로 식별(enum class)
					FGameplayAbilitySpec Spec(AbilityClass, 1, static_cast<int32>(ELuinAbilityInputID::Attack), this);
					AbilitySystemComponent->GiveAbility(Spec);
				}
			}
		}
	}

	/* GE 테스트
	
	// ASC가 생성된 상태이며 GE를 에디터에서 등록하여 리플렉션 된 상태일 때.
	if (AbilitySystemComponent && DefaultDamageEffect) {
		// Context 생성.
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		// GE의 출처를 지정.
		EffectContext.AddSourceObject(this);

		// 어떤 GE를 어느정도 레벨로 적용할것인지.
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultDamageEffect, 1.0f, EffectContext);

		if (SpecHandle.IsValid()) {
			// SpecHandle에 의해 정의된 스펙대로 GE를 적용. ToSelf 이므로 자기 자신에게 적용
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			UE_LOG(LogTemp, Warning, TEXT("GE Applied ! Damage Taken !"));

			if (GEngine) {
				FString DebugMsg = FString::Printf(TEXT("My HP : %f"), AttributeSet->GetHealth());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMsg);
			}
		}
	}

	*/
}

// Called every frame
void ALuinCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ALuinCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ALuinCharacterBase::Die(AActor* DamageCauser)
{
	if (bIsDead)	return;

	bIsDead = true;

	// 해당 부분에 공격을 취소하도록 하는 코드
	// 진행중인 모든 어빌리티를 강제로 취소함.
	// 아직 테스트 전. (적의 공격을 구현하거나 플레이어가 죽을 상황을 만든 후 확인가능)
	if (AbilitySystemComponent) {
		AbilitySystemComponent->CancelAllAbilities();
	}

	// 캡슐 콜리전을 NoCollision으로 설정하여 의도치않은 충돌 방지
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 캐릭터 움직임을 멈추고 입력 비활성화
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	FVector ImpulseDirection = FVector::ZeroVector;

	if (DamageCauser) {
		FVector MyLoc = GetActorLocation();
		FVector CauserLoc = DamageCauser->GetActorLocation();

		CauserLoc.Z = MyLoc.Z;

		ImpulseDirection = (MyLoc - CauserLoc).GetSafeNormal();
	}
	else {
		ImpulseDirection = -GetActorForwardVector();
	}

	// 랙돌 활성화
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	float KnockbackForce = 30000.0f;
	FVector FinalImpulse = (ImpulseDirection * KnockbackForce) + FVector(0, 0, 15000.0f);

	GetMesh()->AddImpulse(FinalImpulse);

	// 디졸브 시작
	StartDissolve();
}

TArray<AActor*> ALuinCharacterBase::GetEnemiesInAttackRange(float AttackRange, float AttackRadius)
{
	// 결과를 저장할 배열
	TArray<AActor*> HitActors;

	// Sphere의 시작점과 끝점
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * AttackRange);

	// 충돌 검사할 타입 설정. Pawn만 감지하도록 설정.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	// 무시할 대상 설정.
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// 트레이스 결과를 저장할 배열
	TArray<FHitResult> OutHits;
	
	// Sphere Trace 수행.
	bool bResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Start,
		End,
		AttackRadius,
		ObjectTypes,
		false,			// Trace Complex. 정밀하게 판정할 것인지.
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // 디버그 선 그리기.
		OutHits,
		true			// 자신을 무시할 것인지.
	);

	// 충돌한 대상이 있다면.
	if (bResult) {
		for (const FHitResult& Hit : OutHits) {
			if (AActor* HitActor = Hit.GetActor()) {
				// 맞은 액터가 존재하고, 그 대상이 적이라면.
				if (HitActor && IsHostile(HitActor)) {
					// 그 대상이 리스트에 없는 대상이라면.
					if (!HitActors.Contains(HitActor)) {
						// 추가.
						HitActors.Add(HitActor);
					}
				}
			}
		}
	}

	return HitActors;
}

bool ALuinCharacterBase::IsHostile(AActor* TargetActor) const
{
	if (!TargetActor)	return false;

	// 내가 플레이어라면 적 캐릭터만을 공격 대상으로 삼음
	if (this->ActorHasTag(FName("Player"))) {
		return TargetActor->ActorHasTag(FName("Enemy"));
	}
	// 내가 적이라면 플레이어 캐릭터만을 공격 대상으로 삼음.
	else if (this->ActorHasTag(FName("Enemy"))) {
		return TargetActor->ActorHasTag(FName("Player"));
	}

	return false;
}

