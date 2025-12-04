// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBoard.h"
#include "ChessCharacter.h"
#include "MoveHighlight.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AChessBoard::AChessBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AChessBoard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	TArray<UTileComponent*> FoundTiles;
	GetComponents<UTileComponent>(FoundTiles);
	for (UTileComponent* Tile : FoundTiles)
	{
		if (Tile)
		{
			AllTiles.Add(Tile->GetTileName(), Tile);
		}
	}
}

// Called when the game starts or when spawned
void AChessBoard::BeginPlay()
{
	Super::BeginPlay();

	ChessInstance = Cast<UChessInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!ChessInstance) return;
	WhiteRace = ChessInstance->WhiteRace;
	BlackRace = ChessInstance->BlackRace;
	if (GetWorld()) PlayerCharacter = Cast<AChessCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	InitalizePieces();
	TArray<AMasterPiece*> GeneratedPieces;
	AllPieces.GenerateValueArray(GeneratedPieces);
	UpdateMoves(GeneratedPieces);
}

// Called every frame
void AChessBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AMasterPiece* AChessBoard::SpawnPiece(const TSubclassOf<AMasterPiece> Class, const ETeams Team, UTileComponent* Tile)
{
	if (!Class || !Tile || !GetWorld()) return nullptr;	
	const FVector  SpawnLocation = Tile->GetComponentLocation() + FVector(0.f, 0.f, 110.f);
	const FRotator SpawnRotation = FRotator(0.f,Team == ETeams::White ? 180.f : 0.f, 0.f);
	AMasterPiece* SpawnedPiece = GetWorld()->SpawnActor<AMasterPiece>(Class, SpawnLocation, SpawnRotation);
	AllPieces.Add(Tile->GetTileName(), SpawnedPiece);
	SpawnedPiece->Initalize(Team,Tile->GetTileName(), this);
	return SpawnedPiece;
}

FPieceClassPair AChessBoard::GetPieces(const EPieceTypes Type) const
{
	switch (Type)
	{
		case EPieceTypes::Bishop: return FPieceClassPair{WhiteRace.Bishop,BlackRace.Bishop};
		case EPieceTypes::King:   return FPieceClassPair{WhiteRace.King,BlackRace.King};
		case EPieceTypes::Knight: return FPieceClassPair{WhiteRace.Knight,BlackRace.Knight};
		case EPieceTypes::Pawn:   return FPieceClassPair{WhiteRace.Pawn,BlackRace.Pawn};
		case EPieceTypes::Queen:  return FPieceClassPair{WhiteRace.Queen,BlackRace.Queen};
		case EPieceTypes::Rook:  return FPieceClassPair{WhiteRace.Rook,BlackRace.Rook};
		default: return FPieceClassPair{WhiteRace.Pawn,BlackRace.Pawn};	
	}
}

