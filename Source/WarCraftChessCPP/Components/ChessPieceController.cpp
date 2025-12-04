// Fill out your copyright notice in the Description page of Project Settings.


#include "WarCraftChessCPP/Components/ChessPieceController.h"
#include "WarCraftChessCPP/ChessBoard.h"
#include "WarCraftChessCPP/HelperFunctions.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WarCraftChessCPP/MasterPiece.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarCraftChessCPP/ChessCharacter.h"


// Sets default values for this component's properties
UChessPieceController::UChessPieceController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//AIControllerClass = 
	// ...
}



// Called when the game starts
void UChessPieceController::BeginPlay()
{
	Super::BeginPlay();

	ThisPiece = Cast<AMasterPiece>(GetOwner());
	ChessInstance = ChessInstance = Cast<UChessInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Mesh = ThisPiece->GetMesh();
	TargetPiece = nullptr;
	
	
}


// Called every frame
void UChessPieceController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UChessPieceController::Move(UTileComponent* TargetTile)
{
	Tile = TargetTile;
	if (const FFindPiece TargetedPiece = CheckTargetTilesOccupied(TargetTile); TargetedPiece.PieceFound)
	{
		TargetPiece = TargetedPiece.Piece;
		switch(AttackType)
		{
		case EAttackTypes::Meele:		FindAttackPosition(TargetPiece);break;
		case EAttackTypes::MeeleNotify: FindAttackPosition(TargetPiece); break;
		case EAttackTypes::Channeled:   DoRangedAttack();
		case EAttackTypes::Ranged:		DoRangedAttack();
		case EAttackTypes::Notify:		NotifyAttack();
		}
	}
	else MoveWithoutAttack();
	
}

void UChessPieceController::MoveWithoutAttack() const
{
	if (AAIController* AIController = Cast<AAIController>(ThisPiece->GetController()))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(AIController, GetMoveDestination(Tile));
		ThisPiece->SetActorRotation(ThisPiece->Team == ETeams::White ? FRotator(0.f, -90.f, 0.f) : FRotator(0.f, 90.f, 0.f));
		if (CanPromote())
		{
			if (UUserWidget* PromotionWidget = CreateWidget<UUserWidget>(GetWorld(), ThisPiece->PromotionWidgetClass))
			{
				PromotionWidget->AddToViewport();
			}
		}
		ChessBoardReference->PieceMoved(ThisPiece->Team);
	}
	
}

void UChessPieceController::FindAttackPosition(const AMasterPiece* Target) const
{
	if (AAIController* AIController = Cast<AAIController>(ThisPiece->GetController()))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(AIController,FindAtkPos(Target));
		ThisPiece->SetActorRotation(ThisPiece->Team == ETeams::White ? FRotator(0.f, -90.f, 0.f) : FRotator(0.f, 90.f, 0.f));
		switch (AttackType)
		{
		case EAttackTypes::Meele:
			{
				break;
			}
		case EAttackTypes::MeeleNotify:
			{
				break;
			}
		}
	}
}

void UChessPieceController::DoMeeleAttack()
{
	Mesh->PlayAnimation(Attack,false);
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			[this]()
			{
				Mesh->SetAnimInstanceClass(Mesh->GetAnimClass());
				AfterAttack();
				
			},
			Attack->GetPlayLength(), false
		);
	});
}

void UChessPieceController::AfterAttack()
{

	TargetPiece->ChessPieceController->GetKilled();
	if (TargetPiece->ChessPieceController->Death)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(
				DelayHandle,
				[this]()
				{
					MoveWithoutAttack();					
				},
				TargetPiece->ChessPieceController->Death->GetPlayLength(), false
			);
		});
	}
	else if (TargetPiece->ChessPieceController->DeathMontage)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(
				DelayHandle,
				[this]()
				{
					MoveWithoutAttack();					
				},
				TargetPiece->ChessPieceController->DeathMontage->GetPlayLength(), false
			);
		});
	}
	else MoveWithoutAttack();
	
}

void UChessPieceController::GetKilled() const
{
	USkeletalMeshComponent* LocalMesh = Type == EPieceTypes::Knight ? ThisPiece->Mount : Mesh;
	if (Death)
	{
		
		LocalMesh->PlayAnimation(Death,false);
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(
				DelayHandle,
				[this]()
				{
					ThisPiece->Destroy();
					
				},
				Death->GetPlayLength(), false
			);
		});
	}
	else if (DeathMontage)
	{
		UAnimInstance* AnimInst = LocalMesh->GetAnimInstance();
    
		AnimInst->Montage_Play(DeathMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			if (ThisPiece)
			{
				ThisPiece->Destroy();
			}
		});

		AnimInst->Montage_SetEndDelegate(EndDelegate, DeathMontage);
	}
	else ThisPiece->Destroy();
}

