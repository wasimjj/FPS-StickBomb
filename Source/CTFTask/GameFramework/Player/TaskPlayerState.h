// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TaskPlayerState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureFlagStatusUpdateDelegate, bool, bIsCapture);


UCLASS()
class CTFTASK_API ATaskPlayerState : public APlayerState
{
	GENERATED_BODY()
	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
public:
	
	UPROPERTY(ReplicatedUsing=OnRep_AmmoCount , BlueprintReadOnly, Category="Health")
	int AmmoCount;
	UPROPERTY(ReplicatedUsing=OnRep_IsBombCaptured)
	bool bIsBombCaptured;
	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead;
	UFUNCTION()
	void OnRep_AmmoCount();
	UFUNCTION()
	void OnRep_IsBombCaptured();
	UFUNCTION()
	void OnRep_IsDead();
	UPROPERTY(BlueprintAssignable , Category="Score")
	FOnScoreUpdateDelegate OnPlayerDeathDelegate;
	UPROPERTY(BlueprintAssignable , Category="Score")
	FOnScoreUpdateDelegate OnAmmoUpdateDelegate;

	
};
