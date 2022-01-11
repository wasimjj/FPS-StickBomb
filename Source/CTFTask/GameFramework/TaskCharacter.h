// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CTFTask/Engine/TaskGameInstance.h"
#include "GameFramework/Character.h"
#include "Player/TaskPlayerState.h"

#include "TaskCharacter.generated.h"

class UInputComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateInitializeDelegate);

UCLASS(config=Game)
class ACTFTaskCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Character body for others player can see*/
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* MeshCharacterBody;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class UStaticMeshComponent* BombMesh;
	/** Gun mesh: for the mesh body to for other players*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* ClientBodyGun;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	ACTFTaskCharacter();


protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	//void OnStartGame();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AddControllerPitchInput(float Val) override;
	virtual void AddControllerYawInput(float Val) override;
	virtual void PostInitializeComponents() override;
	virtual void OnRep_PlayerState() override;
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ACTFTaskProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* DeathMontage;
	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

protected:
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData()
		{
			bIsPressed = false;
			Location = FVector::ZeroVector;
		}

		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};

	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData TouchItem;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rotation")
	FRotator CharacterRotationCorrection;


protected:
	UFUNCTION(Server, Reliable)
	void OnFireServer(const FVector Location, const FRotator Rotation);
	void OnFireServer_Implementation(const FVector Location, const FRotator Rotation);
	UFUNCTION(Server, Unreliable)
	void CorrectRotationOnServer(FRotator Rotator);
	void CorrectRotationOnServer_Implementation(FRotator Rotator);
	UFUNCTION(NetMulticast, Unreliable)
	void CorrectRotationMulticast(FRotator Rotator);
	UFUNCTION(BlueprintCallable, Category="Rotation")
	void CorrectRotationMulticast_Implementation(FRotator Rotator);
public:
	UFUNCTION()
	void PlayDeathAnimation();
protected:
	UPROPERTY()
	class ATaskGameModeGameplay* TaskGameModeGameplay;
	UPROPERTY()
	class ATaskGameStateBase* TaskGameStateBase;
	UPROPERTY()
	UAnimInstance* AnimInstanceSelf;
	UPROPERTY()
	UAnimInstance* AnimInstanceEnemy;
	
	UPROPERTY()
	class UMaterialInstanceDynamic* BlinkMaterial;
public:
	
	UFUNCTION(BlueprintCallable, Category="Bomb")
	void SetBombVisibility(const bool bVisibility);
	UFUNCTION(Server, Reliable)
	void PlayerStateSetupInternal(FPlayerDataStruct PlayerDataStruct);
	UFUNCTION()
	void PlayerStateSetup();
	UFUNCTION()
	void OnPlayerDeath();
	UFUNCTION()
	FVector GetLocationNearMyBase();
	UFUNCTION()
	void ReSpawnMe();
	UFUNCTION()
	void OnGameStart();
protected:
	float DelayToBlink = 1;
	float TotalBlinkTime = 4;
	float blink = 0.0f;
	UPROPERTY()
	UTaskGameInstance* TaskGameInstance;
public:
	UPROPERTY(BlueprintReadOnly, Category="PlayerState")
	ATaskPlayerState* TaskPlayerState;
	UPROPERTY(BlueprintAssignable, Category="State Delegate")
	FOnStateInitializeDelegate OnStateInitializeDelegate;
};
