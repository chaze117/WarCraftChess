#pragma once
#include "CoreMinimal.h"
#include "Enums.h"
#include "Structures.generated.h"

USTRUCT(BlueprintType)
struct FRaces : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> Bishop;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> King;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> Knight;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> Pawn;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> Queen;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> Rook;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ERaces Race;
	
	FRaces()
		: Bishop(nullptr)
		, King(nullptr)
		, Knight(nullptr)
		, Pawn(nullptr)
		, Queen(nullptr)
		, Rook(nullptr)
		, Race(ERaces::Undead)  
	{}
};

USTRUCT(BlueprintType)
struct FPromote : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* Bishop;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* King;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* Knight;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* Queen;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ERaces Race;
	
	FPromote()
		: Bishop(nullptr)
		, King(nullptr)
		, Knight(nullptr)
		, Queen(nullptr)
		, Race(ERaces::Undead) 
	{}
};

USTRUCT(BlueprintType)
struct FPieceClassPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> WhiteClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMasterPiece> BlackClass;

	FPieceClassPair()
		: WhiteClass(nullptr), BlackClass(nullptr) {}

	FPieceClassPair(
		TSubclassOf<class AMasterPiece> InWhite,
		TSubclassOf<class AMasterPiece> InBlack)
		: WhiteClass(InWhite), BlackClass(InBlack)
	{
	}
};

USTRUCT(BlueprintType)
struct FFindPiece
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMasterPiece* Piece;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PieceFound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeams Team;

	FFindPiece()
		: Piece(nullptr), PieceFound(false), Team(ETeams::White) {}

	FFindPiece(
		class AMasterPiece* InPiece,
		const bool InPieceFound,
		const ETeams InTeam)
		: Piece(InPiece), PieceFound(InPieceFound), Team(InTeam)
	{
	}
};

USTRUCT(BlueprintType)
struct FMovements
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Movements;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Attacks;	
	
	FMovements()
		: Movements(TArray<FString>()), Attacks(TArray<FString>()) {}
	
	FMovements(
		const TArray<FString>& InMovements,
		const TArray<FString>& InAttacks)
		: Movements(InMovements), Attacks(InAttacks)
	{
	}
};
