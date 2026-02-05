// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboAttack.h"
#include "LuinPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UComboAttack::UComboAttack()
{
	// GA가 실행될 때, 새로운 개체를 만들지, 기존 개채를 재사용할지 정의.
	// NonInstance 로 두면, 이 GA가 호출될 때, 인스턴스를 두지 않고 이 GA 하나 자체가 실행됨.
	// 따라서, NonInstance일 때는 다른 캐릭터에서 GA를 호출해도 이 GA 클래스 내부 변수를 공유함.
	// 가령 다른 캐릭터가 GA_ComboAttack을 했을 때, 그 캐릭터는 최초 공격으로 첫번째 콤보가 나가야하지만, 공유돼서 2번째 콤보가 나갈 수 있음.
	// 필자는 NonInstance를 static 변수 혹은 싱글톤 패턴과 유사하다고 이해.
	// InstancedPerActor 는 각 액터마다 해당 GA의 인스턴스를 가지기 때문에, 변수의 값이 독립적으로 존재함.
	// 따라서, 위에서 서술한 버그가 발생하지 않음.
	// 대신 이 경우 액터마다 인스턴스를 생성하기에 메모리를 많이 잡아먹음. (즉, NonInstance는 변수를 공유해도 문제없을 때 최적화 유리.)
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 실행할 콤보 몽타주가 없다면 GA 실행안함.
	if (!ComboMontage) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 최초는 첫번째 콤보
	CurrentCombo = 1;

	// 이벤트 태그 확인.
	FGameplayTag HitTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.AttackHit"));

	// 이벤트 태그를 기다림.
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,			// 실행한 GA
		HitTag,			// 수신 대기할 태그
		nullptr,		// 어디에서 오는걸 기다릴것인지 ? 싱글플레이니까 nullptr(나 자신)
		false,			// 한번만 받을 것인지 ?
		false			// 태그가 완전히 같아야 하는지 ? (false로 두면 자식 태그도 허용 Event.Montage.AttackHit.~~~ 도 허용한다는거임?)
	);

	// 이벤트 태그를 수신하면 OnGameplayEventReceived 콜백 함수 호출
	EventTask->EventReceived.AddDynamic(this, &UComboAttack::OnGameplayEventReceived);
	// 이벤트 태그를 수신하면 콜백함수를 호출하기로 작성한 내용을 GAS 시스템에 등록. (즉, 실행 대기)
	EventTask->ReadyForActivation();

	// 콤보 몽타주 실행
	// 시작 섹션은 Combo1
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("PlayCombo"),
		ComboMontage,
		1.0f,
		FName("Combo1")
	);

	// 거의 끝날때, 도중에 모종의 이유로 멈췄을때, 취소됐을 때, GA 끝냄.(OnMontageEnded 함수 내부에 EndAbility를 호출하므로)
	Task->OnBlendOut.AddDynamic(this, &UComboAttack::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UComboAttack::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &UComboAttack::OnMontageEnded);
	Task->ReadyForActivation();
}

void UComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// 현재 공격이 끝났으니 콤보는 다시 0.
	CurrentCombo = 0;
}

void UComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 현재 공격이 최대 콤보를 넘어섰으면 입력 더 안받아도됨.
	if (CurrentCombo >= MaxCombo)	return;

	// 캐릭터의 애님 인스턴스를 가져옴. (애니메이션의 섹션을 넘어가기 위해)
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || !Character->GetMesh()->GetAnimInstance())	return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	// 현재 섹션과 다음 섹션을 구함.
	FName CurrentSection = *FString::Printf(TEXT("Combo%d"), CurrentCombo);
	FName NextSection = *FString::Printf(TEXT("Combo%d"), CurrentCombo + 1);

	// 몽타주의 다음 섹션을 실행.
	AnimInstance->Montage_SetNextSection(CurrentSection, NextSection, ComboMontage);

	// 콤보 1 증가
	CurrentCombo++;
}

void UComboAttack::OnMontageEnded()
{
	// 콤보 갱신이 없든, 다 때렸든 몽타주가 종료되면 GA도 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UComboAttack::OnGameplayEventReceived(FGameplayEventData Payload)
{
	// 이벤트 태그 수신 시 BP에 정의한 내용 호출
	HitCheck();
}