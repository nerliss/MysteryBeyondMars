// Fill out your copyright notice in the Description page of Project Settings.


#include "Objectives/NObjective.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectN/ProjectNCharacter.h"

// Sets default values
ANObjective::ANObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set up box collision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetBoxExtent(FVector(96.f, 96.f, 96.f));

	// Set up arrow component
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(BoxCollision);
	ArrowComponent->SetRelativeRotation(FRotator(180.f, 90.f, 180.f));
	ArrowComponent->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	ArrowComponent->SetArrowColor(FColor::Purple);
	ArrowComponent->ArrowSize = 3.f;
	ArrowComponent->bHiddenInGame = false; // for test purposes only... just like the whole idea of having an arrow component here
	
	// Set default value of objective
	Objective = "";
}

// Called when the game starts or when spawned
void ANObjective::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind OnOverlapBegin event to BoxCollision
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANObjective::OnOverlapBegin);

}

// Called every frame
void ANObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// On overlap begin event 
void ANObjective::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Cast to player
	AProjectNCharacter* Character = Cast<AProjectNCharacter>(OtherActor);

	if (OtherActor && OtherActor == Character) 
	{
		// Set player's current objective to this actor's
		Character->CurrentObjective = Objective;

		// Play sound if chosen
		if (ObjectiveUpdateSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ObjectiveUpdateSound);
		}
	}
	else
	{
		return;
	}
}
