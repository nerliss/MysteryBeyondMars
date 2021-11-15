// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/NHealthComponent.h"

// Sets default values for this component's properties
UNHealthComponent::UNHealthComponent()
{
	MaxHealth = 100.f;
	bDead = false;


}


// Called when the game starts
void UNHealthComponent::BeginPlay()
{
	Super::BeginPlay();


	CurrentHealth = MaxHealth;
	
}