void UChessPieceController::DoRangedAttack()
{
	ThisPiece->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Mesh->GetComponentLocation(), TargetPiece->ChessPieceController->Mesh->GetComponentLocation()));
	ShootProjectile(TargetPiece, ProjectileSocketName, Projectile,ProjectileSpeed);
}

void UChessPieceController::ShootProjectile(const AMasterPiece* Target,  FName SocketName, TSubclassOf<AProjectile> ProjectileClass, float Speed)
{
	ThisPiece->SetActorRotation(UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::GetDirectionUnitVector(ThisPiece->ChessPieceController->Mesh->GetComponentLocation(),Target->ChessPieceController->Mesh->GetComponentLocation())*Speed));
	if (PreAttack)
	{
		Mesh->PlayAnimation(PreAttack,false);
		GetWorld()->GetTimerManager().SetTimerForNextTick([this,SocketName,ProjectileClass,Speed]()
		{
			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(
				DelayHandle,
				[this,SocketName,ProjectileClass,Speed]()
				{
					Mesh->PlayAnimation(Attack,false);
					GetWorld()->GetTimerManager().SetTimerForNextTick([this,SocketName,ProjectileClass,Speed]()
					{
						FTimerHandle DelayHandle2;
						GetWorld()->GetTimerManager().SetTimer(
							DelayHandle2,
							[this,SocketName,ProjectileClass,Speed]()
							{
								FActorSpawnParameters SpawnInfo;
								SpawnInfo.Owner = ThisPiece;
								SpawnInfo.Instigator = ThisPiece->GetInstigator();
								SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
								AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,Mesh->GetSocketLocation(SocketName),FRotator(0.f,0.f,0.f),SpawnInfo);			
								SpawnedProjectile->Target = TargetPiece;
								SpawnedProjectile->Speed = Speed;
								Mesh->SetAnimInstanceClass(Mesh->GetAnimClass());
								SpawnedProjectile->OnImpact.AddDynamic(this, &UChessPieceController::AfterAttack);
							},
							Attack->GetPlayLength(), false
						);
					});
				},
				PreAttack->GetPlayLength(), false
			);
		});
	}
	else
	{
		Mesh->PlayAnimation(Attack,false);
		GetWorld()->GetTimerManager().SetTimerForNextTick([this,SocketName,ProjectileClass,Speed]()
		{
			FTimerHandle DelayHandle2;
			GetWorld()->GetTimerManager().SetTimer(
				DelayHandle2,
				[this,SocketName,ProjectileClass,Speed]()
				{
					FActorSpawnParameters SpawnInfo;
					SpawnInfo.Owner = ThisPiece;
					SpawnInfo.Instigator = ThisPiece->GetInstigator();
					SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,Mesh->GetSocketLocation(SocketName),FRotator(0.f,0.f,0.f),SpawnInfo);			
					SpawnedProjectile->Target = TargetPiece;
					SpawnedProjectile->Speed = Speed;
					Mesh->SetAnimInstanceClass(Mesh->GetAnimClass());
					SpawnedProjectile->OnImpact.AddDynamic(this, &UChessPieceController::AfterAttack);
				},
				Attack->GetPlayLength(), false
			);
		});
	}
}

void UChessPieceController::NotifyAttack()
{
	
	ThisPiece->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ThisPiece->GetMesh()->GetComponentLocation(), TargetPiece->ChessPieceController->Mesh->GetComponentLocation()));
	if (AttackMontage)
	{
		UAnimInstance* AnimInst = ThisPiece->GetMesh()->GetAnimInstance();
    
		AnimInst->Montage_Play(AttackMontage);
	}
}

void UChessPieceController::MeeleNotifyAttack()
{
	if (AttackMontage)
	{
		UAnimInstance* AnimInst = Mesh->GetAnimInstance();
    
		AnimInst->Montage_Play(AttackMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			AfterAttack();
		});

		AnimInst->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
}



