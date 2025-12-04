// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessCamera.h"

#include "ChessPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

AChessCamera::AChessCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultTransform = FTransform(FRotator(0.f, 0.f, 0.f), FVector(0.f, 0.f, 50.f), FVector(1.f, 1.f, 1.f));
	
	MarkerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MarkerBox"));
	MarkerBox->SetBoxExtent(FVector(10.f));
	RootComponent = MarkerBox;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = MaxDistance;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bDoCollisionTest = false;
	
	if (UCameraComponent* Camera = GetCameraComponent())
	{
		Camera->SetupAttachment(SpringArm);
		Camera->bUsePawnControlRotation = false;
	}
	
	
}

void AChessCamera::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (SpringArm)
	{
		SpringArm->SetRelativeRotation(SpringArmRotation);
	}
}

void AChessCamera::Zoom(float Value)

{
	if (SpringArm)
	{
		if (Value > 0.f)
			SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength - ZoomRate, MinDistance, MaxDistance);
		else
			SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + ZoomRate, MinDistance, MaxDistance);
	}
	
}

void AChessCamera::ResetCamera()
{
	if (SpringArm)
	{
		SpringArm->TargetArmLength = MaxDistance;
		SpringArm->SetRelativeRotation(SpringArmRotation);
	}
	this->SetActorTransform(DefaultTransform,false);
}

void AChessCamera::RotateAndPanCamera(const FVector2D Value,const AChessPlayerController* PC)
{
	if (PC)
	{
		if (PC->IsInputKeyDown(EKeys::RightMouseButton))
		{
			if (SpringArm)
			{
				SpringArm->AddWorldRotation(FRotator(0.f, -Value.X*RotateRate, 0.f));
				FRotator CurrentRotation = SpringArm->GetRelativeRotation();
				const float NewPitch = FMath::Clamp(CurrentRotation.Pitch + Value.Y * RotateRate, -90.f, 0.f);
				CurrentRotation.Pitch = NewPitch;
				SpringArm->SetRelativeRotation(CurrentRotation);
			}
		}
		if (PC->IsInputKeyDown(EKeys::MiddleMouseButton))
		{
			const FVector CurrentLocation = this->GetActorLocation();
			this->SetActorLocation(FVector(FMath::Clamp(CurrentLocation.X + Value.Y*PanRate, -800.f, 800.f),
										   FMath::Clamp(CurrentLocation.Y + Value.X*PanRate, -800.f, 800.f),
										   CurrentLocation.Z));
		}
	}
}

void AChessCamera::SnapToPiece(AChessBoard* ChessBoardReference)
{
	if (ChessBoardReference && ChessBoardReference->CurrentPiece)
	{
		this->SetActorLocation(ChessBoardReference->CurrentPiece->GetActorLocation());
			
	}
}
