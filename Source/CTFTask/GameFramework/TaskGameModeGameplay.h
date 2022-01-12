// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Pickable.h"
#include "CoreMinimal.h"
#include "TaskGameMode.h"
#include "TaskGameModeGameplay.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlagCapturedDelegate,bool, bIsBlueTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotifyDelegate);
UCLASS()
class CTFTASK_API ATaskGameModeGameplay : public ATaskGameMode
{
	GENERATED_BODY()
	protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
public:
	ATaskGameModeGameplay();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION()
	void SpawnPickable(TSubclassOf<APickable> Pickable, FVector SpawnLocation);
	
	public:
	UPROPERTY(EditDefaultsOnly, Category="Team Base")
	TSubclassOf<APickable> AmmoPreset;
	UPROPERTY(EditDefaultsOnly, Category="Team Base")
	TSubclassOf<APickable> BombPreset;

	//UPROPERTY()
	//FOnFlagCapturedDelegate OnBombCapturedDelegate;
	UPROPERTY(BlueprintAssignable , Category="Game Start")
	FNotifyDelegate OnGameStartDelegate;

	UPROPERTY(BlueprintReadOnly , Category="Players")
	int PlayersInGame;
	UPROPERTY(BlueprintReadOnly , Category="Rule")
	int MaxPlayer = 2;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Rule")
	int GameMatchTime = 600;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Rule")
	int AmmoCount = 3;
};
