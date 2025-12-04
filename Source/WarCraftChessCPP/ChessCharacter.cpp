// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChessCharacter::AChessCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);
	RootComponent = GetCapsuleComponent();

}

// Called when the game starts or when spawned
void AChessCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		ChessPlayerController = Cast<AChessPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
		if (ChessPlayerController)
		{
			SetUpCameras();
		}
		TArray<AActor*> FoundBoards;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChessBoard::StaticClass(), FoundBoards);
		if (FoundBoards.Num()> 0 ) ChessBoardReference = Cast<AChessBoard>(FoundBoards[0]);
		OnAfterBeginPlay();
	}
}

// Called every frame
void AChessCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChessCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChessCharacter::SetUpCameras()
{
	if (GetWorld())
	{
		TArray<AActor*> FoundCameras;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),AChessCamera::StaticClass(), FoundCameras);
		for (AActor* Camera : FoundCameras)
		{
			if (Camera->ActorHasTag("Camera.P1")) P1Cam = Cast<AChessCamera>(Camera);
			if (Camera->ActorHasTag("Camera.P2")) P2Cam = Cast<AChessCamera>(Camera);
		}
		ChessPlayerController->SetViewTargetWithBlend(P1Cam,0.f,VTBlend_Linear,0.f,false);
	}
}

void AChessCharacter::TogglePlayer()
{
	if (GetWorld() && ChessPlayerController)
	{
		switch (CurrentTeam)
		{
		case ETeams::Black:
			{
				CurrentTeam = ETeams::White;
				ChessPlayerController->SetViewTargetWithBlend(P1Cam,0.5f,VTBlend_Linear,0.f,false);
				break;
			}
		case ETeams::White:
			{
				CurrentTeam = ETeams::Black;
				ChessPlayerController->SetViewTargetWithBlend(P2Cam,0.5f,VTBlend_Linear,0.f,false);
				break;
			}
			default: break;
		}
	}
}

bool AChessCharacter::KeyIsDown(const FKey Key) const
{
	return ChessPlayerController->IsInputKeyDown(Key);
}

