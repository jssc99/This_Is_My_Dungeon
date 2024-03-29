#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

UCLASS()
class THISISMYDUNGEON_API ATrap : public AActor
{
	GENERATED_BODY()

public:
	ATrap();

	int GetCost() const;
	FUintVector2 GetTileSize() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Trap")
	FUintVector2 tileSize = { 1,1 };

	UPROPERTY(EditAnywhere, Category = "Trap")
	ATrap* upgradedTrap;

	UPROPERTY(EditAnywhere, Category = "Trap")
	int	cost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	float damage = 1.f;

	UPROPERTY(EditAnywhere, Category = "Trap")
	bool canDamageEnemy = true;

	UPROPERTY(EditAnywhere, Category = "Trap")
	UTexture2D* trapImage = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Trap")
	UTexture2D* GetTrapImage(); //Used for the ui
};
