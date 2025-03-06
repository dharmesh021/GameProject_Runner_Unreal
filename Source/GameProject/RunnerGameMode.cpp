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

// Called when the game starts or when spawned
void ARunnerGameMode::BeginPlay()
{
    // Show the mouse cursor
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

    // Create and initialize the game HUD
    GameHUD = Cast<UGameHUD>(CreateWidget(GetWorld(), GameHudClass));
    check(GameHUD);
    GameHUD->InitializeHUD(this);
    GameHUD->AddToViewport();

    // Set the number of lives to the maximum
    NumberOfLives = MaxLives;

    // Create the initial floor tiles
    CreateInitialFloorTiles();

    // Log a message indicating that the AI Runner is being spawned
    UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay: Attempting to spawn AI Runner."));

    // Set AI Spawn Location
    FVector SpawnLocation = FVector(0.f, 0.f, 92.f); // Adjust Y to place it next to the player
    FRotator SpawnRotation = FRotator::ZeroRotator;

    // Spawn AI
    GetWorld()->SpawnActor<AAIRunnerCharacter>(AAIRunnerCharacter::StaticClass(), SpawnLocation, SpawnRotation);
}

// Create the initial set of floor tiles
void ARunnerGameMode::CreateInitialFloorTiles()
{
    // Add the first floor tile without spawning items
    AFloorTile* Tile = AddFloorTile(false);

    // If the tile was successfully added, store the lane switch values
    if (Tile) 
    {
        LaneSwitchValues.Add(Tile->LeftLane->GetComponentLocation().Y);
        LaneSwitchValues.Add(Tile->CenterLane->GetComponentLocation().Y);
        LaneSwitchValues.Add(Tile->RightLane->GetComponentLocation().Y);
    }

    // Add two more floor tiles without spawning items
    AddFloorTile(false);
    AddFloorTile(false);

    // Add the remaining initial floor tiles with item spawning
    for (int i = 0; i < NumInitialFloorTiles; i++)
    {
        AddFloorTile(true);
    }
}

// Add a floor tile to the game
AFloorTile* ARunnerGameMode::AddFloorTile(const bool bSpawnItems)
{
    UWorld* World = GetWorld();

    if (World)
    {
        // Spawn a new floor tile at the next spawn point
        AFloorTile* Tile = World->SpawnActor<AFloorTile>(FloorTileClass, NextSpawnPoint);

        if (Tile)
        {
            // Add the tile to the list of floor tiles
            FloorTiles.Add(Tile);

            // If items should be spawned, call the SpawnItems method on the tile
            if (bSpawnItems)
            {
                Tile->SpawnItems();
            }

            // Update the next spawn point to the end of the newly added tile
            NextSpawnPoint = Tile->GetAttachTransform();
        }
        return Tile;
    }
    return nullptr;
}

// Add a coin to the player's total and handle related events
void ARunnerGameMode::AddCoin()
{
    TotalCoins += 1;
    OnCoinsCountChanged.Broadcast(TotalCoins);

    // Check if the total coins collected is a multiple of 10
    if (TotalCoins % 12 == 0)
    {
        // Increase the player's speed
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

// Handle the player's death
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

// Handle the game over state
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

// Remove a floor tile from the game
void ARunnerGameMode::RemoveTile(AFloorTile* Tile)
{
    FloorTiles.Remove(Tile);
}
