// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunCharacter.generated.h"

UCLASS()
class GAMEPROJECT_API ARunCharacter : public ACharacter
{
	GENERATED_BODY()


private:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly)
	class ARunnerGameMode* RunGameMode;




public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float MovementSpeed = 500.0f; // Default Speed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float SpeedIncreaseAmount = 300.0f; // Increase amount per 10 coins

	void IncreaseSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	class UParticleSystem* DeathParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	class USoundBase* DeathSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	int32 CurrentLane = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	int32 NextLane = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveDownImpulse = -1000.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Lane")
	void ChangeLane();

	UFUNCTION(BlueprintCallable, Category = "Lane")
	void ChangeLaneUpdate(float Value);

	UFUNCTION(BlueprintCallable, Category = "Lane")
	void ChangeLaneFinished();

	UFUNCTION(BlueprintCallable)
	void Death();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SpawnActor();	

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> actorBPtoSpawn;



	// Sets default values for this character's properties
	ARunCharacter();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void MoveLeft();

	UFUNCTION()
	void MoveRight();

	UFUNCTION()
	void MoveDown();

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void ResetLevel();

	UPROPERTY()
	FTimerHandle DeathHandle;

	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY()
	class APlayerStart* PlayerStart;





public:	
	UFUNCTION()
	void AddCoin();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
