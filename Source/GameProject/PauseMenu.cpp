// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UPauseMenu::NativeConstruct()
{
	// Bind the Continue button click event to the OnContinueClick function
	if (ContinueBtn)
	{
		ContinueBtn->OnClicked.AddDynamic(this, &UPauseMenu::OnContinueClick);
	}

	// Bind the Restart button click event to the OnRestartClick function
	if (RestartBtn)
	{
		RestartBtn->OnClicked.AddDynamic(this, &UPauseMenu::OnRestartClick);
	}
}

void UPauseMenu::OnContinueClick()
{
	// Unpause the game and remove the pause menu from the viewport
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	RemoveFromParent();
}

void UPauseMenu::OnRestartClick()
{
	UWorld* World = GetWorld();

	if (World)
	{
		// Restart the level using a console command
		UKismetSystemLibrary::ExecuteConsoleCommand(World, TEXT("RestartLevel"));
	}
}
