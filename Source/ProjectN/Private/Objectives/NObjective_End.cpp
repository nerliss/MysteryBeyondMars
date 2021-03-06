// © 2021. Destruction Games


#include "Objectives/NObjective_End.h"
#include "ProjectN/ProjectNCharacter.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

ANObjective_End::ANObjective_End()
{
	ArrowComponent->SetArrowColor(FColor::Yellow);
	ArrowComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
}

void ANObjective_End::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Cast to player
	AProjectNCharacter* Character = Cast<AProjectNCharacter>(OtherActor);

	if (OtherActor && OtherActor == Character) 
	{
		// Check whether player's current objective matches required objective and is not empty
		if (Character->CurrentObjective == Objective && (!Character->CurrentObjective.IsEmpty()))
		{
			// Append completion status to objective
			Character->CurrentObjective = "Completed: " + Objective; 

			// Play sound if chosen
			if (ObjectiveUpdateSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ObjectiveUpdateSound);
			}
		}
		else return;
	}
	else return;
}
