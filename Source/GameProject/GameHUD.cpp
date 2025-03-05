// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "RunnerGameMode.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"



void UGameHUD::NativeConstruct()
{
	if (PauseBtn)
	{
		PauseBtn->OnClicked.AddDynamic(this, &UGameHUD::OnPauseClick);
	}
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(UGameplayStatics::GetPlayerController(GetWorld(), 0), this);
}




void UGameHUD::InitializeHUD(ARunnerGameMode* RunGameMode)
{
	if (RunGameMode)
	{

		CoinsCount->SetText(FText::AsNumber(0));
		LivesCount->SetText(FText::AsNumber(RunGameMode->MaxLives));
				

		RunGameMode->OnCoinsCountChanged.AddDynamic(this, &UGameHUD::SetCoinsCount);
		RunGameMode->OnLivesCountChanged.AddDynamic(this, &UGameHUD::SetLivesCount);
	}
}
		


void UGameHUD::SetCoinsCount(const int32 Count)
{
	CoinsCount->SetText(FText::AsNumber(Count));
}

void UGameHUD::SetLivesCount(int32 Count)
{
	LivesCount->SetText(FText::AsNumber(Count));
}

void UGameHUD::OnPauseClick()
{
	if (IsValid(PauseMenuWidgetClass))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		UUserWidget* Widget = CreateWidget(GetWorld(), PauseMenuWidgetClass);

		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}

