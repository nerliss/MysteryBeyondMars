// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectNCharacter.generated.h"

class UNHealthComponent;
class USpotLightComponent;
class USoundBase;
class UBoxComponent;

UCLASS(config = Game)
class AProjectNCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AProjectNCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bOnWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bHanging;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bInWater;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSubmerged;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	FString CurrentObjective;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float Oxygen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float OxygenMax;

	FTimerHandle SubstractOxygenTimer;

	FTimerHandle AddOxygenTimer;

	// Primarily used for UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Water movement events")
	void OnSubmerged();

	// Primarily used for UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Water movement events")
	void OnEmerged();


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight")
	USoundBase* FlashlightSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* Flashlight;

	bool bFlashlightTurnedOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNHealthComponent* HealthComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxHead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "POV")
	bool isFP; // first person POV

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "POV")
	bool isTP; // third person POV

	// TargetBoom lengths
	float MaxTargetBoomLength;
	float MinTargetBoomLength;

	void SwitchCameraPOV();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

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

// 	/** Handler for when a touch input begins. */
// 	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
// 
// 	/** Handler for when a touch input stops. */
// 	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/* Crouch*/
	void Crouch();

	void StopCrouching();

	/* Jump */
	void Jump();

	void StopJumping();

	/* Float up while in water */
	UFUNCTION(BlueprintCallable, Category = "Water Movement")
	void FloatUp(float Value);

	/* Dive while in water */
	UFUNCTION(BlueprintCallable, Category = "Water Movement")
	void Dive(float Value);

	UFUNCTION()
	void AddOxygen();

	UFUNCTION()
	void SubstractOxygen();

	/* Death function with saving pose snapshot */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Death();

	/* Save pose snapshot and turn off collision for mesh */
	void RagdollSnapshot();

	/* Turn off / turn on the flash light */
	void SwitchFlashlight();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

