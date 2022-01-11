// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskPlayerState.h"

#include "CTFTask/GameFramework/TaskCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


void ATaskPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATaskPlayerState, bIsBombCaptured);
	DOREPLIFETIME(ATaskPlayerState, AmmoCount);
	DOREPLIFETIME(ATaskPlayerState, bIsDead);

}

void ATaskPlayerState::BeginPlay()
{
	
}


void ATaskPlayerState::OnRep_AmmoCount()
{
	GLog->Log(FString::Printf(TEXT("OnRep_AmmoCount Ammo ::: %d"),AmmoCount));
}

void ATaskPlayerState::OnRep_IsBombCaptured()
{
	bIsBombCaptured = true;
}


void ATaskPlayerState::OnRep_IsDead()
{
	OnPlayerDeathDelegate.Broadcast();
}

