// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskCharacter.h"
#include "TaskProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "TaskGameModeGameplay.h"
#include "TaskGameStateBase.h"

#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "CTFTask/Engine/TaskGameInstance.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACTFTaskCharacter

ACTFTaskCharacter::ACTFTaskCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	//Create mesh body for client
	MeshCharacterBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshCharacterBody"));
	MeshCharacterBody->SetOnlyOwnerSee(false);
	MeshCharacterBody->SetOwnerNoSee(true);
	MeshCharacterBody->CastShadow = false;
	MeshCharacterBody->SetupAttachment(RootComponent);
	MeshCharacterBody->SetRelativeRotation(FRotator(0, -90.0f, 0));
	MeshCharacterBody->SetRelativeLocation(FVector(0, 0, -90));
	//Create mesh for flag
	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	BombMesh->SetupAttachment(RootComponent);
	BombMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BombMesh->SetOnlyOwnerSee(false);
	BombMesh->SetOwnerNoSee(true);
	BombMesh->CastShadow = false;
	BombMesh->SetVisibility(false);
	BlinkMaterial = UMaterialInstanceDynamic::Create(BombMesh->GetMaterial(0),nullptr);
	BombMesh->SetMaterial(0,BlinkMaterial);
	// Create a gun mesh component
	ClientBodyGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClientBodyGun"));
	ClientBodyGun->SetOnlyOwnerSee(false);
	ClientBodyGun->SetOwnerNoSee(true);
	ClientBodyGun->bCastDynamicShadow = false;
	ClientBodyGun->CastShadow = false;
	ClientBodyGun->SetupAttachment(MeshCharacterBody);
	ClientBodyGun->SetRelativeRotation(FRotator(0, 0, 0));
	ClientBodyGun->SetRelativeLocation(FVector(0, 0, 0));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true); // only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true); // only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f)); // Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
	//
	//Initialize the player's Health
	PrimaryActorTick.bCanEverTick = true;
}
void ACTFTaskCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(TaskPlayerState &&  TaskPlayerState->bIsBombCaptured)
	{
		if(TotalBlinkTime > 0  )
		{

			DelayToBlink -= DeltaSeconds;
			TotalBlinkTime -= DeltaSeconds;
			if( DelayToBlink <= 0)
			{
				DelayToBlink = FMath::Clamp(TotalBlinkTime/4 ,0.1f,1.0f);
				blink = blink > 0 ? 0 : 1;
				if(!BlinkMaterial)
				{
			
					BlinkMaterial = UMaterialInstanceDynamic::Create(BombMesh->GetMaterial(0),nullptr);
				}
				BombMesh->SetMaterial(0,BlinkMaterial);
				BlinkMaterial->SetScalarParameterValue("Blink",blink);
				GLog->Log(FString::Printf(TEXT("Blink = %f"),blink));

			}
			return;
		}
		PlayDeathAnimation();
		TaskPlayerState->bIsBombCaptured = false;
	}
	
	
}
void ACTFTaskCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (!IsLocallyControlled()) // if remote player 
	{
		ClientBodyGun->AttachToComponent(MeshCharacterBody,
		                                 FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		                                 TEXT("GunSocket"));
	}
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
	                          TEXT("GripPoint"));


	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	if(GetLocalRole() == ROLE_Authority)
	{
		TaskGameModeGameplay = Cast<ATaskGameModeGameplay>( UGameplayStatics::GetGameMode(GetWorld()));
	}

	AnimInstanceSelf = Mesh1P->GetAnimInstance();
	AnimInstanceEnemy = MeshCharacterBody->GetAnimInstance();
	TaskGameStateBase =  Cast<ATaskGameStateBase>( UGameplayStatics::GetGameState(GetWorld()));
	if(HasAuthority())
	{
		TaskGameModeGameplay->OnGameStartDelegate.AddDynamic(this,&ACTFTaskCharacter::OnGameStart);// This character is also server
	}
	if(HasAuthority())
	{
		//PlayerStateSetup();
	}
}



void ACTFTaskCharacter::OnGameStart()
{
	OnStateInitializeDelegate.Broadcast(); // fire again for listen server 

}
void ACTFTaskCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	PlayerStateSetup();
	GLog->Log(FString::Printf(TEXT("OnRep_PlayerState local Role :: %d"),GetLocalRole()));
}
void ACTFTaskCharacter::PlayerStateSetup()
{
	 if (GetPlayerState())
	 {
	 	TaskPlayerState = Cast<ATaskPlayerState> (GetPlayerState());
	 	SetPlayerState(TaskPlayerState);
	 	GLog->Log(FString::Printf(TEXT("PlayerStateSetup local Role :: %d"),GetLocalRole()));
	 	TaskPlayerState->OnPlayerDeathDelegate.AddDynamic(this,&ACTFTaskCharacter::OnPlayerDeath);
	 }
	  TaskGameInstance = Cast<UTaskGameInstance>(GetWorld()->GetGameInstance());
	 if (IsLocallyControlled())
	 {
	 	PlayerStateSetupInternal(TaskGameInstance->PlayerDataStruct);
	 	OnStateInitializeDelegate.Broadcast();
	 }
}
void ACTFTaskCharacter::OnPlayerDeath()
{
	GLog->Log("My Player Dead.....");
}

