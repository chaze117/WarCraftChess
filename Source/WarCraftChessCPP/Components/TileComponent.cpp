// Fill out your copyright notice in the Description page of Project Settings.


#include "WarCraftChessCPP/Components/TileComponent.h"
#include "WarCraftChessCPP/ChessBoard.h"
#include "WarCraftChessCPP/ChessCharacter.h"
#include "Kismet/GameplayStatics.h"
UTileComponent::UTileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionResponseToAllChannels(ECR_Block);
	SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // Click trace ezt használja

	SetGenerateOverlapEvents(true);
	bReturnMaterialOnMove = true;

	// Hogy a komponens kaphasson inputot:
	bSelectable = true;
}
void UTileComponent::BeginPlay()
{
	Super::BeginPlay();

	OnClicked.AddDynamic(this, &UTileComponent::OnTileClicked);
}

void UTileComponent::OnTileClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Tile %s clicked"), *GetTileName());
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard->CurrentPiece == nullptr) return;
	UChessPieceController* SelectedPieceController = ChessBoard->CurrentPiece->ChessPieceController;
	if (ChessBoard && (SelectedPieceController->CurrentMoves.Contains(this->GetTileName()) || SelectedPieceController->CurrentAttacks.Contains(this->GetTileName())))
	{
		ChessBoard->MoveToTile(this);
		ChessBoard->CurrentPiece->SetSelected(false);
	}
	else
	{
		ChessBoard->CurrentPiece->SetSelected(false);
		ChessBoard->CurrentPiece = nullptr;
		ChessBoard->ClearMoveHighLights();
	}
}
