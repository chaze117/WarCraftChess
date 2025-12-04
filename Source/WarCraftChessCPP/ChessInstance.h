// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures.h"
#include "Engine/GameInstance.h"
#include "ChessInstance.generated.h"

/**
 * 
 */
UCLASS()
class WARCRAFTCHESSCPP_API UChessInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMoveHighlight> MoveHighlightClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACheckHighlight> CheckHighlightClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRaces WhiteRace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRaces BlackRace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAI;
};
