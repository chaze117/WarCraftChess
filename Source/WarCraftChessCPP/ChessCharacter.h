// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums.h"
#include  "ChessCamera.h"
#include "ChessPlayerController.h"
#include "ChessCharacter.generated.h"

UCLASS()
class WARCRAFTCHESSCPP_API AChessCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AChessCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintImplementableEvent) 
	void OnAfterBeginPlay();
	
	UPROPERTY(BlueprintReadOnly)
	AChessCamera* P1Cam;
	
	UPROPERTY(BlueprintReadOnly)
	AChessCamera* P2Cam;
	
	UPROPERTY(BlueprintReadOnly)
	AChessPlayerController* ChessPlayerController;
	
	UPROPERTY(BlueprintReadOnly)
	ETeams CurrentTeam = ETeams::White;
	
	UPROPERTY(BlueprintReadOnly)
	class AChessBoard* ChessBoardReference;
	
	UFUNCTION(BlueprintCallable)
	void TogglePlayer();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool KeyIsDown(const FKey Key) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void SetUpCameras();

};
