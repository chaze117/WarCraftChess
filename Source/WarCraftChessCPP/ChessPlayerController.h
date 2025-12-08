// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChessBoard.h"
#include "ChessPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WARCRAFTCHESSCPP_API AChessPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	void HandleLeftClick();
	void DebugClickTrace();
	
	UPROPERTY() 
	AChessBoard* ChessBoardReference;
};