void AChessBoard::InitalizePieces()
{
	TArray<AActor*> PieceActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMasterPiece::StaticClass(),PieceActors);
	for (AActor* PieceActor : PieceActors) PieceActor->Destroy();
	AllPieces.Empty();
	TArray<UTileComponent*> GeneratedTiles;
	AllTiles.GenerateValueArray(GeneratedTiles);
	FPieceClassPair Bishops = GetPieces(EPieceTypes::Bishop);
	FPieceClassPair Kings   = GetPieces(EPieceTypes::King);
	FPieceClassPair Knights = GetPieces(EPieceTypes::Knight);
	FPieceClassPair Pawns   = GetPieces(EPieceTypes::Pawn);
	FPieceClassPair Queens  = GetPieces(EPieceTypes::Queen);
	FPieceClassPair Rooks   = GetPieces(EPieceTypes::Rook);
	for (UTileComponent* Tile : GeneratedTiles)
	{
		switch (FCString::Atoi(*Tile->GetTileNumber()))
		{
			case 2: SpawnPiece(Pawns.WhiteClass,ETeams::White,Tile);break;
			case 7: SpawnPiece(Pawns.BlackClass, ETeams::Black,Tile);break;
		}
		switch (Tile->TileName)
		{
			case ETileName::A1: SpawnPiece(Rooks.WhiteClass,   ETeams::White, Tile);break;
			case ETileName::H1: SpawnPiece(Rooks.WhiteClass,   ETeams::White, Tile);break;
			case ETileName::A8: SpawnPiece(Rooks.BlackClass,   ETeams::Black, Tile);break;
			case ETileName::H8: SpawnPiece(Rooks.BlackClass,   ETeams::Black, Tile);break;
			case ETileName::B1: SpawnPiece(Knights.WhiteClass, ETeams::White, Tile);break;
			case ETileName::G1: SpawnPiece(Knights.WhiteClass, ETeams::White, Tile);break;
			case ETileName::B8: SpawnPiece(Knights.BlackClass, ETeams::Black, Tile);break;
			case ETileName::G8: SpawnPiece(Knights.BlackClass, ETeams::Black, Tile);break;
			case ETileName::C1: SpawnPiece(Bishops.WhiteClass, ETeams::White, Tile);break;
			case ETileName::F1: SpawnPiece(Bishops.WhiteClass, ETeams::White, Tile);break;
			case ETileName::C8: SpawnPiece(Bishops.BlackClass, ETeams::Black, Tile);break;
			case ETileName::F8: SpawnPiece(Bishops.BlackClass, ETeams::Black, Tile);break;
			case ETileName::D1: SpawnPiece(Queens.WhiteClass,  ETeams::White, Tile);break;
			case ETileName::D8: SpawnPiece(Queens.BlackClass,  ETeams::Black, Tile);break;
			case ETileName::E1: WhiteKing = SpawnPiece(Kings.WhiteClass, ETeams::White, Tile);break;
			case ETileName::E8: BlackKing = SpawnPiece(Kings.BlackClass, ETeams::Black, Tile);break;
		}
	}
}

void AChessBoard::PieceMoved(const ETeams Team)
{
	if (Team == PlayerCharacter->CurrentTeam) EndTurn();
}

FFindPiece AChessBoard::GetPositionInfo(const FString Position)
{

	if (AllPieces.Contains(Position))
	{
		AMasterPiece* Piece = AllPieces[Position];
		return FFindPiece(Piece, true, Piece->Team);
		
	}
	else
	{
		return FFindPiece(nullptr, false, ETeams::White);
	}
}

bool AChessBoard::IsPositionInDanger(const FString Position, const ETeams Team) const
{
	if (Position.Len() == 2)
	{
		TArray<AMasterPiece*> Pieces;
		AllPieces.GenerateValueArray(Pieces);
		for (const AMasterPiece* Piece : Pieces)
		{
			if (Piece && Piece->Team != Team)
			{
				if (Piece->ChessPieceController->CurrentAttacks.Contains(Position)) return true;
			}
		}
	}
	return false;
}

void AChessBoard::EndTurn()
{
	TArray<AMasterPiece*> Pieces;
	AllPieces.GenerateValueArray(Pieces);
	UpdateMoves(Pieces);
	PlayerCharacter->TogglePlayer();
	ClearCheckHighlights();
	if (IsKingInCheck()) HighLightCheckedKing();
	
}

bool AChessBoard::IsKingInCheck() const
{
	if (const AMasterPiece* CurrentKing = GetCurrentKing())
	{
		return IsPositionInDanger(CurrentKing->Position, PlayerCharacter->CurrentTeam);
	}
	return false;
}

void AChessBoard::Promote(const FString Position, AMasterPiece* Piece, const TSubclassOf<AMasterPiece> NewPiece)
{
	Piece->Destroy();
	AllPieces.Remove(Position);
	AllPieces.Add(Position, SpawnPiece(NewPiece,PlayerCharacter->CurrentTeam,AllTiles[Position]));
}

AMasterPiece* AChessBoard::GetCurrentKing() const
{
	if (PlayerCharacter)
	{
		switch (PlayerCharacter->CurrentTeam)
		{
		case ETeams::White: return WhiteKing;
		case ETeams::Black: return BlackKing;
		}
	}
	return WhiteKing;
}

