// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseNotify.h"
#include "Perception/AISense_Hearing.h"
#include "Components/SkeletalMeshComponent.h"

void UNoiseNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner()) {
		// 노티파이 실행 시 노이즈 발생
		UAISense_Hearing::ReportNoiseEvent(MeshComp->GetWorld(), MeshComp->GetComponentLocation(), VolumeMultiplier, MeshComp->GetOwner(), MaxRange);
	}
}
