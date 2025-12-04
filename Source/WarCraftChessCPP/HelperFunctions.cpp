// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions.h"

#include "Styling/SlateBrush.h"
#include "ChessBoard.h"

FString UHelperFunctions::GetPositionLetter(const FString& Position)
{
	return Position.Left(1);
}

FString UHelperFunctions::GetPositionNumber(const FString& Position)
{
	return Position.Right(1);
}

TArray <FString> UHelperFunctions::CombineStringArrays(const TArray<FString>& ArrayA, const TArray<FString>& ArrayB)
{
	TArray<FString> CombinedArray = ArrayA;
	CombinedArray.Append(ArrayB);
	return CombinedArray;
}

FString UHelperFunctions::GetNumberFromLetter(const FString& Letter)
{
	static  const TArray<FString> Letters = {
		"A", "B", "C", "D", "E", "F", "G", "H"
	};
	return FString::FromInt(Letters.IndexOfByKey(Letter) + 1);
}

FString UHelperFunctions::GetLetterFromNumber(const FString& Number)
{
	static  const TArray<FString> Letters = {
		"A", "B", "C", "D", "E", "F", "G", "H"
	};
	int32 Index = FCString::Atoi(*Number) - 1;
	if (Index >= 0 && Index < Letters.Num())
	{
		return Letters[Index];
	}
	return "";
}

FString UHelperFunctions::GetPositionInDirection(int32 Length, const AMasterPiece* Piece, EDirections Direction)
{
	int32 Letter = FCString::Atoi(*GetNumberFromLetter(GetPositionLetter(Piece->Position)));
	int32 Number = FCString::Atoi(*GetPositionNumber(Piece->Position));
	
	if (Piece)
	{
		if (Piece->Team == ETeams::Black)
		{
			Length *= -1;
		}
		switch (Direction)
		{
		case EDirections::Backward:
			Number -= Length;
			break;
		case EDirections::Forward:
			Number += Length;
			break;
		case EDirections::Left:
			Letter -= Length;
			break;
		case EDirections::Right:
			Letter += Length;
			break;
		case EDirections::ForwardLeft:
			Number += Length;
			Letter -= Length;
			break;
		case EDirections::ForwardRight:
			Number += Length;
			Letter += Length;
			break;
		case EDirections::BackwardLeft:
			Number -= Length;
			Letter -= Length;
			break;
		case EDirections::BackwardRight:
			Number -= Length;
			Letter += Length;
			break;
		default:
			break;
		}
		
	}
			
	return GetLetterFromNumber(FString::FromInt(Letter)) + FString::FromInt(Number);
	
}

int32 UHelperFunctions::GetEdgeLength(const AMasterPiece* Piece, const EDirections Direction)
{
	const int32 Letter = FCString::Atoi(*GetNumberFromLetter(GetPositionLetter(Piece->Position)));
	const int32 Number = FCString::Atoi(*GetPositionNumber(Piece->Position));

	switch (Direction)
	{
	case EDirections::Left:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				return Letter - 1;	
			case ETeams::Black:
				return 8 - Letter;
			default:
				return 0;
			}
		}
	case EDirections::Right:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				return 8 - Letter;	
			case ETeams::Black:
				return Letter - 1;
			default:
				return 0;
			}
		}
	case EDirections::Forward:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				return 8 - Number;
			case ETeams::Black:
				return Number - 1;
			default:
				return 0;
			}
		}
	case EDirections::Backward:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				return Number - 1;
			case ETeams::Black:
				return 8 - Number;
			default:
				return 0;
			}
		}
	case EDirections::ForwardLeft:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				{
					if (8 - Number >= Letter -1)
						return Letter - 1;
					else
						return 8 - Number;
				}
			case ETeams::Black:
				{
					if (Number - 1 >= 8 - Letter)
						return 8 - Letter;
					else
						return Number - 1;
				}
			default:
				return 0;
			}
		}
	case EDirections::ForwardRight:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				{
					if (8 - Number >= 8 - Letter)
						return 8 - Letter;
					else
						return 8 - Number;
				}
			case ETeams::Black:
				{
					if (Number - 1 >= Letter - 1)
						return Letter - 1;
					else
						return Number - 1;
				}
			default:
				return 0;
			}
		}
	case EDirections::BackwardLeft:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				{
					if (Number - 1 >= Letter - 1)
						return Letter - 1;
					else
						return Number - 1;
				}
			case ETeams::Black:
				{
					if (8 - Number >= 8 - Letter)
						return 8 - Letter;
					else
						return 8 - Number;
				}
			default:
				return 0;
			}
		}
	case EDirections::BackwardRight:
		{
			switch (Piece->Team)
			{
			case ETeams::White:
				{
					if (Number - 1 >= 8 - Letter)
						return 8 - Letter;
					else
						return Number - 1;
				}
			case ETeams::Black:
				{
					if (8 - Number >= Letter - 1)
						return Letter - 1;
					else
						return 8 - Number;
				}
			default:
				return 0;
			}
		}
	}
	return 0;
}

TArray<FString> UHelperFunctions::FindPathInDirection(AMasterPiece* Piece, EDirections Direction)
{
	/*AChessBoard* ChessBoardReference = Piece->ChessBoardReference;
	TArray<FString> Positions;
	int32 EdgeLength = GetEdgeLength(Piece, Direction);
	for (int32 i = 1; i <= EdgeLength; i++)
	{
		FString NewPosition = GetPositionInDirection(i, Piece, Direction);
		Positions.Add(NewPosition);
		FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(NewPosition);
		if (FoundPiece.PieceFound)
		{
			break;
		}
	}
	return Positions;*/return {""};
}

void UHelperFunctions::CleanPositionsArray(TArray<FString>& Positions)
{
	for (FString& Position : Positions)
	{
		Position.RemoveSpacesInline();
		if (Position.Len() != 2)
		{
			Positions.Remove(Position);
		}
	}
}

void UHelperFunctions::SetButtonImage(UButton* Button, UTexture2D* Texture)
{
	if (Button && Texture)
	{
		FButtonStyle Style = Button->GetStyle();
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		Brush.ImageSize = FVector2D(200.0f, 200.0f);
		
		Style.SetNormal(Brush);
		Style.SetHovered(Brush);
		Style.SetPressed(Brush);
		
		Button->SetStyle(Style);
	}
}

void UHelperFunctions::SetBrush(UImage* Target, UTexture2D* Texture)
{
	if (Target && Texture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
		Target->SetBrush(Brush);
	}
}
