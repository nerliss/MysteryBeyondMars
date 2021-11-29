// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/NHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Print string on screen macro
#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

//////////////////////////////////////////////////////////////////////////
// AProjectNCharacter

AProjectNCharacter::AProjectNCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f; // default is 600
	GetCharacterMovement()->AirControl = 1.f; // default is 0.2

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent(), "head");
	CameraBoom->TargetArmLength = MaxTargetBoomLength; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a health component
	HealthComp = CreateDefaultSubobject<UNHealthComponent>(TEXT("HealthComp"));

	// Create and configure flashlight
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(GetMesh(), "ChestFlashlightSocket");
	Flashlight->SetVisibility(false);
	Flashlight->Intensity = 30000.f;
	Flashlight->LightColor = FColor::FromHex("E2A119FF");
	Flashlight->AttenuationRadius = 16384.f;
	Flashlight->OuterConeAngle = 22.f;
	Flashlight->SoftSourceRadius = 100.f;
	Flashlight->SourceLength = 100.f;
	Flashlight->bUseTemperature = true;
	Flashlight->IntensityUnits = ELightUnits::Unitless;

	bFlashlightTurnedOn = false;

	// Create box collision for head
	BoxHead = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxHead"));
	BoxHead->SetupAttachment(GetMesh(), "head");
	BoxHead->SetBoxExtent(FVector(8, 11, 11));
	BoxHead->SetRelativeLocation(FVector(19, 3, 1));

	// Camera 
	MaxTargetBoomLength = 400.f;
	MinTargetBoomLength = 0.f;

	// Default OxygenMax
	OxygenMax = 100.f;

	// Default Objective
	CurrentObjective = "";

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Water
	bInWater = false;

	// For wall running
	bOnWall = false;

	// For ledge climbing
	bHanging = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Default POV condition
	isTP = true;
	isFP = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectNCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AProjectNCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AProjectNCharacter::StopJumping);

	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AProjectNCharacter::SwitchCameraPOV);

	// Crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AProjectNCharacter::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AProjectNCharacter::StopCrouching);

	// Switch on/off the flashlight
	PlayerInputComponent->BindAction("SwitchFlashlight", IE_Pressed, this, &AProjectNCharacter::SwitchFlashlight);

	// Swim up and dive while in water
	PlayerInputComponent->BindAxis("FloatUp", this, &AProjectNCharacter::FloatUp);
	PlayerInputComponent->BindAxis("Dive", this, &AProjectNCharacter::Dive);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectNCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectNCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AProjectNCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AProjectNCharacter::LookUpAtRate);
}

void AProjectNCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap events to BoxHead's 
	BoxHead->OnComponentBeginOverlap.AddDynamic(this, &AProjectNCharacter::OnOverlapBegin);
	BoxHead->OnComponentEndOverlap.AddDynamic(this, &AProjectNCharacter::OnOverlapEnd);

	Oxygen = OxygenMax;

	// Default value of interaction trace length
	TraceLength = 350.f; 

	SwitchCameraPOV();
}

void AProjectNCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Set bSubmerged to true once BoxHead overlaps a water volume
void AProjectNCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APhysicsVolume* WaterVolume = Cast<APhysicsVolume>(OtherActor);

	if (OtherActor && OtherActor != this && OtherActor == WaterVolume) // Check whether other actor is water volume and not self
	{
		if (WaterVolume->bWaterVolume) // Assure that this physics volume is water volume
		{
			bSubmerged = true;
			bInWater = true;

			// Substract oxygen every second
			GetWorldTimerManager().SetTimer(SubstractOxygenTimer, this, &AProjectNCharacter::SubstractOxygen, 1.f, true, 1.f);

			// Stop AddOxygen timer if it's active
			if (GetWorldTimerManager().IsTimerActive(AddOxygenTimer))
			{
				GetWorldTimerManager().ClearTimer(AddOxygenTimer);
			}

			OnSubmerged(); // Call blueprint event
		}
	}
}

// Set bSubmerged to false once BoxHead overlaps a water volume
void AProjectNCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APhysicsVolume* WaterVolume = Cast<APhysicsVolume>(OtherActor);

	if (OtherActor && (OtherActor != this) && (OtherActor == WaterVolume)) // Check whether other actor is water volume and not self
	{
		if (WaterVolume->bWaterVolume) // Assure that this physics volume is water volume
		{
			bSubmerged = false;

			// Stop SubstractOxygen timer if it's active
			if (GetWorldTimerManager().IsTimerActive(SubstractOxygenTimer))
			{
				GetWorldTimerManager().ClearTimer(SubstractOxygenTimer);
			}

			// Add oxygen every second
			GetWorldTimerManager().SetTimer(AddOxygenTimer, this, &AProjectNCharacter::AddOxygen, 1.f, true, 1.f);

			// Stop adding oxygen once Oxygen greater than or equal to OxygenMax (100.f by default)
			if ((Oxygen >= OxygenMax) && (GetWorldTimerManager().IsTimerActive(AddOxygenTimer)))
			{
				GetWorldTimerManager().ClearTimer(AddOxygenTimer);
			}

			OnEmerged(); // Call blueprint event
		}
	}
}


