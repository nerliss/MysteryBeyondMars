// © 2021. Destruction Games

#pragma once

#include "CoreMinimal.h"
#include "Objectives/NObjective.h"
#include "NObjective_End.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTN_API ANObjective_End : public ANObjective
{
	GENERATED_BODY()
	

public:

	ANObjective_End();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
