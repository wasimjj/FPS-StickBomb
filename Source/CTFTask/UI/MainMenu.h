// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class CTFTASK_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta =(BindWidget))
	UButton* ButtonJoinServer;
	UPROPERTY(meta =(BindWidget))
	UButton* ButtonCreateServer;
};
