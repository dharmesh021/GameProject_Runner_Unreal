// Fill out your copyright notice in the Description page of Project Settings.

#include "RunCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RunnerGameMode.h"

// Sets default values
ARunCharacter::ARunCharacter()
{
		// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and configure the camera arm component
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->TargetArmLength = 350.f;
	CameraArm->SocketOffset = FVector(0.f, 0.f, 100.f);
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->SetupAttachment(GetRootComponent());

	// Create and configure the camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the game mode and ensure it is valid
	RunGameMode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	check(RunGameMode);

	// Bind the level reset event
	RunGameMode->OnLevelReset.AddDynamic(this, &ARunCharacter::ResetLevel);

	// Get the player start actor
	PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
}

// Add a coin to the game mode
void ARunCharacter::AddCoin()
{
	RunGameMode->AddCoin();
}

// Increase the character's movement speed
void ARunCharacter::IncreaseSpeed()
{
	MovementSpeed += SpeedIncreaseAmount; // Example: SpeedIncreaseAmount = 100.0f
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

// Called every frame
void ARunCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get the control rotation and zero out the roll and pitch
	FRotator ControlRot = GetControlRotation();
	ControlRot.Roll = 0.f;
	ControlRot.Pitch = 0.f;

	// Add movement input based on the control rotation
	AddMovementInput(ControlRot.Vector());
}

// Called to bind functionality to input
void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump actions
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunCharacter::StopJumping);

	// Bind movement actions
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARunCharacter::MoveLeft);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARunCharacter::MoveRight);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ARunCharacter::MoveDown);
}

// Update the character's lane change
void ARunCharacter::ChangeLaneUpdate(const float Value)
{
	FVector Location = GetCapsuleComponent()->GetComponentLocation();
	Location.Y = FMath::Lerp(RunGameMode->LaneSwitchValues[CurrentLane], RunGameMode->LaneSwitchValues[NextLane], Value);
	SetActorLocation(Location);
}

// Finish the lane change
void ARunCharacter::ChangeLaneFinished()
{
	CurrentLane = NextLane;
}

// Handle the character's death
void ARunCharacter::Death()
{
	if (!bIsDead)
	{
		const FVector Location = GetActorLocation();
		UWorld* World = GetWorld();

		if (World)
		{
			bIsDead = true;
			DisableInput(nullptr);

			// Spawn death particle system if available
			if (DeathParticleSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, DeathParticleSystem, Location);
			}

			// Play death sound if available
			if (DeathSound)
			{
				UGameplayStatics::PlaySoundAtLocation(World, DeathSound, Location);
			}

			// Hide the character mesh
			GetMesh()->SetVisibility(false);

			// Set a timer to handle post-death actions
			World->GetTimerManager().SetTimer(DeathHandle, this, &ARunCharacter::OnDeath, 1.f);
		}
	}
}

// Spawn an actor at the character's location
void ARunCharacter::SpawnActor()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(actorBPtoSpawn, GetActorTransform(), SpawnParams);
}

// Handle post-death actions
void ARunCharacter::OnDeath()
{
	if (DeathHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DeathHandle);
	}

	// Notify the game mode that the player died
	RunGameMode->PlayerDied();
}

// Reset the level
void ARunCharacter::ResetLevel()
{
	bIsDead = false;
	EnableInput(nullptr);
	GetMesh()->SetVisibility(true);

	// Reset the character's location and rotation to the player start
	if (PlayerStart)
	{
		SetActorLocation(PlayerStart->GetActorLocation());
		SetActorRotation(PlayerStart->GetActorRotation());
	}
}

// Move the character to the left lane
void ARunCharacter::MoveLeft()
{
	NextLane = FMath::Clamp(CurrentLane - 1, 0, 2);
	ChangeLane();
}

// Move the character to the right lane
void ARunCharacter::MoveRight()
{
	NextLane = FMath::Clamp(CurrentLane + 1, 0, 2);
	ChangeLane();
}

// Move the character down
void ARunCharacter::MoveDown()
{
	static FVector Impulse = FVector(0, 0, MoveDownImpulse);
	GetCharacterMovement()->AddImpulse(Impulse, true);
}
