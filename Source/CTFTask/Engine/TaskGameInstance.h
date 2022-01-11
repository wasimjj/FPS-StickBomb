// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CTFTask/GameFramework/Player/TaskPlayerState.h"
#include "Engine/GameInstance.h"
#include "TaskGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPlayerDataStruct
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite , Category="Player Data")
	FString PlayerName;
	UPROPERTY(BlueprintReadWrite , Category="Player Data")
	bool bIsBlueTeam = true;
	
};
UCLASS()
class CTFTASK_API UTaskGameInstance : public UGameInstance
{
	GENERATED_BODY()
	public:
	UTaskGameInstance();
	UPROPERTY(BlueprintReadOnly,Category="Player State")
	FPlayerDataStruct PlayerDataStruct;
};
