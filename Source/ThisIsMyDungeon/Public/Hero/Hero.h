#pragma once

#include "CoreMinimal.h"
#include "MyEntity.h"
#include "Hero.generated.h"

/*
*	Hero is a generic class that get declined through blueprints
*	The upgrade values are static, and the stats increase linearly
*	Visuals can change when ganing an upgrade lvl, max upgrade lvl exists
*	Blinks when hurt
*/

USTRUCT(BlueprintType)
struct FUpgradeWeapon
{
	GENERATED_BODY()

public:
	// Index is the weapon order in the Animation Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Weapon x Is On Display"))
	TArray<bool> bIsOnDisplay;
};

/*
	/!\ this IS the ENEMY /!\
*/

UCLASS()
class THISISMYDUNGEON_API AHero : public AMyEntity
{
	GENERATED_BODY()

	/*
		VARIABLES
	*/
public:
	/*
		Death
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|OnDeath", meta = (DisplayName = "Treasure Droped"))
	float treasureDrop = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|OnDeath")
	float lifeSpanOnDeath = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|OnDeath")
	float blinkingSpeed = .5f;

	/*
		Upgrade stats
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|Upgrade", meta = (DisplayName = "Treasure Droped Increase"))
	float treasureUpgrade = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|Upgrade", meta = (DisplayName = "Damage Increase"))
	float damageUpgrade = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|Upgrade", meta = (DisplayName = "Health Increase"))
	float healthUpgrade = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|Upgrade", meta = (DisplayName = "Speed Increase"))
	float speedUpgrade = 0.f;

	/*
		Attack
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values", meta = (DisplayName = "Attack Moment (sync with AB)"))
	float timerAttackMoment = .4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values", meta = (DisplayName = "Attack Sequence (sync with AB)"))
	UAnimSequence* attackSequence = nullptr;

protected:
	/*
		More Upgrade (Weapon)
	*/
	// Index is the upgrade level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values|Upgrade")
	TArray<FUpgradeWeapon> weaponOnUpgrade; // for Weapon Scales in Animation Blueprint

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CPP Values")
	int nbWeaponsMax = 0; // for Weapon Scale

private:
	/*
		Components
	*/
	UCharacterMovementComponent* moveComponent = nullptr;

	class AHeroController* controller = nullptr;

	// World Timer Manager ptr
	FTimerManager* WTM = nullptr;

	/*
		Attack
	*/
	// If demon in range is nullptr (or None) -> no demon to attack is in range
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CPP Values|Debug", meta = (AllowPrivateAccess = "true"))
	class ADemon* demonInRange = nullptr;

	FTimerHandle attackHandle;

	/*
		Even more Upgrade
	*/
	// Cannot upgrade beyond weaponOnUpgrade size
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CPP Values|Upgrade", meta = (DisplayName = "Current Upgrade Level", AllowPrivateAccess = "true"))
	int upgradeLevel = 0;

	// Used for Animation Blueprint
	UPROPERTY(BlueprintReadWrite, Category = "CPP Values|Debug", meta = (AllowPrivateAccess = "true"))
	bool bUpgradeDone = false; // for Animation Blueprint to know to change its current values

	// Used for Animation Blueprint
	UPROPERTY(BlueprintReadOnly, Category = "CPP Values|Debug", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> weaponScale; // for Weapon Scales in the Animation Blueprint to change its values

	/*
		FUNCTIONS
	*/
public: // Constructor(s)
	AHero();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override; // TODO delete?

	UFUNCTION(BlueprintCallable, Category = "Functions")
	virtual void DemonDetected(class ADemon* demon);

	UFUNCTION(BlueprintCallable, Category = "Functions")
	virtual void DemonLost();

private:
	virtual void DamageBlinking();
	virtual void SetWeaponSize();
	virtual void AttackDemon();

public:
	UFUNCTION(BlueprintCallable, Category = "Functions")
	virtual void Upgrade(int nbUpgrades = 1);

	UFUNCTION(BlueprintCallable, Category = "Functions")
	virtual bool IsMoving();

	UFUNCTION(BlueprintCallable, Category = "Functions")
	virtual bool IsAttacking();

	// Already blueprint callable
	virtual void GetDamaged(float value) override;

	// Used by HeroSpawner
	virtual void SetStartWaypoint(class AWaypoint* startWaypoint);

	UFUNCTION(BlueprintCallable, Category = "Functions")
	virtual void Death();
};