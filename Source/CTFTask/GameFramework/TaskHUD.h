// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TaskCharacter.h"
#include "GameFramework/HUD.h"
#include "TaskHUD.generated.h"

class UTexture2D;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemPicked , class APickable* , Pickable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitializeDelegate);
UCLASS()
class ATaskHUD : public AHUD
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UTexture2D* CrosshairTex = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> CrosshairTexture;


public:
	ATaskHUD();

	virtual void BeginPlay() override;

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	// UPROPERTY(BlueprintReadOnly,Category="Score")
	// ATaskPlayerState* TaskPlayerStateBlue;
	// UPROPERTY(BlueprintReadOnly,Category="Score")
	// ATaskPlayerState* TaskPlayerStateRed;
	// UPROPERTY(BlueprintAssignable , Category="Setup")
	// FOnInitializeDelegate OnRedTeamInitializeDelegate;
};
