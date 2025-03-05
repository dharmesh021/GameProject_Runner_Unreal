// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameMode.h"
#include "FloorTile.h"
#include "Obstacle.h"
#include "RunCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "GameHUD.h"
#include "AIRunnerCharacter.h"


void ARunnerGameMode::BeginPlay()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	GameHUD = Cast<UGameHUD>(CreateWidget(GetWorld(), GameHudClass));
	check(GameHUD);
	GameHUD->InitializeHUD(this);
	GameHUD->AddToViewport();


	NumberOfLives = MaxLives;

	

	CreateInitialFloorTiles();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay: Attempting to spawn AI Runner."));
	// Set AI Spawn Location
	FVector SpawnLocation = FVector(0.f, 0.f, 92.f); // Adjust Y to place it next to the player
	FRotator SpawnRotation = FRotator::ZeroRotator;

	// Spawn AI
	GetWorld()->SpawnActor<AAIRunnerCharacter>(AAIRunnerCharacter::StaticClass(), SpawnLocation, SpawnRotation);

}


void ARunnerGameMode::CreateInitialFloorTiles()
{

	AFloorTile* Tile = AddFloorTile(false);

	if (Tile) 
	{
		LaneSwitchValues.Add(Tile->LeftLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->CenterLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->RightLane->GetComponentLocation().Y);
	}

	AddFloorTile(false);
	AddFloorTile(false);

	for (int i = 0; i < NumInitialFloorTiles; i++)
	{
		AddFloorTile(true);
	}
}

AFloorTile* ARunnerGameMode::AddFloorTile(const bool bSpawnItems)
{
	UWorld* World = GetWorld();

	if (World)
	{
		AFloorTile* Tile = World->SpawnActor<AFloorTile>(FloorTileClass, NextSpawnPoint);

		if (Tile)
		{
			FloorTiles.Add(Tile);
			if (bSpawnItems)
			{
				Tile->SpawnItems();
			}
			NextSpawnPoint = Tile->GetAttachTransform();
		}
		return Tile;


	}
	return nullptr;
}

void ARunnerGameMode::AddCoin()
{
    TotalCoins += 1;
    OnCoinsCountChanged.Broadcast(TotalCoins);

    // Check if the total coins collected is a multiple of 2
    if (TotalCoins % 2 == 0)
    {

        ARunCharacter* Player = Cast<ARunCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (Player)
        {
            Player->IncreaseSpeed();
        }

        // Show the "Speed Increasing" widget
		if (IsValid(LevelCompletedClass))
		{
			UUserWidget* Widget = CreateWidget(GetWorld(), LevelCompletedClass);

			if (Widget)
			{
				Widget->AddToViewport();
			}
		}
    }
}




void ARunnerGameMode::PlayerDied()
{
	NumberOfLives -= 1;
	OnLivesCountChanged.Broadcast(NumberOfLives);

	if (NumberOfLives > 0)
	{
		// Iterate all FloorTiles and call DestroyFloorTile
		for (auto Tile : FloorTiles)
		{
			Tile->DestroyFloorTile();
		}

		// Empty our array
		FloorTiles.Empty();

		// set NextSpawnPoint to initial value
		NextSpawnPoint = FTransform();

		// Create out Initial floor tiles
		CreateInitialFloorTiles();

		// Broadcast level reset event
		OnLevelReset.Broadcast();
	}
	else
	{
		GameOver();
	}
}

void ARunnerGameMode::GameOver()
{
	if (IsValid(GameOverScreenClass))
	{
		UUserWidget* Widget = CreateWidget(GetWorld(), GameOverScreenClass);

		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}

void ARunnerGameMode::RemoveTile(AFloorTile* Tile)
{
	FloorTiles.Remove(Tile);
}





