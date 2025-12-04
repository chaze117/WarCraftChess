// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayerController.h"
#include  "Kismet/GameplayStatics.h"

void AChessPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed,this, &AChessPlayerController::HandleLeftClick);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	if (GetWorld())
	{
		TArray<AActor*> FoundBoards;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChessBoard::StaticClass(), FoundBoards);
		if (FoundBoards.Num() > 0)
		{
			ChessBoardReference = Cast<AChessBoard>(FoundBoards[0]);
		}
	}
}

void AChessPlayerController::HandleLeftClick()
{
	//ChessBoardReference->MouseClicked();
}
