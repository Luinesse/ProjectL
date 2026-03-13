// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "LuinAIController.generated.h"

/**
 * 
 */

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;

UCLASS()
class PROJECTL_API ALuinAIController : public AAIController
{
	GENERATED_BODY()
public:
	ALuinAIController();

	// 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* AIBehaviorTree;

protected:
	virtual void BeginPlay() override;

private:
	// AIPerception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerceptionComponent;

	// 시야 감각
	UAISenseConfig_Sight* SightConfig;

	// 퍼셉션 업데이트 시 호출될 콜백함수 (현재는 시야뿐이므로 시야에 들어오거나 시야에서 사라졌을 때)
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
};
