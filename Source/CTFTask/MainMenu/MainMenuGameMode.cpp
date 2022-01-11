// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

#include "Blueprint/UserWidget.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	FStringClassReference MyWidgetClassRef(TEXT("/Game/Blueprint/Widget/WBP_MainaMenu.WBP_MainaMenu_C"));
	if ( UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UUserWidget>() )
	{
	//	UUserWidget* MyWidget = CreateWidget<UUserWidget>(World / GameInstance / PlayerController, MyWidgetClass);
		// Do stuff with MyWidget
	}
}
