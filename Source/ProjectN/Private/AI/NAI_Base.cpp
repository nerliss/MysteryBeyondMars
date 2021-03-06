// © 2021. Destruction Games


#include "AI/NAI_Base.h"
#include "Components/NHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANAI_Base::ANAI_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f; // default is 600
	GetCharacterMovement()->AirControl = 1.f; // default is 0.2

	// Create a health component
	HealthComp = CreateDefaultSubobject<UNHealthComponent>(TEXT("HealthComp"));

}

// Called when the game starts or when spawned
void ANAI_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANAI_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
// void ANAI_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
// 
// }

