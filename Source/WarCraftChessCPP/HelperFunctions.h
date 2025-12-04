// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterPiece.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HelperFunctions.generated.h"

/**
 * 
 */
UCLASS()
class WARCRAFTCHESSCPP_API UHelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	static FString GetPositionLetter(const FString& Position);
	
	UFUNCTION()
	static FString GetPositionNumber(const FString& Position);
	
	UFUNCTION()
	static TArray<FString> CombineStringArrays(const TArray<FString>& ArrayA, const TArray<FString>& ArrayB);
	
	UFUNCTION()
	static FString GetNumberFromLetter(const FString& Letter);
	
	UFUNCTION()
	static FString GetLetterFromNumber(const FString& Number);
	
	UFUNCTION()
	static FString GetPositionInDirection(int32 Length, const AMasterPiece* Piece, EDirections Direction);
	
	UFUNCTION()
	static int32 GetEdgeLength(const AMasterPiece* Piece, EDirections Direction);
	
	UFUNCTION()
	static TArray<FString> FindPathInDirection(AMasterPiece* Piece, EDirections Direction);
	
	UFUNCTION()
	static void CleanPositionsArray(TArray<FString>& Positions);
	
	UFUNCTION(BlueprintCallable)
	static void SetButtonImage(UButton* Button, UTexture2D* Texture);
	
	UFUNCTION(BlueprintCallable)
	static void SetBrush(UImage* Target, UTexture2D* Texture);
};
