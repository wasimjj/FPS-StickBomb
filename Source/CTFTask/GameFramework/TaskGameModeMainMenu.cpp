// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGameModeMainMenu.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "CTFTask/Engine/TaskGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ATaskGameModeMainMenu::BeginPlay()
{
	 TaskGameInstance = Cast<UTaskGameInstance>(GetWorld()->GetGameInstance());
	
}

ATaskGameModeMainMenu::ATaskGameModeMainMenu()
{
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
		if (OnlineSessionPtr.IsValid())
		{
			OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddUObject(
				this, &ATaskGameModeMainMenu::OnSessionComplete);
			OnlineSessionPtr->OnFindSessionsCompleteDelegates.AddUObject(
				this, &ATaskGameModeMainMenu::OnFindSessionsComplete);
			OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddUObject(
				this, &ATaskGameModeMainMenu::OnJoinSessionComplete);
		}
	}
	
}

void ATaskGameModeMainMenu::OnSessionComplete(FName ServerName, bool Success)
{
	GLog->Log("On server created..");
	if (Success && GetWorld() != nullptr)
	{
		if(TaskGameInstance != nullptr)
		{
			TaskGameInstance->PlayerDataStruct.PlayerName = "First player";
			
		}
		GetWorld()->ServerTravel("/Game/Maps/gameplay?listen");
	}
}

void ATaskGameModeMainMenu::OnFindSessionsComplete(const bool bSuccess)
{
	if (bSuccess && OnlineSessionSearch != nullptr)
	{
		TArray<FOnlineSessionSearchResult> OnlineSessionSearchResults = OnlineSessionSearch->SearchResults;
		if (OnlineSessionSearchResults.Num() > 0)
		{
			TArray<FServerInfo> ServerList;
			for(int Index = 0 ; Index < OnlineSessionSearchResults.Num() ; Index++)
			{
				FString ServerName ="";
				OnlineSessionSearchResults[Index].Session.SessionSettings.Get("ServerName",ServerName);
				ServerList.Add(FServerInfo{ServerName,Index});
			}
			OnSearchServerCompletedDelegate.Broadcast(FServerInfoList{ServerList});
		}
	}
}

void ATaskGameModeMainMenu::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UE_LOG(LogTemp,Warning,TEXT ("Joined server.."));
		FString Address = "";
		OnlineSessionPtr->GetResolvedConnectString(SessionName, Address);
		if (!Address.IsEmpty())
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void ATaskGameModeMainMenu::OnRegisterSessionComplete(FName SessionName,
	const TArray<TSharedRef<const FUniqueNetId>>& ListOfPlayers, bool bSuccess)
{
}

void ATaskGameModeMainMenu::CreateServer(const FString ServerName)
{
	UE_LOG(LogTemp,Warning,TEXT ("Create server.."));
	FOnlineSessionSettings OnlineSessionSettings;
	OnlineSessionSettings.bIsDedicated = false;
	OnlineSessionSettings.bIsLANMatch = true;
	OnlineSessionSettings.bAllowJoinInProgress = true;
	OnlineSessionSettings.NumPublicConnections = 2;
	OnlineSessionSettings.bUsesPresence = true;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.Set("ServerName",ServerName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionPtr->CreateSession(0, FName(ServerName), OnlineSessionSettings);
}
void ATaskGameModeMainMenu::JoinServer(const FName ServerName , const int SessionIndex)
{
	if (OnlineSessionPtr != nullptr && OnlineSessionSearch != nullptr && OnlineSessionSearch->SearchResults.Num() > 0)
	{
		UE_LOG(LogTemp,Warning,TEXT ("JoinServer server.. second player "));
		TaskGameInstance->PlayerDataStruct.PlayerName = "Second player";
		TaskGameInstance->PlayerDataStruct.bIsBlueTeam = false;
		OnlineSessionPtr->JoinSession(0, ServerName, OnlineSessionSearch->SearchResults[SessionIndex]);
	}
}
void ATaskGameModeMainMenu::FindServer()
{
	OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch());
	OnlineSessionSearch->bIsLanQuery = true;
	OnlineSessionSearch->MaxSearchResults = 20;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	OnlineSessionPtr->FindSessions(0, OnlineSessionSearch.ToSharedRef());
}

