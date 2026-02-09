// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"
#include "LuinHUDWidget.h"
#include "LuinAttributeSet.h"

ALuinPlayer::ALuinPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));

	// 기본 이동속도 설정
	//GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// 모험 모드로 설정 (UseControllerRotationYaw 사용 X -> 카메라 회전이 마우스를 따라가지 않음.)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// OrientRotationToMovement 사용으로 입력에 따라 회전 
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 회전 속도
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 식별 전용 태그
	Tags.Add(FName("Player"));
}

void ALuinPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 입력 세팅
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			if (DefaultMappingContext) {
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// 위젯 출력
	if (IsLocallyControlled() && HUDWidgetClass) {
		HUDWidget = CreateWidget<ULuinHUDWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget) {
			HUDWidget->AddToViewport();
			HUDWidget->BindToAttribte(AbilitySystemComponent);
		}
	}

	// 스태미너의 변경을 델리게이트로 체크
	if (AbilitySystemComponent) {
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			ULuinAttributeSet::GetStaminaAttribute()).AddUObject(this, &ALuinPlayer::OnStaminaChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			ULuinAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &ALuinPlayer::OnSpeedChanged);

		GetCharacterMovement()->MaxWalkSpeed = AbilitySystemComponent->GetNumericAttribute(ULuinAttributeSet::GetMovementSpeedAttribute());
		WalkSpeed = AbilitySystemComponent->GetNumericAttribute(ULuinAttributeSet::GetMovementSpeedAttribute());
	}
}

void ALuinPlayer::UpdateMotionWarpTarget()
{
	// 락온 대상과 모션워핑 컴포넌트가 정상적으로 생성됐다면.
	if (CurrentTargetActor && MotionWarpingComp) {
		// 락온 대상과의 거리를 검사하여 상식선에서 워프 가능한 거리인지 확인.
		FVector TargetLoc = CurrentTargetActor->GetActorLocation();
		float Dist = FVector::Dist(GetActorLocation(), TargetLoc);
		float MaxWarpRange = 1000.0f;

		// 워프하기에 너무 멀다면 현재 워프는 취소.
		if (Dist > MaxWarpRange) {
			MotionWarpingComp->RemoveWarpTarget(FName("AttackTarget"));
			return;
		}

		// 라인 트레이싱 수행.
		FHitResult HitResult;
		FCollisionQueryParams Params;
		// 플레이어 자신과 적 캐릭터는 검사에서 제외. (적 캐릭터에게 붙어서 공격해야하므로.)
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(CurrentTargetActor);

		// WorldStatic 타입만 검사.
		bool bHitWall = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			GetActorLocation(),
			TargetLoc,
			ECC_WorldStatic,
			Params
		);

		// 히트한 대상이 있다면, 락온 대상으로 워프하는 길목에 장애물이 있으므로 워프 취소.
		if (bHitWall) {
			MotionWarpingComp->RemoveWarpTarget(FName("AttackTarget"));
			return;
		}

		// 적이 나를 바라보는 방향벡터.
		FVector Dir = (GetActorLocation() - TargetLoc).GetSafeNormal();
		// 적의 앞 100.0f 위치에 워프할 예정.
		float AttackRange = 100.0f;

		// 락온 대상의 위치로부터 적이 나를 바라보는 방향으로 100.0f 앞이 최종적으로 워프할 위치.
		FVector FinalWarpLoc = TargetLoc + (Dir * AttackRange);

		// 내가 타겟을 바라보는 방향벡터
		FVector ToTarget = (TargetLoc - GetActorLocation()).GetSafeNormal();
		// 워프했을 때, 플레이어캐릭터가 바라볼 방향.
		FRotator FinalWarpRot = ToTarget.Rotation();

		// AttackTarget 이라는 워프타깃이름이 설정된 노티파이 스테이트를 만날때 모션워핑 수행.
		MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
			FName("AttackTarget"),
			FinalWarpLoc,
			FinalWarpRot
		);
	}
	// 만일 락온 대상이 없다면 워프 취소.
	else if (MotionWarpingComp) {
		MotionWarpingComp->RemoveWarpTarget(FName("AttackTarget"));
	}
	// 컴포넌트마저 없기때문에 검사대상 아님. (nullptr로 크래시)
	else {
		return;
	}
}

