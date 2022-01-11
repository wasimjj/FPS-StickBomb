// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGameModeGameplay.h"
#include "TaskHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void ATaskGameModeGameplay::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PlayersInGame++;
	//if (PlayersInGame >= MaxPlayer)
	{
		//StartGame();
	}
}

ATaskGameModeGameplay::ATaskGameModeGameplay()
{
	HUDClass = ATaskHUD::StaticClass();
}

void ATaskGameModeGameplay::BeginPlay()
{
}

void ATaskGameModeGameplay::StartGame()
{
	SpawnPickable(AmmoPreset,FVector(-1700.0f, 1400.0f, 270.0f));
	SpawnPickable(BombPreset,FVector(30.0f, -1400.0f, 270.0f));
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	OnGameStartDelegate.Broadcast();
}


void ATaskGameModeGameplay::SpawnPickable(TSubclassOf<APickable> Pickable, FVector SpawnLocation)
{
	if(Pickable)
	{
		GetWorld()->SpawnActor<APickable>(Pickable, SpawnLocation,
															  FRotator::ZeroRotator);
	}
}