FMovements UChessPieceController::GetAllPossibleMoves() 
{
	switch (Type)
	{
		case EPieceTypes::Pawn:
			return PawnPossibleMoves();
		case EPieceTypes::Rook:
			return RookPossibleMoves();
		case EPieceTypes::Knight:
			return KnightPossibleMoves();
		case EPieceTypes::Bishop:
			return BishopPossibleMoves();
		case EPieceTypes::Queen:
			return QueenPossibleMoves();
		case EPieceTypes::King:
			return KingPossibleMoves();
		default:
			break;
	}
	return FMovements{{},{}};
}

void UChessPieceController::SetAllPossibleMoves(const TArray<FString>& Moves, const TArray<FString>& Attacks)
{
	CurrentMoves = Moves;
	CurrentAttacks = Attacks;
}

void UChessPieceController::RefreshMoves()
{
	const FMovements Moves = GetAllPossibleMoves();
	SetAllPossibleMoves(Moves.Movements, Moves.Attacks);
}

TArray<FString> UChessPieceController::FindValidPath(const int32 Length, const EDirections Direction) const 
{
	TArray<FString> ValidPaths;
	if (const int32 EdgeLength = UHelperFunctions::GetEdgeLength(ThisPiece,Direction); FMath::Clamp(Length, 0, EdgeLength) > 0)
	{
		for (int i = 1; i < EdgeLength; ++i)
		{
			const FString Position = UHelperFunctions::GetPositionInDirection(i, ThisPiece, Direction);
			if (const FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(Position); !FoundPiece.PieceFound) ValidPaths.Add(Position);
			else
			{
				if (ThisPiece->Team == FoundPiece.Piece->Team) break;
				else
				{
					ValidPaths.Add(Position);
				}
				
			}
		}
	}
	return ValidPaths;
}

bool UChessPieceController::AttackCheck(const FString& Position) const
{
	if (const FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(Position); FoundPiece.PieceFound && FoundPiece.Team != ThisPiece->Team) return true;
	return false;
}

bool UChessPieceController::MovementCheck(const FString& Position) const
{
	if (const FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(Position); !FoundPiece.PieceFound) return true;
	return false;
}

FMovements UChessPieceController::PawnPossibleMoves() const
{
	FMovements Movements;
	const int32 Length = HasMoved? 1 : 2;
	for (FString Position : FindValidPath(Length, EDirections::Forward))
	{
		if (MovementCheck(Position)) Movements.Movements.Add(Position);
	}
	for (TArray AttackDirections = {EDirections::ForwardLeft, EDirections::ForwardRight}; const EDirections Direction : AttackDirections)
	{
		for (FString Position : FindValidPath(Length, Direction))
		{
			if (AttackCheck(Position)) Movements.Attacks.Add(Position);
		}
	}
	return Movements;
	
}

FMovements UChessPieceController::RookPossibleMoves() const
{
	TArray<FString> ValidPaths = {};
	for (TArray Directions = {EDirections::Forward, EDirections::Backward, EDirections::Left, EDirections::Right}; const EDirections Direction : Directions)
	{
		ValidPaths.Append(FindValidPath(UHelperFunctions::GetEdgeLength(ThisPiece, Direction), Direction));
	}
	return FMovements{ValidPaths, ValidPaths};
}

FMovements UChessPieceController::KnightPossibleMoves() const
{
	TArray<FString> ValidPaths = {};
	FString RF,RB,LF,LB;
	// Forward
	FString Position = UHelperFunctions::GetPositionInDirection(2, ThisPiece, EDirections::Forward);
	int32 Letter = FCString::Atoi(*UHelperFunctions::GetNumberFromLetter(UHelperFunctions::GetPositionLetter(Position)));
	FString Number = UHelperFunctions::GetPositionNumber(Position);
	FString FR = UHelperFunctions::GetLetterFromNumber(FString::FromInt(Letter + 1)) + Number;
	FString FL = UHelperFunctions::GetLetterFromNumber(FString::FromInt(Letter - 1)) + Number;
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(FR); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(FR);
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(FL); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(FL);
	// Backward
	Position = UHelperFunctions::GetPositionInDirection(2, ThisPiece, EDirections::Backward);
	Letter = FCString::Atoi(*UHelperFunctions::GetNumberFromLetter(UHelperFunctions::GetPositionLetter(Position)));
	Number = UHelperFunctions::GetPositionNumber(Position);
	FString BR = UHelperFunctions::GetLetterFromNumber(FString::FromInt(Letter + 1)) + Number;
	FString BL = UHelperFunctions::GetLetterFromNumber(FString::FromInt(Letter - 1)) + Number;
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(BR); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(BR);
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(BL); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(BL);
	// Right
	Position = UHelperFunctions::GetPositionInDirection(2, ThisPiece, EDirections::Right);
	FString Letter2 = UHelperFunctions::GetPositionLetter(Position);
	int32 Number2 = FCString::Atoi(*UHelperFunctions::GetPositionNumber(Position));
	RF = Letter2 + FString::FromInt(Number2 + 1);
	RB = Letter2 + FString::FromInt(Number2 - 1);
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(RF); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(RF);
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(RB); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(RB);
	// Left
	Position = UHelperFunctions::GetPositionInDirection(2, ThisPiece, EDirections::Left);
	Letter2 = UHelperFunctions::GetPositionLetter(Position);
	Number2 = FCString::Atoi(*UHelperFunctions::GetPositionNumber(Position));
	LF = Letter2 + FString::FromInt(Number2 + 1);
	LB = Letter2 + FString::FromInt(Number2 - 1);
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(LF); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(LF);
	if (const FFindPiece FP = ChessBoardReference->GetPositionInfo(LB); !FP.PieceFound || (FP.PieceFound && FP.Team != ThisPiece->Team)) ValidPaths.Add(LB);
	return FMovements{ValidPaths,ValidPaths};
}

