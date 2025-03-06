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
    PrimaryActorTick.bCanEverTick = true;
    MoveSpeed = 2000.0f; // Adjust as needed
    bIsDodging = false;
    bCanJump = true; // Cooldown flag for jumping
}

void AAIRunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->JumpZVelocity = 700.0f;

    PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
}

void AAIRunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveForward();
    AvoidObstacles();
}

void AAIRunnerCharacter::MoveForward()
{
    // Move forward constantly
    AddMovementInput(GetActorForwardVector(), 2.0f);
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void AAIRunnerCharacter::AvoidObstacles()
{
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * 50.0f); // Reduced detection range

    FHitResult Hit;
    FCollisionQueryParams CollisionParams;

   

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
    {
        if (Hit.GetActor())
        {
            float DistanceToObstacle = (Hit.ImpactPoint - GetActorLocation()).Size();

            // Jump or dodge only if the obstacle is close (200 units instead of 500)
            if (DistanceToObstacle < 50.0f)
            {
                float RandomChoice = FMath::RandRange(0.1f, 2.0f);

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

void AAIRunnerCharacter::JumpOverObstacle()
{
    if (bCanJump && GetCharacterMovement()->IsMovingOnGround())
    {
        ACharacter::Jump();
        bCanJump = false; // Prevent multiple jumps

        // Reset jump ability after 1 second
        GetWorld()->GetTimerManager().SetTimer(JumpCooldownHandle, [this]() {
            bCanJump = true;
            }, 1.0f, false);
    }
}

void AAIRunnerCharacter::DodgeObstacle()
{
    if (!bIsDodging)
    {
        bIsDodging = true;

        FVector RightVector = GetActorRightVector();
        float DodgeDirection = (FMath::RandBool() ? 1.0f : -1.0f); // Random left or right dodge

        AddMovementInput(RightVector, DodgeDirection);

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
