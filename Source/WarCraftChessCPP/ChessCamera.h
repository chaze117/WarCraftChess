// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "ChessCamera.generated.h"

/**
 * 
 */
UCLASS()
class WARCRAFTCHESSCPP_API AChessCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	AChessCamera();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Control")
	float MaxDistance = 1700.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Control")
	float MinDistance = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Control")
	float ZoomRate = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Control")
	float RotateRate = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Control")
	float PanRate = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Positioning")
    FTransform DefaultTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess Camera Settings.Positioning")
	FRotator SpringArmRotation = FRotator(-30.f, 0.f, 0.f);
	
	UPROPERTY()
	class UBoxComponent* MarkerBox;
	
	UPROPERTY()
	class USpringArmComponent* SpringArm;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION(BlueprintCallable, Category = "Chess Camera Control")
	void Zoom(float Value);
	
	UFUNCTION(BlueprintCallable, Category = "Chess Camera Control")
	void ResetCamera();
	
	UFUNCTION(BlueprintCallable, Category = "Chess Camera Control")
	void RotateAndPanCamera(FVector2D Value,const AChessPlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "Chess Camera Control")
	void SnapToPiece(AChessBoard* ChessBoardReference);
};
