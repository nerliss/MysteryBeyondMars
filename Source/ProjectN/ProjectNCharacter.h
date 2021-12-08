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
	bool bHanging;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bInWater;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSubmerged;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	FString CurrentObjective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water|Oxygen")
	float Oxygen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water|Oxygen")
	float OxygenMax;

	FTimerHandle SubstractOxygenTimer;

	FTimerHandle AddOxygenTimer;

	/** Used in fade in\out animation for Oxygen Bar. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Water|Movement")
	void OnSubmerged();

	/** Used in fade in\out animation for Oxygen Bar. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Water|Movement")
	void OnEmerged();

	/** Used in Oxygen Bar set percent. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Water|Movement")
	void OnOxygenSubstracted();

	/** Used in Oxygen Bar set percent. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Water|Movement")
	void OnOxygenAdded();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Sounds")
	USoundBase* CharacterDeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Sounds")
	USoundBase* DeathImpactSound;

	/** Our variable for checking whether keyboard input is active or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "POV")
	bool bKeyboardInputEnabled;

	/** Checks if we are dead before using Death(). 
	 * This should fix some cases when Death() is called more than once simultaneously. 
	 */
	bool bDiedAlready;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Sounds")
	USoundBase* FlashlightSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight|Components")
	USpotLightComponent* Flashlight;

	bool bFlashlightTurnedOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNHealthComponent* HealthComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water|Components")
	UBoxComponent* BoxHead;

	/** First person POV. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "POV")
	bool isFP; 

	/** Third person POV. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "POV")
	bool isTP; 

	/** Trace length that depends on current POV. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	float TraceLength;

	// TargetBoom lengths
	float MaxTargetBoomLength;
	float MinTargetBoomLength;

	void SwitchCameraPOV();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bOnWall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WallRun|Variables")
	bool bCanWallRun;

	/** 
	 * Called on wallrun timeline update node.
	 * @param PlayerWallDirectionsDotProduct  Dot product between player's forward vector and wall's normal that he is facing
	 * @param WallRunSpeed	Wallrunning speed that is passed into LaunchCharacter()
	 */
	UFUNCTION(BlueprintCallable, Category = "WallRun|Functions")
	void InitWallRun(float PlayerWallDirectionsDotProduct, float WallRunSpeed);

	void SwimForward(float Value);

	void SwimRight(float Value);

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

	/** Crouch. */
	void Crouch();

	/** UnCrouch. */
	void StopCrouching();

	/** Jump. */
	void Jump();

	/** Stop jumping. */
	void StopJumping();

	/** Float up while in water. */
	UFUNCTION(BlueprintCallable, Category = "Water|Movement")
	void FloatUp(float Value);

	/** Dive while in water. */
	UFUNCTION(BlueprintCallable, Category = "Water|Movement")
	void Dive(float Value);

	UFUNCTION()
	void AddOxygen();

	UFUNCTION()
	void SubstractOxygen();

	/** Death function with saving pose snapshot. */
	UFUNCTION(BlueprintCallable, Category = "Health|Death|Functions")
	void Death();

	/** Called in blueprints to add some cosmetic stuff that is difficult/impossible to implement in C++ to death event
	 * (i.e. timelines, widgets that are created in BP etc.)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Health|Death|Functions")
	void OnDied();

	FTimerHandle CallDeathTimer;

	/** Calls Death() after a small delay and changes POV to Third Person if was in first. */
	UFUNCTION(BlueprintCallable, Category = "Health|Death|Functions")
	void CallDeath();

	/** Save pose snapshot and turn off collision for mesh. */
	void RagdollSnapshot();

	/** Turn off / turn on the flash light. */
	void SwitchFlashlight();

	/** This function allows to set JumpCurrentCount from Blueprints. */
	UFUNCTION(BlueprintCallable, Category = "Jump")
	void SetJumpCurrentCount(int NewCurrentJumpCount);

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

