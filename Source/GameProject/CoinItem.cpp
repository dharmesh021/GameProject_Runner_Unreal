// Fill out your copyright notice in the Description page of Project Settings.

#include "CoinItem.h"
#include "RunCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACoinItem::ACoinItem()
{
    // Create and set the root scene component
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = SceneComponent;

    // Create and set up the sphere collider
    SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    SphereCollider->SetupAttachment(SceneComponent);
    SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

    // Create and set up the coin mesh
    CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
    CoinMesh->SetupAttachment(SphereCollider);
    CoinMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

    // Create and set up the rotating movement component
    RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
    RotatingMovement->RotationRate = FRotator(0, 180, 0);
}

// Called when the game starts or when spawned
void ACoinItem::BeginPlay()
{
    Super::BeginPlay();

    // Bind the overlap event to the OnSphereOverlap function
    SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ACoinItem::OnSphereOverlap);
}

// Function called when the sphere collider overlaps with another component
void ACoinItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Cast the overlapping actor to ARunCharacter
    ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

    // If the overlapping actor is a run character
    if (RunCharacter)
    {
        // Play the overlap sound if it is set
        if (OverlapSound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), OverlapSound, GetActorLocation());
        }

        // Add a coin to the run character
        RunCharacter->AddCoin();

        // Destroy the coin item
        Destroy();
    }    
}
