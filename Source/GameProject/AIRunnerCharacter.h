// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"  // Include Box Component
#include "AIRunnerCharacter.generated.h"

UCLASS()
class GAMEPROJECT_API AAIRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIRunnerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class APlayerStart* PlayerStart;

public:
	virtual void Tick(float DeltaTime) override;

	// Function to move AI forward
	void MoveForward();

	// Function to handle obstacle avoidance
	void AvoidObstacles();

	// Function to make the AI jump
	void JumpOverObstacle();

	// Function to move AI sideways
	void DodgeObstacle();

	FTimerHandle DodgeTimerHandle;
	FTimerHandle JumpCooldownHandle;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float MoveSpeed;
	bool bIsDodging;
	bool bCanJump;


	//UPROPERTY(VisibleDefaultsOnly)
	//class ARunnerGameMode* RunGameMode;

};