void ALuinPlayer::ToggleLockOn()
{
	// 락온하고 있는 대상이 있다면.
	if (CurrentTargetActor) {
		// 락온을 해제하고 모험 모드로 동작. (키보드 입력에 회전 의존)
		CurrentTargetActor = nullptr;

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else {
		// 락온 대상 찾기
		AActor* NewTarget = FindLockOnTarget();

		// 락온 대상이 있다면, 전투 모드로 동작. (락온 대상에게 카메라를 고정하고 회전)
		if (NewTarget) {
			CurrentTargetActor = NewTarget;

			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void ALuinPlayer::Input_Attack(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		// 달리던 도중 공격을 시도하면 달리기를 멈춤.
		// 어차피 모션 워프로 접근하게 되므로 이 로직이 상식적.
		if (GetCharacterMovement()->MaxWalkSpeed > WalkSpeed) {
			SprintStop();
		}

		UpdateMotionWarpTarget();
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(ELuinAbilityInputID::Attack));
	}
}

void ALuinPlayer::Input_AttackReleased(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(ELuinAbilityInputID::Attack));
	}
}

void ALuinPlayer::Input_Move(const FInputActionValue& Value)
{
	// 입력값 (x, y) 가져오기
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 컨트롤러가 보고 있는 방향 알아내기 (Z축 회전만 고려)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전후방 벡터 (Forward)
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// 좌우 벡터 (Right)
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 이동 적용
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALuinPlayer::Input_Look(const FInputActionValue& Value)
{
	// 마우스 입력값 가져오기
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 락온 시 회전은 마우스에 의존하면 안되므로 예외처리.
		if (!CurrentTargetActor) {
			// 마우스 X -> 캐릭터 좌우 회전 (Yaw)
			AddControllerYawInput(LookAxisVector.X);
			// 마우스 Y -> 카메라 상하 회전 (Pitch)
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

// 스프린트 시작 및 종료 시 Sprint InputID 를 사용하는 GA 실행
void ALuinPlayer::SprintStart()
{
	if (AbilitySystemComponent) {
		float CurrentStamina = AbilitySystemComponent->GetNumericAttribute(ULuinAttributeSet::GetStaminaAttribute());

		// 스태미너가 부족하다면 달릴 수 없음.
		if (CurrentStamina <= 2.0f) {
			return;
		}

		// 공격 검사 삭제 (2026.02.09 - 에디터에서 활성화 소유 태그, 활성화 차단 태그로 관리중)

		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(ELuinAbilityInputID::Sprint));
	}
}

void ALuinPlayer::SprintStop()
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(ELuinAbilityInputID::Sprint));
	}
}

void ALuinPlayer::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	// 스태미너 변경 시 현재 달릴 수 있는 상태인지 계속 검사하고, 안된다면 SprintStop 강제 실행
	if (Data.NewValue <= 2.0f && GetCharacterMovement()->MaxWalkSpeed > WalkSpeed) {
		SprintStop();
	}
}

void ALuinPlayer::OnSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