FVector ACTFTaskCharacter::GetLocationNearMyBase()
{
	
	return FVector::ZeroVector;
}

void ACTFTaskCharacter::ReSpawnMe()
{
	
	
}

void ACTFTaskCharacter::PlayerStateSetupInternal_Implementation(FPlayerDataStruct PlayerDataStruct)
{
	if (TaskPlayerState)
	{
		TaskPlayerState->SetPlayerName(PlayerDataStruct.PlayerName);
	}
}


void ACTFTaskCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}




float ACTFTaskCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser)
{
	AnimInstanceEnemy->Montage_Play(HitReactMontage, 1.f);
	if(HasAuthority())
	{
	}
	
	return DamageAmount;
}

void ACTFTaskCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}


//////////////////////////////////////////////////////////////////////////
// Input

void ACTFTaskCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACTFTaskCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACTFTaskCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACTFTaskCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACTFTaskCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ACTFTaskCharacter::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &ACTFTaskCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ACTFTaskCharacter::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &ACTFTaskCharacter::LookUpAtRate);
}

void ACTFTaskCharacter::AddControllerPitchInput(float Val)
{
	if (Val != 0.f)
	{
		Super::AddControllerPitchInput(Val);
		if (IsLocallyControlled())
		{
			if (GetLocalRole() == ROLE_Authority)
			{
				CorrectRotationMulticast(GetControlRotation());
				return;
			}
			
			CorrectRotationOnServer(GetControlRotation());
		}
	}
}

void ACTFTaskCharacter::AddControllerYawInput(float Val)
{
	if (Val != 0.f)
	{
		Super::AddControllerYawInput(Val);
	}
}


void ACTFTaskCharacter::OnFire()
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				GetWorld()->SpawnActor<ACTFTaskProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
				return;
			}
			
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			FVector SpawnLocation = ((FP_MuzzleLocation != nullptr)
				                               ? FP_MuzzleLocation->GetComponentLocation()
				                               : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			OnFireServer(SpawnLocation, SpawnRotation);
			
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr && AnimInstanceSelf != nullptr)
	{
		AnimInstanceSelf->Montage_Play(FireAnimation, 1.f);
	}
}

void ACTFTaskCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACTFTaskCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ACTFTaskCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (!TouchItem.bIsPressed)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.

void ACTFTaskCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACTFTaskCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACTFTaskCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACTFTaskCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ACTFTaskCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACTFTaskCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACTFTaskCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ACTFTaskCharacter::TouchUpdate);
		return true;
	}

	return false;
}

void ACTFTaskCharacter::OnFireServer_Implementation(const FVector Location, const FRotator Rotation)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Instigator = GetInstigator();
	ActorSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	GetWorld()->SpawnActor<ACTFTaskProjectile>(ProjectileClass, Location, Rotation, ActorSpawnParams);
}

void ACTFTaskCharacter::CorrectRotationOnServer_Implementation(FRotator Rotator)
{
	CharacterRotationCorrection = Rotator;
	if (!IsLocallyControlled())
	{
		FirstPersonCameraComponent->SetWorldRotation(Rotator);
	}
}

void ACTFTaskCharacter::CorrectRotationMulticast_Implementation(FRotator Rotator)
{
	CharacterRotationCorrection = Rotator;
	if (!IsLocallyControlled())
	{
		FirstPersonCameraComponent->SetWorldRotation(Rotator);
	}
}

void ACTFTaskCharacter::PlayDeathAnimation()
{
	AnimInstanceEnemy->Montage_Play(DeathMontage, 1.f);
	BombMesh->SetVisibility(false);
	if(HasAuthority())
	{
		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;
		TimerDel.BindUFunction(this, FName("ReSpawnMe"), 0, 0.0);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.50f, false);
	}
}


void ACTFTaskCharacter::SetBombVisibility(const bool bVisibility)
{
	if(!TaskPlayerState)
	{
		TaskPlayerState = Cast<ATaskPlayerState> (GetPlayerState());
	}
	
	TaskPlayerState->bIsBombCaptured = bVisibility;
	BombMesh->SetVisibility(bVisibility);
}