FMovements UChessPieceController::BishopPossibleMoves() const
{
	TArray<FString> ValidPaths;
	for (TArray Directions = {EDirections::ForwardLeft, EDirections::ForwardRight, EDirections::BackwardLeft, EDirections::BackwardRight}; const EDirections Direction : Directions)
	{
		ValidPaths.Append(FindValidPath(UHelperFunctions::GetEdgeLength(ThisPiece, Direction), Direction));
	}
	return FMovements{ValidPaths,ValidPaths};
}

FMovements UChessPieceController::QueenPossibleMoves() const
{
	TArray<FString> ValidPaths;
	for (TArray Directions = {EDirections::Forward, EDirections::Backward, EDirections::Left, EDirections::Right, EDirections::ForwardLeft, EDirections::ForwardRight, EDirections::BackwardLeft, EDirections::BackwardRight}; const EDirections Direction : Directions)
	{
		ValidPaths.Append(FindValidPath(UHelperFunctions::GetEdgeLength(ThisPiece, Direction), Direction));
	}
	return FMovements{ValidPaths,ValidPaths};
}

FMovements UChessPieceController::KingPossibleMoves()
{
	CastleingPoints.Empty();
	TArray<FString> ValidPaths;
	for (TArray Directions = {EDirections::Forward, EDirections::Backward, EDirections::Left, EDirections::Right, EDirections::ForwardLeft, EDirections::ForwardRight, EDirections::BackwardLeft, EDirections::BackwardRight}; const EDirections Direction : Directions)
	{
		ValidPaths.Append(FindValidPath(1, Direction));
	}
	
	if (CanCastle(EDirections::Left))
	{
		CastleingPoints.Add(UHelperFunctions::GetPositionInDirection(ThisPiece->Team == ETeams::White ? 3 : 2,ThisPiece,EDirections::Left));
	}
	if (CanCastle(EDirections::Right))
	{
		CastleingPoints.Add(UHelperFunctions::GetPositionInDirection(ThisPiece->Team == ETeams::White ? 2 : 3,ThisPiece,EDirections::Left));
	}
	TArray<FString> CastlePoints;
	CastleingPoints.GenerateKeyArray(CastlePoints);
	ValidPaths.Append(CastlePoints);
	for (FString Pos : ValidPaths)
	{
		if (ChessBoardReference->IsPositionInDanger(Pos, ThisPiece->Team))
		{
			ValidPaths.Remove(Pos);
		}
	}
	if (IsCheckMate(ValidPaths))
	{
		//Implement GameOver
	}
	return FMovements{ValidPaths,ValidPaths};
}

FFindPiece UChessPieceController::CheckTargetTilesOccupied(UTileComponent* TargetTile)
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	ChessBoardReference->AllPieces.Remove(ThisPiece->Position);
	HasMoved = true;
	ThisPiece->Position = TargetTile->GetTileName();
	return ChessBoardReference->GetPositionInfo(ThisPiece->Position);
}

FVector UChessPieceController::GetMoveDestination(UTileComponent* TargetTile) const
{
	ChessBoardReference->AllPieces.Remove(ThisPiece->Position);
	ChessBoardReference->AllPieces.Add(TargetTile->GetTileName(), ThisPiece);
	return TargetTile->GetComponentLocation() + FVector(0.f,0.f,110.f);
}

