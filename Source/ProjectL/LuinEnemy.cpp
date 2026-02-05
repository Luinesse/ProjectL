// Fill out your copyright notice in the Description page of Project Settings.


#include "LuinEnemy.h"

ALuinEnemy::ALuinEnemy()
{
	// 식별 전용 태그
	Tags.Add(FName("Enemy"));
}

void ALuinEnemy::Die(AActor* DamageCauser)
{
	Super::Die(DamageCauser);

	// 적 캐릭터의 사망 처리가 들어갈 영역
}
