	// Fill out your copyright notice in the Description page of Project Settings.


	#include "FloorTile.h"
	#include "RunnerGameMode.h"
	#include "RunCharacter.h"
	#include "Components/ArrowComponent.h"
	#include "Components/BoxComponent.h"
	#include "Kismet/GameplayStatics.h"
	#include "Kismet/KismetMathLibrary.h"
	#include "Obstacle.h"
	#include "CoinItem.h"







	// Sets default values
	AFloorTile::AFloorTile()
	{
 		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = true;

		SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
		RootComponent = SceneComponent;

		FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
		FloorMesh->SetupAttachment(SceneComponent);

		AttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Point"));
		AttachPoint->SetupAttachment(SceneComponent);

		CenterLane = CreateDefaultSubobject<UArrowComponent>(TEXT("Center Lane"));
		CenterLane->SetupAttachment(SceneComponent);

		RightLane = CreateDefaultSubobject<UArrowComponent>(TEXT("Right Lane"));
		RightLane->SetupAttachment(SceneComponent);

		LeftLane = CreateDefaultSubobject<UArrowComponent>(TEXT("Left Lane"));
		LeftLane->SetupAttachment(SceneComponent);

		FloorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorTriggerBox"));
		FloorTriggerBox->SetupAttachment(SceneComponent);
		FloorTriggerBox->SetBoxExtent(FVector(32.f, 500.f, 200.f));
		FloorTriggerBox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	}

	// Called when the game starts or when spawned
	void AFloorTile::BeginPlay()
	{
		Super::BeginPlay();

		RunGameMode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		check(RunGameMode);

		FloorTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::OnTriggerBoxOverlap);
	}

	void AFloorTile::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

		if (RunCharacter)
		{
			RunGameMode->AddFloorTile(true);

	
			GetWorldTimerManager().SetTimer(DestroyHandle, this, &AFloorTile::DestroyFloorTile, 2.f, false);
		}

	}




	void AFloorTile::SpawnItems()
	{
		if (IsValid(SmallObstacleClass) && IsValid(BigObstacleClass) && IsValid(CoinItemClass))
		{
			int32 NumBigs = 0;
			SpawnLaneItem(CenterLane, NumBigs);
			SpawnLaneItem(LeftLane, NumBigs);
			SpawnLaneItem(RightLane, NumBigs);
		}
	
	}
	void AFloorTile::SpawnLaneItem(UArrowComponent* Lane, int32& NumBigs)
	{
		const float RandVal = FMath::FRandRange(0.f, 1.f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const FTransform SpawnLocation = Lane->GetComponentTransform();

		if (UKismetMathLibrary::InRange_FloatFloat(RandVal, SpawnPercent1, SpawnPercent2, true, true))
		{
			AObstacle* Obstacle = GetWorld()->SpawnActor<AObstacle>(SmallObstacleClass, SpawnLocation, SpawnParams);
			ChildActors.Add(Obstacle);
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(RandVal, SpawnPercent2, SpawnPercent3, true, true))
		{
			if (NumBigs <= 2)
			{
				AObstacle* Obstacle = GetWorld()->SpawnActor<AObstacle>(BigObstacleClass, SpawnLocation, SpawnParams);
				if (Obstacle)
				{
					NumBigs += 1;
				}
				ChildActors.Add(Obstacle);
			}
			else
			{
				AObstacle* Obstacle = GetWorld()->SpawnActor<AObstacle>(SmallObstacleClass, SpawnLocation, SpawnParams);
				ChildActors.Add(Obstacle);
			}
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(RandVal, SpawnPercent3, 0.9f, true, true))
		{
			ACoinItem* Coin = GetWorld()->SpawnActor<ACoinItem>(CoinItemClass, SpawnLocation, SpawnParams);
			/*ChildActors.Add(Coin);*/
		}
		else if (CubeClass) // New condition to spawn a physics-enabled cube
		{
			AActor* CubeActor = GetWorld()->SpawnActor<AActor>(CubeClass, SpawnLocation, SpawnParams);
	
		}
	}


	void AFloorTile::DestroyFloorTile()
	{
		if (DestroyHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(DestroyHandle);
		}

		for (auto Child : ChildActors)
		{
			if (IsValid(Child))
			{
				Child->Destroy();
			}
		}

		ChildActors.Empty();

		RunGameMode->RemoveTile(this);

		this->Destroy();

	}





	//// Called every frame
	//void AFloorTile::Tick(float DeltaTime)
	//{
	//	Super::Tick(DeltaTime);
	//
	//}


