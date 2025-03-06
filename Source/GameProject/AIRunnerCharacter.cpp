// Fill out your copyright notice in the Description page of Project Settings.

#include "AIRunnerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "RunnerGameMode.h"

// Sets default values
AAIRunnerCharacter::AAIRunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = true; // Enable ticking every frame
    MoveSpeed = 2000.0f; // Set default movement speed
    bIsDodging = false; // Initialize dodging state
    bCanJump = true; // Initialize jump cooldown flag
}

// Called when the game starts or when spawned
void AAIRunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Set character movement properties
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->JumpZVelocity = 700.0f;

    // Find the player start actor in the world
    PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
}

// Called every frame
void AAIRunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveForward(); // Move the character forward
    AvoidObstacles(); // Check and avoid obstacles
}

// Move the character forward constantly
void AAIRunnerCharacter::MoveForward()
{
    AddMovementInput(GetActorForwardVector(), 2.0f); // Add forward movement input
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed; // Ensure the movement speed is set
}

// Check for obstacles and decide to jump or dodge
void AAIRunnerCharacter::AvoidObstacles()
{
    FVector Start = GetActorLocation(); // Start location for line trace
    FVector ForwardVector = GetActorForwardVector(); // Forward direction
    FVector End = Start + (ForwardVector * 50.0f); // End location for line trace

    FHitResult Hit;
    FCollisionQueryParams CollisionParams;

    // Perform a line trace to detect obstacles
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
    {
        if (Hit.GetActor())
        {
            float DistanceToObstacle = (Hit.ImpactPoint - GetActorLocation()).Size(); // Calculate distance to obstacle

            // Jump or dodge only if the obstacle is close
            if (DistanceToObstacle < 50.0f)
            {
                float RandomChoice = FMath::RandRange(0.1f, 2.0f); // Random choice between jump and dodge

                if (RandomChoice < 0.50f) // 50% chance to dodge
                {
                    DodgeObstacle();
                }
                else // 50% chance to jump
                {
                    JumpOverObstacle();
                }
            }
        }
    }
}

// Jump over the detected obstacle
void AAIRunnerCharacter::JumpOverObstacle()
{
    if (bCanJump && GetCharacterMovement()->IsMovingOnGround())
    {
        ACharacter::Jump(); // Perform jump
        bCanJump = false; // Prevent multiple jumps

        // Reset jump ability after 1 second
        GetWorld()->GetTimerManager().SetTimer(JumpCooldownHandle, [this]() {
            bCanJump = true;
            }, 1.0f, false);
    }
}

// Dodge the detected obstacle
void AAIRunnerCharacter::DodgeObstacle()
{
    if (!bIsDodging)
    {
        bIsDodging = true; // Set dodging state

        FVector RightVector = GetActorRightVector(); // Get right direction
        float DodgeDirection = (FMath::RandBool() ? 1.0f : -1.0f); // Random left or right dodge

        AddMovementInput(RightVector, DodgeDirection); // Add dodge movement input

        // Increase speed slightly while dodging
        GetCharacterMovement()->MaxWalkSpeed = MoveSpeed * 1.2f;

        // Reset dodge state after 0.5 seconds
        GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, [this]() {
            bIsDodging = false;
            GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
            }, 0.5f, false);
    }
}

// Called to bind functionality to input
void AAIRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
