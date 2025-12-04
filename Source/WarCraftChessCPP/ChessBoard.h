// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckHighlight.h"
#include "ChessInstance.h"
#include "MasterPiece.h"
#include "Structures.h"
#include "Components/TileComponent.h"
#include  "Components/ChessPieceController.h"
#include "GameFramework/Actor.h"
#include "ChessBoard.generated.h"

UCLASS()
class WARCRAFTCHESSCPP_API AChessBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessBoard();
	UPROPERTY()
	AMasterPiece* CurrentPiece;
	
	UPROPERTY()
	TMap<FString, AMasterPiece*> AllPieces;
	
	UPROPERTY()
	TMap<FString, UTileComponent*> AllTiles;
	
	UPROPERTY(BlueprintReadWrite)
	FRaces WhiteRace;

	UPROPERTY(BlueprintReadWrite)
	FRaces BlackRace;
	
	UPROPERTY()
	UChessInstance* ChessInstance;
	
	UPROPERTY()
	AMasterPiece* WhiteKing;
	
	UPROPERTY()
	AMasterPiece* BlackKing;
		
	UPROPERTY()
	AChessCharacter* PlayerCharacter;
	
	UPROPERTY()
	TArray<ACheckHighlight*> CheckHighlights; 
	
	UPROPERTY()
	TArray<AMoveHighlight*> MoveHighlights;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	AMasterPiece* SpawnPiece(TSubclassOf<AMasterPiece> Class, ETeams Team, UTileComponent* Tile);
	
	UFUNCTION()
	FPieceClassPair GetPieces(const EPieceTypes Type) const;
	
	UFUNCTION(BlueprintCallable)
	void InitalizePieces();
	
	UFUNCTION(BlueprintCallable)
	void PieceMoved(ETeams Team);

	UFUNCTION(BlueprintCallable)
	FFindPiece GetPositionInfo(FString Position);
	
	UFUNCTION(BlueprintCallable)
	bool IsPositionInDanger(FString Position, ETeams Team) const;
	
	UFUNCTION(BlueprintCallable)
	bool IsKingInCheck() const;
	
	UFUNCTION(BlueprintCallable)
	void Promote(FString Position, AMasterPiece* Piece, TSubclassOf<AMasterPiece> NewPiece);
	
	UFUNCTION(BlueprintCallable)
	void EndTurn();
	
	UFUNCTION(BlueprintCallable)
	AMasterPiece* GetCurrentKing() const;
	
	UFUNCTION(BlueprintCallable)
	void UpdateMoves(TArray<AMasterPiece*> Pieces);
	
	UFUNCTION()
	void ClearCheckHighlights();
	
	UFUNCTION()
	void HighLightCheckedKing();
	
	UFUNCTION()
	void HighLightValidPoints(TArray<FString> ValidPoints);
	
	UFUNCTION()
	void OnPieceSelected(AMasterPiece* InCurrentPiece, const TArray<FString>& ValidPoints);
	
	UFUNCTION()
	void ClearMoveHighLights();
	
	UFUNCTION()
	void MoveToTile(UTileComponent* TargetTile);
	
	UFUNCTION()
	void Castle(UTileComponent* TargetTile);
};
