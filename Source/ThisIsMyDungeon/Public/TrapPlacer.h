// Sue Me.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "PreviewTrap.h"
#include "Trap.h"

#include "TrapPlacer.generated.h"

UCLASS()
class THISISMYDUNGEON_API ATrapPlacer : public ACharacter
{
	GENERATED_BODY()
	
public:	
	ATrapPlacer();
	virtual void Tick(float DeltaTime) override;
	//----------------------------------------------------------------------------
	
	UPROPERTY(VisibleAnywhere)
	FIntVector2 posTile = { 0,0 };

	UPROPERTY(EditAnywhere, Category = "TrapPlacer", meta = (AllowPrivateAcess = "true", ClampMin = "1"))
	int tileSize = 16;
	
	UPROPERTY(EditAnywhere, Category = "TrapPlacer", meta = (AllowPrivateAcess = "true"))
	TSubclassOf<APreviewTrap> previewTileActor;

	UPROPERTY(EditAnywhere, Category = "TrapPlacer", meta = (AllowPrivateAcess = "true"))
	TArray<TSubclassOf<ATrap>> trapsToPlace;

	UPROPERTY(EditAnywhere, Category = "TrapPlacer", meta = (AllowPrivateAcess = "true"))
	float floorCoord = 0;

	UFUNCTION(BlueprintCallable, Category = "TrapPlacer")
	void PlaceTrap();

	UFUNCTION(BlueprintCallable, Category = "TrapPlacer")
	void ToPlayer();

	UFUNCTION(BlueprintCallable, Category = "TrapPlacer")
	ACharacter* GetStaticPlayer();//Used for the ui

	UFUNCTION(BlueprintCallable, Category = "TrapPlacer")
	void NextTrap();

	UFUNCTION(BlueprintCallable, Category = "TrapPlacer")
	void PrevTrap();

	UFUNCTION(BlueprintCallable, Category = "TrapPlacer")
	ATrap* GetCurrentTrap();

	//----------------------------------------------------------------------------

	TArray<FIntVector2> usedTiles;
	bool isEmpty = false;
	
	void OpenPlacer();
	void ClosePlacer();

	static ATrapPlacer* GetInstance();

protected:
	bool IsTileEmpty();

	UPROPERTY(VisibleAnywhere)
	FIntVector2 pPreTilePos = { 0,0 };

	APreviewTrap* previewTile;

	virtual void BeginPlay() override;

	static ATrapPlacer* instance;

	unsigned char index = 0; //use a unsigned char because small amount
};
