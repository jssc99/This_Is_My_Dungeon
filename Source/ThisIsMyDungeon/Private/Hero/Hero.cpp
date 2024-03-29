#include "Hero/Hero.h"

#include "Demon/Demon.h"
#include "Dungeon/DungeonManager.h"

#include "Hero/HeroController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include <Kismet/GameplayStatics.h>

AHero::AHero()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AHeroController::StaticClass();

	moveComponent = GetCharacterMovement();
}

void AHero::BeginPlay()
{
	Super::BeginPlay();

	WTM = &GetWorldTimerManager();
	controller = Cast<AHeroController>(GetController());

	moveComponent->MaxWalkSpeed = speed * speedMultiplier;

	weaponScale.SetNum(nbWeaponsMax);

	GetDungeonManager();
	SetWeaponSize();
}

void AHero::GetDungeonManager()
{
	TArray<AActor*> arrayDungeonManager;
	if (UWorld* World = GetWorld())
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADungeonManager::StaticClass(), arrayDungeonManager);
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red,
				TEXT("AHero::GetDungeonManager Coudn't get World for " + GetName()));
		return;
	}

	if (arrayDungeonManager.Num() != 1)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red,
				TEXT("AHero::GetDungeonManager Coudn't get THE DungeonManager for " + GetName()));
		return;
	}

	dManager = Cast<ADungeonManager>(arrayDungeonManager[0]);
}

void AHero::DemonDetected(ADemon* demon)
{
	if (!controller->IsDemonInSight(demon)) // Check if demon in sight to avoid detection through walls
		return;

	demonInRange = demon;
	if (demonInRange == nullptr)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Orange,
				TEXT("AHero::DemonDetected Called but no Demon at hand for " + GetName()));
		return;
	}

	controller->DemonDetected(demonInRange);
	WTM->SetTimer(attackHandle, this, &AHero::AttackDemon, timerAttackMoment);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green,
			TEXT("AHero::DemonDetected Called success for " + GetName()));
}

void AHero::DemonLost()
{
	if (demonInRange == nullptr)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Orange,
				TEXT("AHero::DemonLost Called but no Demon at hand for " + GetName()));
		return;
	}

	WTM->ClearTimer(attackHandle);
	controller->DemonLost();
	demonInRange = nullptr;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green,
			TEXT("AHero::DemonLost Called success for " + GetName()));
}

void AHero::GetDamaged(float value)
{
	Super::GetDamaged(value);

	if (GetLifeSpan() != 0.f) // Already dead
		return;

	if (!IsAlive())
		Death();
	else
	{
		if (WTM->TimerExists(damageAnimHandle))
		{
			GetMesh()->SetVisibility(false);
			blinkingCpt = 0;
			WTM->ClearTimer(damageAnimHandle);
		}
		// Blinking when damaged
		WTM->SetTimer(damageAnimHandle, this, &AHero::DamageBlinking, blinkingSpeed, true);
	}
}

void AHero::SetStartWaypoint(AWaypoint* startWaypoint)
{
	if (!controller)
	{
		controller = Cast<AHeroController>(GetController());
	}
	if (!startWaypoint)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red,
				TEXT("AHero::SetStartWaypoint Called but no Waypoint was sent for " + GetName()));
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red,
				TEXT("AHero::SetStartWaypoint Death for " + GetName()));
		}
		SetLifeSpan(.5f);
		return;
	}
	controller->SetStartWaypoint(startWaypoint);
}

void AHero::Death()
{
	SetLifeSpan(lifeSpanOnDeath);

	moveComponent->StopActiveMovement();
	moveComponent->Deactivate();

	controller->StopMovement();
	controller->Destroy();

	GetMesh()->SetVisibility(true);
	if (WTM->TimerExists(damageAnimHandle))
		WTM->ClearTimer(damageAnimHandle);
	if (WTM->TimerExists(lootHandle))
		WTM->ClearTimer(lootHandle);

	SetActorEnableCollision(false);

	dManager->treasureLife += treasureDrop;
	ADemon::GetInstance()->mana += manaDrop;
}

void AHero::StartLooting()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue,
			TEXT("AHero::StartLooting Called for " + GetName()));

	GetWorldTimerManager().SetTimer(lootHandle, this, &AHero::LootTreasure, lootTimer, false);
}

void AHero::LootTreasure()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue,
			TEXT("AHero::LootTreasure Is Looting " + GetName()));

	dManager->DamageTreasure(damage);
	treasureDrop += damage;
}

void AHero::DamageBlinking()
{
	blinkingCpt++;
	if (blinkingCpt >= blinkingAmount)
	{
		GetMesh()->SetVisibility(true);
		blinkingCpt = 0;
		WTM->ClearTimer(damageAnimHandle);

		return;
	}
	else
		GetMesh()->SetVisibility(!GetMesh()->IsVisible());
}

void AHero::Upgrade(int nbUpgrades)
{
	if (upgradeLevel >= weaponOnUpgrade.Num())
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Orange,
				TEXT("AHero::Upgrade Called but already at max lvl possible for " + GetName()));
		return;
	}

	if ((upgradeLevel + nbUpgrades) >= weaponOnUpgrade.Num())
	{
		nbUpgrades = weaponOnUpgrade.Num() - upgradeLevel - 1;
		if (nbUpgrades <= 0)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Orange,
					TEXT("AHero::Upgrade Called but no upgrade possible for " + GetName()));
			return;
		}
	}

	upgradeLevel += nbUpgrades;
	treasureDrop += treasureUpgrade * nbUpgrades;
	manaDrop += manaUpgrade * nbUpgrades;
	health += healthUpgrade * nbUpgrades;
	speed += speedUpgrade * nbUpgrades;
	damage += damageUpgrade * nbUpgrades;

	moveComponent->MaxWalkSpeed = speed * speedMultiplier;

	SetWeaponSize();
}

void AHero::SetWeaponSize()
{
	for (int i = 0; i < nbWeaponsMax; i++)
		weaponScale[i] = FVector((double)weaponOnUpgrade[upgradeLevel].bOnDisplay[i]);
	bUpgradeDone = true;
}

void AHero::AttackDemon()
{
	if (!demonInRange)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Orange,
				TEXT("AHero::AttackDemon Called but no demon in range anymore " + GetName()));
		return;
	}

	DoAttack(demonInRange);

	WTM->SetTimer(attackHandle, this, &AHero::AttackDemon, attackSequence->GetPlayLength());
}

bool AHero::IsAttacking()
{
	return WTM->TimerExists(attackHandle);
}

void AHero::OnDeath_Implementation() { /* For BP Use */ }