void AChessBoard::UpdateMoves(TArray<AMasterPiece*> Pieces)
{
	for (AMasterPiece* Piece : Pieces)
	{
		if (Piece && Piece->ChessPieceController)
		{
			Piece->ChessPieceController->RefreshMoves();
		}
	}
}

void AChessBoard::ClearCheckHighlights()
{
	for (ACheckHighlight* HighLight : CheckHighlights) if (HighLight) HighLight->Destroy();
	CheckHighlights.Empty();
}

void AChessBoard::HighLightCheckedKing()
{
	
	CheckHighlights.Add(GetWorld()->SpawnActor<ACheckHighlight>(
		ChessInstance->CheckHighlightClass, 
		AllTiles[GetCurrentKing()->Position]->GetComponentLocation() +FVector(0.f,0.f,170.f),
		FRotator(0.f,0.f,0.f)));
	
}

void AChessBoard::HighLightValidPoints(TArray<FString> ValidPoints)
{
	for (FString Point : ValidPoints)
	{
		UTileComponent** TilePtr = AllTiles.Find(Point);
		UTileComponent* Tile = *TilePtr;
		MoveHighlights.Add(GetWorld()->SpawnActor<AMoveHighlight>(
			ChessInstance->MoveHighlightClass,
			Tile->GetComponentLocation() + FVector(0.f,0.f,30.f),
			FRotator(0.f,0.f,0.f)));
	}
}

void AChessBoard::OnPieceSelected(AMasterPiece* InCurrentPiece, const TArray<FString>& ValidPoints)
{
	ClearMoveHighLights();
	CurrentPiece = InCurrentPiece;

	HighLightValidPoints(ValidPoints);
}

void AChessBoard::ClearMoveHighLights()
{
	for (AMoveHighlight* HighLight : MoveHighlights) if (HighLight) HighLight->Destroy();
	MoveHighlights.Empty();
}

void AChessBoard::MoveToTile(UTileComponent* TargetTile)
{
	if (CurrentPiece && CurrentPiece->ChessPieceController)
	{
		if (CurrentPiece->ChessPieceController->GetAllPossibleMoves().Movements.Contains(TargetTile->GetTileName()) || 
			CurrentPiece->ChessPieceController->GetAllPossibleMoves().Attacks.Contains(TargetTile->GetTileName()))
		{
			Castle(TargetTile);
			CurrentPiece->ChessPieceController->Move(TargetTile);
		}
	}
}

void AChessBoard::Castle(UTileComponent* TargetTile)
{
	
	FString PointToCheck = "";
	if (GetCurrentKing() == CurrentPiece && CurrentPiece->ChessPieceController->CastleingPoints.Contains(TargetTile->GetTileName()))
	{
		
		switch (CurrentPiece->ChessPieceController->CastleingPoints[TargetTile->GetTileName()])
		{
		case EDirections::Left:	 PointToCheck = PlayerCharacter->CurrentTeam == ETeams::White ? "A1" : "H8";break;
		case EDirections::Right: PointToCheck = PlayerCharacter->CurrentTeam == ETeams::White ? "H1" : "A8";break;
		}
		AMasterPiece* PossibleRook = AllPieces[PointToCheck];
		if (PossibleRook && !PossibleRook->ChessPieceController->HasMoved && PossibleRook->ChessPieceController->Type == EPieceTypes::Rook)
		{
			UTileComponent* CastleTile;
			switch (CurrentPiece->ChessPieceController->CastleingPoints[TargetTile->GetTileName()])
			{
			case EDirections::Left:  CastleTile = PlayerCharacter->CurrentTeam == ETeams::White ? AllTiles["C1"] : AllTiles["F8"]; break;
			case EDirections::Right: CastleTile = PlayerCharacter->CurrentTeam == ETeams::White? AllTiles["F1"] : AllTiles["C8"]; break;
			default: CastleTile = AllTiles["F8"]; break;
			}
			PossibleRook->ChessPieceController->Move(CastleTile);
		}
	}
}

