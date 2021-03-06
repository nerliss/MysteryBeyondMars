// © 2021. Destruction Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NAI_Base.generated.h"

class UNHealthComponent;

UCLASS()
class PROJECTN_API ANAI_Base : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANAI_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNHealthComponent* HealthComp;

};
