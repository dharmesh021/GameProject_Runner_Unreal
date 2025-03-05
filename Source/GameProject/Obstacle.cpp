// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"
#include "RunCharacter.h"

// Sets default values
AObstacle::AObstacle()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneComponent);

	

}

void AObstacle::OnObstacleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

	if (RunCharacter)
	{
		RunCharacter->Death();
	}
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->OnComponentHit.AddDynamic(this, &AObstacle::OnObstacleHit);
	
}




