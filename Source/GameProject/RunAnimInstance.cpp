// Fill out your copyright notice in the Description page of Project Settings.

#include "RunAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Initialize animation properties
void URunAnimInstance::NativeInitializeAnimation()
{
    // Check if Pawn is not already set
    if (Pawn == nullptr)
    {
        // Try to get the owning Pawn
        Pawn = TryGetPawnOwner();
    }

    // If Pawn is valid, get its movement component
    if (IsValid(Pawn))
    {
        MovementComponent = Pawn->GetMovementComponent();
    }
}

// Update animation properties every frame
void URunAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    // Ensure Pawn and MovementComponent are valid
    if (Pawn && MovementComponent)
    {
        // Check if the Pawn is in the air
        bIsInAir = MovementComponent->IsFalling();
        // Get the current speed of the Pawn
        Speed = Pawn->GetVelocity().Size();
    }
}