void AProjectNCharacter::SwitchCameraPOV()
{

	if (isTP) // to FirstPerson
	{
		FVector NewBoomLocation = FVector(-14.f, 30.f, 0.f);

		CameraBoom->TargetArmLength = MinTargetBoomLength;
		CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "head");
		CameraBoom->SetRelativeLocation(NewBoomLocation);
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);

		//FollowCamera->SetRelativeLocation(NewBoomLocation); 

		bUseControllerRotationYaw = true;

		TraceLength = 350.f;

		isFP = true;
		isTP = false;
	}
	else if (isFP) // to ThirdPerson
	{
		CameraBoom->TargetArmLength = MaxTargetBoomLength;
		CameraBoom->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 65.f));
		CameraBoom->SocketOffset = FVector(0.f, 80.f, 0.f);

		bUseControllerRotationYaw = false;

		TraceLength = 600.f;

		isFP = false;
		isTP = true;
	}
}

void AProjectNCharacter::Crouch()
{
	if (!bInWater)
	{
		ACharacter::Crouch();
		bCrouching = true;
	}
}

void AProjectNCharacter::StopCrouching()
{
	ACharacter::UnCrouch();
	bCrouching = false;
}

void AProjectNCharacter::Jump()
{
	if (!bHanging) 
	{
		if (!bInWater) 
		{
			ACharacter::Jump();
		}
	}
}

void AProjectNCharacter::StopJumping()
{
	ACharacter::StopJumping();
}

void AProjectNCharacter::FloatUp(float Value)
{
	if (bInWater && Value != 0.f)
	{
		// Set a diagonal-alike vector to move along
		FVector NewDirectionVector = FVector(FollowCamera->GetForwardVector().X, FollowCamera->GetForwardVector().Y, 1.f); 

		AddMovementInput(NewDirectionVector, Value);
	}
}

void AProjectNCharacter::Dive(float Value)
{
	if (bInWater && Value != 0.f)
	{
		// Set a diagonal-alike vector to move along
		FVector NewDirectionVector = FVector(FollowCamera->GetForwardVector().X, FollowCamera->GetForwardVector().Y, -1.f);

		AddMovementInput(NewDirectionVector, Value);
	}
}

void AProjectNCharacter::AddOxygen()
{
	if (!bSubmerged && (Oxygen < OxygenMax))
	{
		Oxygen = FMath::Clamp(Oxygen + 25.f, 0.f, 100.f);

		/*DEBUGMESSAGE("%f", Oxygen);*/

		OnOxygenAdded();
	}
}

void AProjectNCharacter::SubstractOxygen()
{
	if (bSubmerged && (Oxygen >= 0))
	{
		Oxygen = FMath::Clamp(Oxygen - 3.f, 0.f, 100.f);

		/*DEBUGMESSAGE("%f", Oxygen);*/

		OnOxygenSubstracted();

		if (Oxygen <= 0.f) // TODO: Apply damage every sec once oxygen <= 0
		{
			Death();
		}
	}
}

void AProjectNCharacter::Death()
{
	if (!HealthComp->bDead)
	{
		HealthComp->bDead = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetSimulatePhysics(true);
		DetachFromControllerPendingDestroy();

		FTimerHandle SnapshotTimer;
		GetWorldTimerManager().SetTimer(SnapshotTimer, this, &AProjectNCharacter::RagdollSnapshot, 3.f, false);
	}
}

void AProjectNCharacter::RagdollSnapshot()
{
	GetMesh()->GetAnimInstance()->SavePoseSnapshot("Ragdoll");
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DEBUGMESSAGE("Yo Ragdoll snapshot function is called, wassup?");
}

void AProjectNCharacter::SwitchFlashlight()
{
	if (bFlashlightTurnedOn)
	{
		Flashlight->SetVisibility(false);
		bFlashlightTurnedOn = false;

		if (FlashlightSound)
		{
			UGameplayStatics::SpawnSound2D(this, FlashlightSound, 0.7f);
		}
	}
	else
	{
		Flashlight->SetVisibility(true);
		bFlashlightTurnedOn = true;

		if (FlashlightSound)
		{
			UGameplayStatics::SpawnSound2D(this, FlashlightSound, 0.7f);
		}
	}
}

void AProjectNCharacter::SetJumpCurrentCount(int NewCurrentJumpCount)
{
	JumpCurrentCount = NewCurrentJumpCount;
}

void AProjectNCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AProjectNCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AProjectNCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bOnWall)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AProjectNCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bOnWall)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
