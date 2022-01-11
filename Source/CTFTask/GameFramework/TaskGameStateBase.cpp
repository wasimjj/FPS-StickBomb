// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGameStateBase.h"

#include "TaskGameModeGameplay.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ATaskGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATaskGameStateBase, BlueTeamScore);
	DOREPLIFETIME(ATaskGameStateBase, RedTeamScore);
	DOREPLIFETIME(ATaskGameStateBase, GameStateType);
	DOREPLIFETIME(ATaskGameStateBase,GameMatchTimer);
}

void ATaskGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		TaskGameModeGameplay = Cast<ATaskGameModeGameplay>(UGameplayStatics::GetGameMode(GetWorld()));
		TaskGameModeGameplay->OnGameStartDelegate.AddDynamic(this,&ATaskGameStateBase::OnGameStart);
	}
}
void ATaskGameStateBase::OnGameStart()
{
	UpdateGameState(EGameState::EGamePlay);
	GameMatchTimer = TaskGameModeGameplay->GameMatchTime;
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;
	TimerDel.BindUFunction(this, FName("UpdateGameTimer"), 0, 0.0);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1, true);

}
void ATaskGameStateBase::UpdateGameTimer()
{
	GameMatchTimer--;
	CheckIfTimerEnd();

}
void ATaskGameStateBase::UpdateGameState(EGameState GameState)
{
	GameStateType = GameState;
	OnGameStateChange();
}

void ATaskGameStateBase::AddScore(const bool bIsBlueTeam)
{
	UE_LOG(LogTemp, Warning, TEXT("I am from team::%d "), bIsBlueTeam)
	if (bIsBlueTeam)
	{
		BlueTeamScore++;
	}
	else
	{
		RedTeamScore++;
	}
}

void ATaskGameStateBase::OnRep_OnChangeGameStateType()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Satte ::%d"), GameStateType);
	OnGameStateChange();
}

void ATaskGameStateBase::OnRep_OnRedTeamScore()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_OnRedTeamScore ::%d"), RedTeamScore);
}

void ATaskGameStateBase::OnRep_OnGameTimerUpdate()
{
	CheckIfTimerEnd();
}

void ATaskGameStateBase::OnGameStateChange()
{
	switch (GameStateType)
	{
	case EGameState::EGameEnd:
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		OnGameEndDelegate.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("OnGameEndDelegate Fired "));
		break;
	case EGameState::EGameLobby:
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		break;
	case EGameState::EGamePlay:
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		OnGameStartDelegate.Broadcast();
		break;
	}
}

void ATaskGameStateBase::OnRep_OnBlueTeamScore()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_OnBlueTeamScore ::%d"), BlueTeamScore);
}
void ATaskGameStateBase::CheckIfTimerEnd()
{
	if(GameMatchTimer <= 0)
	{
		UpdateGameState(EGameState::EGameEnd);
		if(GameMatchTimer <= 0)
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		}
	}
}

