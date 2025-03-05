#include "Components/ArrowComponent.h"

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"


class UStaticMeshComponent;
class USceneComponent;
class UBoxComponent;
class UArrowComponent;
class AObstacle;
class ACoinItem;

UCLASS()
class GAMEPROJECT_API AFloorTile : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<AObstacle> SmallObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<AObstacle> BigObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<ACoinItem> CoinItemClass;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//TArray<AActor*> ChildActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<AActor> CubeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FloorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* AttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* CenterLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* LeftLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* FloorTriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AObstacle*> ChildActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float SpawnPercent1 = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float SpawnPercent2 = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float SpawnPercent3 = 0.5f;

	UFUNCTION(BlueprintCallable)
	void SpawnItems();

	UFUNCTION(BlueprintCallable)
	void DestroyFloorTile();

public:	
	// Sets default values for this actor's properties
	AFloorTile();

	FORCEINLINE const FTransform& GetAttachTransform() const
	{
		return AttachPoint->GetComponentTransform();
	}

protected:
	// Called when the game starts or when spawned

protected:
	UPROPERTY(VisibleInstanceOnly)
	class ARunnerGameMode* RunGameMode;

	UFUNCTION()
	void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SpawnLaneItem(UArrowComponent* Lane, int32& NumBigs);

	UPROPERTY()
	FTimerHandle DestroyHandle;

	virtual void BeginPlay() override;



//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
