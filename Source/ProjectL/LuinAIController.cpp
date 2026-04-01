// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

ALuinAIController::ALuinAIController()
{
	// 퍼셉션 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

	// AIController 의 PerceptionComponent로 금방 생성한 AIPerception을 사용하도록 설정
	SetPerceptionComponent(*AIPerceptionComponent);

	// 시야 감각 생성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 1000의 거리까지 인식, 1200을 넘어서면 사라졌다고 판단. 시야각은 90도
	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;

	// 수명 설정
	SightConfig->SetMaxAge(5.0f);

	// 적과 중립과 아군 모두 인식하도록함.
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// 생성한 시야 감각을 퍼셉션에 등록
	AIPerceptionComponent->ConfigureSense(*SightConfig);

	// 청각 세팅
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	// 3000의 거리까지 들음. 시야 확보 시 3500까지 들음.
	HearingConfig->HearingRange = 3000.0f;
	HearingConfig->LoSHearingRange = 3500.0f;

	// 적, 중립, 아군 모두 인식
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// 퍼셉션에 등록
	AIPerceptionComponent->ConfigureSense(*HearingConfig);

	// 통각 생성 (맞게되면 바로 인식됨. 별도의 세팅X)
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	// 퍼셉션에 등록
	AIPerceptionComponent->ConfigureSense(*DamageConfig);

	// 주 감각을 시각으로 등록
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ALuinAIController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Is It Right ?"));

	// 퍼셉션 업데이트 시 콜백함수 등록. 현재는 시야뿐이므로 시야감각 업데이트 시 호출함.
	if (AIPerceptionComponent) {
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ALuinAIController::OnTargetDetected);
	}

	// 비헤이비어 트리 실행
	if (AIBehaviorTree) {
		RunBehaviorTree(AIBehaviorTree);
	}
}

void ALuinAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)	return;

	// 업데이트 된 대상이 Player 태그를 가지고 있다면
	if (Actor->ActorHasTag("Player")) {
		if (Stimulus.WasSuccessfullySensed()) {
			UE_LOG(LogTemp, Warning, TEXT("Find Player"));

			FAISenseID SenseID = Stimulus.Type;

			if (SenseID == UAISense::GetSenseID<UAISense_Damage>()) {
				UE_LOG(LogTemp, Warning, TEXT("Damage !"));
			}
			else if (SenseID == UAISense::GetSenseID<UAISense_Sight>()) {
				UE_LOG(LogTemp, Warning, TEXT("Sight !"));
			}

			// 블랙보드의 TargetActor를 업데이트된 대상으로 설정
			if (GetBlackboardComponent()) {
				GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Lose Player"));

			// 시야에서 사라졌을 때이므로, TargetActor의 값을 초기화
			// 시야에서 사라지자마자 ClearValue 하면 SetMaxAge 함수가 의미없어짐.
			// 따라서, 일단 주석처리로 없애고 실행.
			/*if (GetBlackboardComponent()) {
				GetBlackboardComponent()->ClearValue(FName("TargetActor"));
			}*/
		}
	}
}
