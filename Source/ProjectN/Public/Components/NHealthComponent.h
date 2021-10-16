// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTN_API UNHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool isDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float MaxHealth;
		
};
