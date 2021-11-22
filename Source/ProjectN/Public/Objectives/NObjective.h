// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NObjective.generated.h"

class UBoxComponent;
class UArrowComponent;
class USoundBase;

UCLASS()
class PROJECTN_API ANObjective : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANObjective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxCollision;

	/* For testing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USoundBase* ObjectiveUpdateSound;


public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FString Objective;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Will be used for updating player's objective
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
