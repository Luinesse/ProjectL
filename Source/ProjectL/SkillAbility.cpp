// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAbility.h"
#include "LuinCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USkillAbility::USkillAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	CurrentTargetIndex = 0;
}

void USkillAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ALuinCharacterBase* Character = GetCharacterBase();

	if (!Character) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartPos = Character->GetActorLocation();
	TargetActors = Character->GetEnemiesInAttackRange(1000.0f, 1000.0f);

	if (TargetActors.Num() == 0) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);
	Character->CustomTimeDilation = 5.0f;

	CurrentTargetIndex = 0;
	NextAttack();
}

void USkillAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	if (ALuinCharacterBase* Character = GetCharacterBase())
	{
		Character->CustomTimeDilation = 1.0f;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkillAbility::NextAttack()
{
	if (CurrentTargetIndex == TargetActors.Num()) {
		FinishAttack();
		return;
	}

	AActor* Target = TargetActors[CurrentTargetIndex];
	ALuinCharacterBase* Character = GetCharacterBase();

	if (Target && Character) {
		UMotionWarpingComponent* MotionWarpComp = Character->FindComponentByClass<UMotionWarpingComponent>();
		if (MotionWarpComp) {
			FVector WarpPos = GetAttackWarpLocation(Target);
			FRotator WarpRot = (Target->GetActorLocation() - WarpPos).Rotation();

			MotionWarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(FName("SkillTarget"), WarpPos, WarpRot);
		}

		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			SkillMontage,
			1.0f,
			NAME_None,
			false
		);

		if (Task) {
			Task->OnBlendOut.AddDynamic(this, &USkillAbility::OnAttackMontageEnded);
			Task->OnInterrupted.AddDynamic(this, &USkillAbility::OnAttackMontageEnded);
			Task->ReadyForActivation();
		}
	}
	else {
		OnAttackMontageEnded();
	}
}

void USkillAbility::FinishAttack()
{
	ALuinCharacterBase* Character = GetCharacterBase();
	if (Character) {
		Character->SetActorLocation(StartPos);

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		Character->CustomTimeDilation = 1.0f;

		if (FinishMontage) {
			UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				FinishMontage,
				1.0f,
				NAME_None,
				false
			);

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
	if (TargetActors.Num() > 0 && KillDamageEffect) {
		ApplyGameplayEffectToTarget(TargetActors, KillDamageEffect, 1.0f);
	}
}

void USkillAbility::OnAttackMontageEnded()
{
	CurrentTargetIndex++;
	NextAttack();
}

void USkillAbility::OnFinishMontageEnded()
{
	ApplyDamageToTargets();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

FVector USkillAbility::GetAttackWarpLocation(AActor* Target, float Dist)
{
	if (!Target) return FVector::ZeroVector;
	return Target->GetActorLocation() + (Target->GetActorForwardVector() * Dist);
}
