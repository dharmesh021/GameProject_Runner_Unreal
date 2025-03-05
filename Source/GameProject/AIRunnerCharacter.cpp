// Fill out your copyright notice in the Description page of Project Settings.


#include "AIRunnerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "RunnerGameMode.h"




// Sets default values
// Sets default values
AAIRunnerCharacter::AAIRunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    MoveSpeed = 2000.0f; // Adjust as needed
    bIsDodging = false;

  
}
    

// Called when the game starts or when spawned
void AAIRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->JumpZVelocity = 600.0f; // Adjust to suit your game

    /*RunGameMode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    check(RunGameMode);*/

    PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
       
}


// Called every frame
void AAIRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveForward();
	AvoidObstacles();

}

void AAIRunnerCharacter::MoveForward()
{
    // Move forward constantly, use MoveSpeed to scale the speed
    AddMovementInput(GetActorForwardVector(), 1.0f);

    // Set the character movement speed
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}


void AAIRunnerCharacter::AvoidObstacles()
{
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * 300.0f); // Look ahead 300 units

    FHitResult Hit;
    FCollisionQueryParams CollisionParams;

    // Check for obstacles ahead
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
    {
        if (Hit.GetActor())
        {   
            float DistanceToObstacle = (Hit.ImpactPoint - GetActorLocation()).Size();

            // If the obstacle is close, decide to dodge 75% of the time
            if (DistanceToObstacle < 100.0f)
            {
                float RandomChoice = FMath::RandRange(0.0f, 5.0f); // Randomly decide action

                if (RandomChoice < 0.10f) // 75% chance to dodge
                {
                    DodgeObstacle();
                }
                else // 25% chance to jump
                {
                    JumpOverObstacle();
                }
            }
            else
            {
                // If obstacle is farther, prioritize jumping
                JumpOverObstacle();
            }
        }
    }
}


void AAIRunnerCharacter::JumpOverObstacle()
{
    if (GetCharacterMovement()->IsMovingOnGround()) // Check if AI is grounded before jumping
    {
        ACharacter::Jump();
    }
}

void AAIRunnerCharacter::DodgeObstacle()
{
    if (!bIsDodging) // Prevent continuous dodging
    {
        bIsDodging = true;

        FVector RightVector = GetActorRightVector();
        float DodgeDirection = (FMath::RandBool() ? 1.0f : -1.0f); // Randomly dodge left or right

        // Apply dodge movement input
        AddMovementInput(RightVector, DodgeDirection);

        // Set the max walk speed to MoveSpeed while dodging
        GetCharacterMovement()->MaxWalkSpeed = MoveSpeed * 1.5f; // Optionally, increase speed during dodge

        // Set a timer to reset dodge state and walk speed after 0.5 seconds
        GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, [this]() {
            bIsDodging = false;
            // Reset walk speed back to normal after dodge
            GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
            }, 0.5f, false); // 0.5 seconds dodge duration
    }
}




// Called to bind functionality to input
void AAIRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