AActor* ALuinPlayer::FindLockOnTarget(float SearchRange)
{
	FVector CameraLoc;
	FRotator CameraRot;

	if (!GetController())	return nullptr;

	// 현재 플레이어의 카메라 위치와 카메라 회전값을 가져옴.
	GetController()->GetPlayerViewPoint(CameraLoc, CameraRot);
	// 카메라가 바라보고 있는 방향 (전방벡터)
	FVector CameraForward = CameraRot.Vector();

	// 오버랩 결과를 저장할 배열, 검사에서 제외할 대상
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams(NAME_None, false, this);

	// 플레이어 위치를 중심으로 SearchRange 범위의 적 오버랩 탐색.
	// 폰만을 인식.
	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SearchRange),
		QueryParams
	);

	// 탐색된 대상이 없다면 종료
	if (!bHit)	return nullptr;

	// 적합한 타겟을 저장할 AActor 변수
	AActor* BestTarget = nullptr;
	// 카메라 전방벡터와 타겟과의 cos세타를 구하여 최적의 타겟을 구함. (플레이어의 정면에서 가장 가까이 있는 적 일수록 세타는 0도에 가까움 -> cos0 = 1)
	float BestDotProduct = -1.0f;

	// 탐색 결과 순회
	for (const FOverlapResult& Result : OverlapResults) {
		AActor* TargetActor = Result.GetActor();

		// 탐색 대상이 자신이거나, 적이 아닌 경우는 넘어감.
		if (!TargetActor || TargetActor == this || !IsHostile(TargetActor))	continue;

		// 타겟이 이미 죽은 상태라면 넘어감.
		ALuinCharacterBase* LuinTarget = Cast<ALuinCharacterBase>(TargetActor);
		if (LuinTarget && LuinTarget->IsDead())	continue;

		// 카메라 위치에서 타겟을 향하는 방향 벡터
		FVector DirToTarget = (TargetActor->GetActorLocation() - CameraLoc).GetSafeNormal();
		// 벡터 내적을 통한 cos세타를 구함.
		float Dot = FVector::DotProduct(CameraForward, DirToTarget);

		// 카메라 시야각 바깥이면 제외
		if (Dot < 0.5f)	continue;

		// 현재 얻은 cos세타값이 가장 큰 값이라면, 최고 점수로 갱신
		if (Dot > BestDotProduct) {
			BestDotProduct = Dot;
			BestTarget = TargetActor;
		}
	}

	// 가장 가까운 적으로 갱신되어 리턴.
	return BestTarget;
}

void ALuinPlayer::Die(AActor* DamageCauser)
{
	Super::Die(DamageCauser);

	// 플레이어 캐릭터의 사망 처리가 들어갈 영역.
}

void ALuinPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 락온 대상이 있다면
	if (CurrentTargetActor) {
		ALuinCharacterBase* TargetBase = Cast<ALuinCharacterBase>(CurrentTargetActor);

		// 락온 대상이 유효한지, 살아있는지 틱마다 체크
		bool bInvalidTarget = !IsValid(CurrentTargetActor);
		bool bTargetIsDead = (TargetBase && TargetBase->IsDead());

		// 만일 락온 대상이 죽었거나 유효하지 않아졌다면
		if (bInvalidTarget || bTargetIsDead) {
			// 근처에 다음 락온 대상이 있는지 탐색
			AActor* NextTarget = FindLockOnTarget();

			// 있다면, 락온 대상을 새로 지정
			if (NextTarget) {
				CurrentTargetActor = NextTarget;
			}
			else {
				// 없다면, 락온 해제
				ToggleLockOn();
			}
			return;
		}

		// 락온 대상과 플레이어 캐릭터 위치 벡터
		FVector TargetLoc = CurrentTargetActor->GetActorLocation();
		FVector MyLoc = GetActorLocation();

		// 락온 체크용
		DrawDebugLine(GetWorld(), MyLoc, TargetLoc, FColor::Red, false, -1.0f, 0, 2.0f);

		// Z축을 통일해서 락온 (인게임에선 두 캐릭터 모두 인간형으로 스케일 격차가 큰 캐릭터는 사용예정 없음.)
		TargetLoc.Z = MyLoc.Z;

		// 락온 대상을 바라보는 회전값
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);

		// 현재 회전값을 얻고, 락온 대상을(LookAtRot) 향한 회전값 획득
		FRotator CurrentRot = GetController()->GetControlRotation();
		FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, 5.0f);

		// 얻은 회전값을 컨트롤러 회전값에 적용. (틱마다 적용되므로 카메라는 항상 락온 대상을 바라봄.)
		GetController()->SetControlRotation(TargetRot);
	}
}

void ALuinPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (AttackAction) {
			// 기존에는 Triggered를 사용했으나, 콤보 공격을 위해 Started 로 변경
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ALuinPlayer::Input_Attack);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ALuinPlayer::Input_AttackReleased);
		}

		if (MoveAction) {
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALuinPlayer::Input_Move);
		}

		if (LookAction) {
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALuinPlayer::Input_Look);
		}

		if (SprintAction) {
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ALuinPlayer::SprintStart);

			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ALuinPlayer::SprintStop);
		}

		if (JumpAction) {
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		if (LockOnAction) {
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ALuinPlayer::ToggleLockOn);
		}
	}
}
