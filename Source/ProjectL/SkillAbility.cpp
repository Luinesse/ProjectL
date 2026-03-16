// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAbility.h"
#include "LuinCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USkillAbility::USkillAbility()
{
	// CurrentTargetIndex 를 액터 별로 가지게 하여 변수 공유 방지
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	CurrentTargetIndex = 0;
}

void USkillAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ALuinCharacterBase* Character = GetCharacterBase();

	if (!Character) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 카메라 지연 속도를 매우 낮은값으로 설정하여 스킬 도중에는 카메라가 따라오지 않도록함.
	// 0.0f 로 설정시 카메라 지연 기능 자체가 꺼져버려 적용안되는 버그확인.
	// 따라서 0.01f 로 설정
	Character->SetCameraLagSpeed(0.01f);

	// 스킬을 시작한 현재 위치 및 방향 저장 및 사거리 내 적 탐색 및 저장
	StartPos = Character->GetActorLocation();
	StartRot = Character->GetActorRotation();
	TargetActors = Character->GetEnemiesInAttackRange(1000.0f, 1000.0f);

	if (TargetActors.Num() == 0) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 글로벌 시간을 0.5배로 변경
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f);

	// 플레이어는 정상속도로 움직이게 연출해야하므로, 2배 연출. (0.5 * 2 = 1.0)
	Character->CustomTimeDilation = 2.0f;

	// 시작은 배열의 첫번째 타겟부터
	CurrentTargetIndex = 0;
	// 공격 시작
	NextAttack();
}

void USkillAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// GA 종료시 글로벌 시간과 플레이어 시간을 원래대로 돌려둠.
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	if (ALuinCharacterBase* Character = GetCharacterBase())
	{
		Character->CustomTimeDilation = 1.0f;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkillAbility::NextAttack()
{
	// 스킬 종료 조건
	if (CurrentTargetIndex == TargetActors.Num()) {
		// 스킬 종료 시 수행할 함수
		FinishAttack();
		return;
	}

	// 현재 타겟을 가져옴
	AActor* Target = TargetActors[CurrentTargetIndex];
	ALuinCharacterBase* Character = GetCharacterBase();

	if (Target && Character) {
		// 타겟의 앞으로 모션 워핑
		UMotionWarpingComponent* MotionWarpComp = Character->FindComponentByClass<UMotionWarpingComponent>();
		if (MotionWarpComp) {
			FVector WarpPos = GetAttackWarpLocation(Target);
			FRotator WarpRot = (Target->GetActorLocation() - WarpPos).Rotation();

			// AttackTarget 과는 다른 모션 워프 이름을 사용하여 공격과는 무관하게 워프하도록 설정.
			MotionWarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(FName("SkillTarget"), WarpPos, WarpRot);
		}

		// 몽타주 실행
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			SkillMontage,
			1.0f,
			NAME_None,
			false
		);

		// 몽타주 종료 시 콜백함수 등록
		if (Task) {
			Task->OnBlendOut.AddDynamic(this, &USkillAbility::OnAttackMontageEnded);
			Task->OnInterrupted.AddDynamic(this, &USkillAbility::OnAttackMontageEnded);
			Task->ReadyForActivation();
		}
	}
	else {
		// 타겟이 없거나 캐릭터를 가져오는데 실패했으면 바로 몽타주 종료
		OnAttackMontageEnded();
	}
}

void USkillAbility::FinishAttack()
{
	ALuinCharacterBase* Character = GetCharacterBase();
	if (Character) {
		// 공격이 끝났으니 원래위치로 돌아옴. 방향또한 처음 방향으로 세팅
		Character->SetActorLocationAndRotation(StartPos, StartRot);
		// 공격이 끝나면 카메라 지연을 원상태로 복귀
		Character->SetCameraLagSpeed(10.0f);

		if (FinishMontage) {
			// 스킬 종료 몽타주 실행
			UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				FinishMontage,
				1.0f,
				NAME_None,
				false
			);

			// 몽타주 종료 콜백함수 등록
			if (Task) {
				Task->OnBlendOut.AddDynamic(this, &USkillAbility::OnFinishMontageEnded);
				Task->ReadyForActivation();
			}
		}
		else {
			OnFinishMontageEnded();
		}
	}
}

void USkillAbility::ApplyDamageToTargets()
{
	// 타겟이 존재하고 GE를 정상적으로 가져왔다면 모든 타겟에게 GE적용
	if (TargetActors.Num() > 0 && KillDamageEffect) {
		ApplyGameplayEffectToTarget(TargetActors, KillDamageEffect, 1.0f);
	}
}

void USkillAbility::OnAttackMontageEnded()
{
	// 공격 몽타주 종료 시 다음 타겟으로 넘어가기 위해 CurrentTargetIndex 증가 및 NextAttack 호출 (콜백 함수를 통한 루프. CurrentTargetIndex 값에 따른 탈출)
	CurrentTargetIndex++;
	NextAttack();
}

void USkillAbility::OnFinishMontageEnded()
{
	// 스킬 종료 몽타주 종료 시 모든 적에게 GE적용 및 GA 종료
	ApplyDamageToTargets();

	ALuinCharacterBase* Character = GetCharacterBase();
	if (Character) {
		// 글로벌 시간, 플레이어 시간 정상화
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		Character->CustomTimeDilation = 1.0f;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

FVector USkillAbility::GetAttackWarpLocation(AActor* Target, float Dist)
{
	// 모션워프할 위치 리턴
	if (!Target) return FVector::ZeroVector;

	// 현재 타겟의 위치
	FVector TargetLoc = Target->GetActorLocation();

	// 타겟을 중심으로 360도 중 한 곳으로 순간이동 할 예정이니까.
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);

	// 타겟을 중심으로 Dist 만큼 떨어질 것.
	FVector Offset = FVector(Dist, 0.0f, 0.0f);
	// 어느 방향으로 Dist만큼 떨어질 것인가 ? 앞서 구한 360도 중 뽑힌 하나의 각도로 회전. 이때 축은 업벡터(Z)
	Offset = Offset.RotateAngleAxis(RandomAngle, FVector::UpVector);

	// 최종적으로 타겟을 중심으로 Dist만큼 RandomAngle 방향으로 모션워프함.
	return TargetLoc + Offset;
}