FVector UChessPieceController::FindAtkPos(const AMasterPiece* AttackedPiece) const
{
	const FString AttackPosition = UHelperFunctions::GetPositionInDirection(1, AttackedPiece,EDirections::Forward);
	UTileComponent* AttackTile = ChessBoardReference->AllTiles[AttackPosition];
	return AttackTile->GetComponentLocation() + FVector(0.f,0.f,110.f);
}

bool UChessPieceController::CanCastle(EDirections Direction) const
{
	if (ChessBoardReference->IsKingInCheck()) return false;
	if (HasMoved) return false;
	switch (Direction)
	{
	case EDirections::Left:
		{
			FFindPiece RookPiece = ChessBoardReference->GetPositionInfo(UHelperFunctions::GetPositionInDirection(ChessBoardReference->PlayerCharacter->CurrentTeam == ETeams::White? 4 : 3, ThisPiece, EDirections::Left));
			if (!RookPiece.PieceFound || RookPiece.Piece->ChessPieceController->Type != EPieceTypes::Rook || RookPiece.Piece->ChessPieceController->HasMoved) return false;
			TArray<FString> ValidPaths = FindValidPath(2,EDirections::Left);
			if (ValidPaths.Num() > 1)
			{
				for (FString ValidPath : ValidPaths)
				{
					const FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(ValidPath);
					if (FoundPiece.PieceFound) return false;
					if (ChessBoardReference->IsPositionInDanger(ValidPath, ThisPiece->Team)) return false;
					
				}
				return true;
			}
			else
			{
				return false;
			}
			
		}
		case EDirections::Right:
		{
			FFindPiece RookPiece = ChessBoardReference->GetPositionInfo(UHelperFunctions::GetPositionInDirection(ChessBoardReference->PlayerCharacter->CurrentTeam == ETeams::White? 3 : 4, ThisPiece, EDirections::Right));
			if (!RookPiece.PieceFound || RookPiece.Piece->ChessPieceController->Type != EPieceTypes::Rook || RookPiece.Piece->ChessPieceController->HasMoved) return false;
			TArray<FString> ValidPaths = FindValidPath(2,EDirections::Right);
			if (ValidPaths.Num() > 1)
			{
				for (FString ValidPath : ValidPaths)
				{
					const FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(ValidPath);
					if (FoundPiece.PieceFound) return false;
					if (ChessBoardReference->IsPositionInDanger(ValidPath, ThisPiece->Team)) return false;
					
				}
				return true;
			}
			else
			{
				return false;
			}
			
		}
		default: return false;
	}
}

bool UChessPieceController::IsCheckMate(const TArray<FString>& ThreateningMoves) const
{
	if (ThreateningMoves.Num() == 0 && ChessBoardReference->IsKingInCheck())
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

bool UChessPieceController::CanPromote() const
{
	if (Type != EPieceTypes::Pawn) return false;
	if (ThisPiece->Team == ETeams::White && UHelperFunctions::GetPositionNumber(ThisPiece->Position) == "8") return true;
	if (ThisPiece->Team == ETeams::Black && UHelperFunctions::GetPositionNumber(ThisPiece->Position) == "1") return true;
	return false;
}

void UChessPieceController::Promote(EPieceTypes PromoteTo) const
{
	if (CanPromote())
	{
		switch (PromoteTo)
		{
		case EPieceTypes::Bishop:
		{
				PromotePiece(ChessInstance->WhiteRace.Bishop, ChessInstance->BlackRace.Bishop);
			break;
		}
		case EPieceTypes::Knight:
			{
				PromotePiece(ChessInstance->WhiteRace.Knight, ChessInstance->BlackRace.Knight);
				break;
			}
		case EPieceTypes::Rook:
			{
				PromotePiece(ChessInstance->WhiteRace.Rook, ChessInstance->BlackRace.Rook);
				break;
			}
		case EPieceTypes::Queen:
			{
				PromotePiece(ChessInstance->WhiteRace.Queen, ChessInstance->BlackRace.Queen);
				break;
			}
		default:break;
	}
	}
}

void UChessPieceController::PromotePiece(const TSubclassOf<AMasterPiece> White, const TSubclassOf<AMasterPiece> Black) const
{
	ChessBoardReference->Promote(ThisPiece->Position, ThisPiece, ThisPiece->Team == ETeams::White ? White : Black);
}


