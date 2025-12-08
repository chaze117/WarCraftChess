// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayerController.h"
#include  "Kismet/GameplayStatics.h"

void AChessPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed,this, &AChessPlayerController::HandleLeftClick);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	if (GetWorld())
	{
		TArray<AActor*> FoundBoards;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChessBoard::StaticClass(), FoundBoards);
		if (FoundBoards.Num() > 0)
		{
			ChessBoardReference = Cast<AChessBoard>(FoundBoards[0]);
		}
	}
}

void AChessPlayerController::HandleLeftClick()
{
	//ChessBoardReference->MouseClicked();
}
void AChessPlayerController::DebugClickTrace()
{
	FVector WorldMouseLocation, WorldMouseDirection;
	if (!DeprojectMousePositionToWorld(WorldMouseLocation, WorldMouseDirection)) return;

	FVector TraceEnd = WorldMouseLocation + WorldMouseDirection * 10000.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = false;
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, WorldMouseLocation, TraceEnd, ECC_Visibility, Params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit Actor: %s, Component: %s"), 
			   *Hit.GetActor()->GetName(), *Hit.GetComponent()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit nothing"));
	}

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), WorldMouseLocation, TraceEnd, FColor::Red, false, 2.f, 0, 1.f);
#endif
}