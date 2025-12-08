// Fill out your copyright notice in the Description page of Project Settings.


#include "WarCraftChessCPP/Components/ChessPieceController.h"
#include "WarCraftChessCPP/ChessBoard.h"
#include "WarCraftChessCPP/HelperFunctions.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Kismet/GameplayStatics.h"
#include "WarCraftChessCPP/MasterPiece.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "WarCraftChessCPP/ChessAIController.h"
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
	ThisPiece->PromotionWidgetClass = ChessInstance->PromoteWidget->GetClass();
	
	
}


// Called every frame
void UChessPieceController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UChessPieceController::Move(UTileComponent* TargetTile)
{
	ChessBoardReference->ClearMoveHighLights();
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

void UChessPieceController::MoveWithoutAttack()
{
	if (!ThisPiece) return;
	MoveType = EMoveTypes::MoveWithoutAttack;
	if (AChessAIController* AIController = Cast<AChessAIController>(ThisPiece->GetController()))
	{
		CurrentMoveDestination = GetMoveDestination(Tile);
		// Bind the delegate
		FAIRequestID MoveRequestID = AIController->MoveToLocation(CurrentMoveDestination);
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UChessPieceController::OnMoveCompleted);
		AIController->ReceiveMoveCompleted.AddDynamic(this, &UChessPieceController::OnMoveCompleted);
	}
}

// Ez hívódik, amikor a mozgás befejeződik
void UChessPieceController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!ThisPiece) return;
	ThisPiece->Position = Tile->GetTileName();
	SetFinalRotationLocked(); // rotáció beállítása
	if (MoveType == EMoveTypes::FindAttackPosition)
	{
		switch (AttackType)
		{
		case EAttackTypes::Meele:
			{
				DoMeeleAttack();
				break;
			}
		case EAttackTypes::MeeleNotify:
			{
				MeeleNotifyAttack();
				break;
			}
		}
	}
	if (CanPromote() && (MoveType == EMoveTypes::AfterAttack || MoveType == EMoveTypes::MoveWithoutAttack)) 
	{
		if (ThisPiece->PromotionWidgetClass)
		{
			//Mesh->SetAnimInstanceClass(Mesh->GetAnimClass());
			PromoteWidget = CreateWidget<UUserWidget>(GetWorld(), ThisPiece->PromotionWidgetClass);
			PromoteWidget->AddToViewport();
		}
	}
	if ((MoveType == EMoveTypes::AfterAttack || MoveType == EMoveTypes::MoveWithoutAttack) && !CanPromote())
	{
		MoveType = EMoveTypes::CanEndTurn;
	}

	if (ChessBoardReference && MoveType == EMoveTypes::CanEndTurn)
	{
		
		ChessBoardReference->PieceMoved(ThisPiece->Team);
	}
}
void UChessPieceController::FindAttackPosition(const AMasterPiece* Target)
{
	if (!ThisPiece) return;

	if (AChessAIController* AIController = Cast<AChessAIController>(ThisPiece->GetController()))
	{
		CurrentMoveDestination = FindAtkPos(Target);
		MoveType = EMoveTypes::FindAttackPosition;
		// Bind the delegate
		FAIRequestID MoveRequestID = AIController->MoveToLocation(CurrentMoveDestination);
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UChessPieceController::OnMoveCompleted);
		AIController->ReceiveMoveCompleted.AddDynamic(this, &UChessPieceController::OnMoveCompleted);
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
					
					MoveType = EMoveTypes::AfterAttack;
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
	USkeletalMeshComponent* LocalMesh;
	if (ThisPiece->Mount && Type == EPieceTypes::Knight) LocalMesh= ThisPiece->Mount;
	else LocalMesh	 = ThisPiece->GetMesh();
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
	const int32 EdgeLength = UHelperFunctions::GetEdgeLength(ThisPiece,Direction);
	const int32 ClampedLength = FMath::Clamp(Length, 0, EdgeLength);
	if ( ClampedLength > 0)
	{

		for (int i = 1; i <= ClampedLength; ++i)
		{
			const FString Position = UHelperFunctions::GetPositionInDirection(i, ThisPiece, Direction);
			if (const FFindPiece FoundPiece = ChessBoardReference->GetPositionInfo(Position); !FoundPiece.PieceFound) ValidPaths.Add(Position);
			else
			{
				if (ThisPiece->Team == FoundPiece.Piece->Team) break;
				ValidPaths.Add(Position);
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
		for (FString Position : FindValidPath(1, Direction))
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
	TArray<FString> ValidPaths;

	// Knight offsetek
	const TArray<FIntPoint> Offsets = {
		{  1,  2 }, { -1,  2 },
		{  1, -2 }, { -1, -2 },
		{  2,  1 }, {  2, -1 },
		{ -2,  1 }, { -2, -1 }
	};

	FString CurrentLetter = ThisPiece->Position.Left(1);
	int32 CurrentNumber = FCString::Atoi(*ThisPiece->Position.Right(1));

	for (const FIntPoint& Off : Offsets)
	{
		int32 NLetter = FCString::Atoi(*UHelperFunctions::GetNumberFromLetter(CurrentLetter)) + Off.X;
		int32 NNumber = CurrentNumber + Off.Y;

		FString Target =
			UHelperFunctions::GetLetterFromNumber(FString::FromInt(NLetter))
			+ FString::FromInt(NNumber);

		if (!IsValidBoardCoordinate(Target))
			continue;

		const FFindPiece FP = ChessBoardReference->GetPositionInfo(Target);

		if (!FP.PieceFound || FP.Team != ThisPiece->Team)
			ValidPaths.Add(Target);
	}

	return FMovements{ ValidPaths, ValidPaths };
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

    // 1 mezős mozgások
    TArray<EDirections> Directions = {
        EDirections::Forward, EDirections::Backward,
        EDirections::Left, EDirections::Right,
        EDirections::ForwardLeft, EDirections::ForwardRight,
        EDirections::BackwardLeft, EDirections::BackwardRight
    };

    for (const EDirections Direction : Directions)
    {
        ValidPaths.Append(FindValidPath(1, Direction));
    }

    // --- CASTLING LOGIC ---
    // RIGHT (king side) -> king moves 2 to the right -> target = GetPositionInDirection(2, Right)
	if (CanCastle(EDirections::Right))
	{
		FString OneRight = UHelperFunctions::GetPositionInDirection(1, ThisPiece, EDirections::Right);
		FString TwoRight = UHelperFunctions::GetPositionInDirection(2, ThisPiece, EDirections::Right);

		if (!OneRight.IsEmpty() && !TwoRight.IsEmpty())
		{
			const bool bOneSafe = !ChessBoardReference->IsPositionInDanger(OneRight, ThisPiece->Team);
			const bool bTwoSafe = !ChessBoardReference->IsPositionInDanger(TwoRight, ThisPiece->Team);

			const bool bOneEmpty = !ChessBoardReference->AllPieces.Contains(OneRight);
			const bool bTwoEmpty = !ChessBoardReference->AllPieces.Contains(TwoRight);

			if (bOneSafe && bTwoSafe && bOneEmpty && bTwoEmpty)
			{
				ValidPaths.Add(TwoRight);
				CastleingPoints.Add(TwoRight, EDirections::Right);
			}
		}
	}

    // LEFT (queen side) -> king moves 2 to the left -> target = GetPositionInDirection(2, Left)
	if (CanCastle(EDirections::Left))
	{
		FString OneLeft = UHelperFunctions::GetPositionInDirection(1, ThisPiece, EDirections::Left);
		FString TwoLeft = UHelperFunctions::GetPositionInDirection(2, ThisPiece, EDirections::Left);

		if (!OneLeft.IsEmpty() && !TwoLeft.IsEmpty())
		{
			const bool bOneSafe = !ChessBoardReference->IsPositionInDanger(OneLeft, ThisPiece->Team);
			const bool bTwoSafe = !ChessBoardReference->IsPositionInDanger(TwoLeft, ThisPiece->Team);

			const bool bOneEmpty = !ChessBoardReference->AllPieces.Contains(OneLeft);
			const bool bTwoEmpty = !ChessBoardReference->AllPieces.Contains(TwoLeft);

			if (bOneSafe && bTwoSafe && bOneEmpty && bTwoEmpty)
			{
				ValidPaths.Add(TwoLeft);
				CastleingPoints.Add(TwoLeft, EDirections::Left); // ← helyesen
			}
		}
	}

    // Append castle positions to valid paths (already added above), remove danger positions if any slipped in
    // But since we already checked danger for intermediate squares, this is mostly redundant.
    // Re-check to be safe:
    TArray<FString> FilteredPaths;
    for (const FString& Pos : ValidPaths)
    {
        if (!ChessBoardReference->IsPositionInDanger(Pos, ThisPiece->Team))
        {
            FilteredPaths.Add(Pos);
        }
    }

    if (IsCheckMate(FilteredPaths))
    {
        // Implement GameOver
    }

    return FMovements{FilteredPaths, FilteredPaths};
}


FFindPiece UChessPieceController::CheckTargetTilesOccupied(UTileComponent* TargetTile)
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	ChessBoardReference->AllPieces.Remove(ThisPiece->Position);
	HasMoved = true;
	return ChessBoardReference->GetPositionInfo(TargetTile->GetName());
}

FVector UChessPieceController::GetMoveDestination(UTileComponent* TargetTile) const
{
	ChessBoardReference->AllPieces.Remove(ThisPiece->Position);
	ChessBoardReference->AllPieces.Add(TargetTile->GetTileName(), ThisPiece);
	return TargetTile->GetComponentLocation() + FVector(0.f ,0.f,112.318839f);

	
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
		PromoteWidget->RemoveFromParent();
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

bool UChessPieceController::IsValidBoardCoordinate(const FString& Coord) const
{
	if (Coord.Len() != 2)
		return false;

	TCHAR Letter = Coord[0];
	TCHAR Number = Coord[1];

	// Betu A–H
	if (Letter < 'A' || Letter > 'H')
		return false;

	// Szam 1–8
	if (Number < '1' || Number > '8')
		return false;

	return true;
}
void UChessPieceController::SetFinalRotationLocked() const
{
	if (!ThisPiece) return;
	FRotator FinalRot = ThisPiece->Team == ETeams::White
	? FRotator(0, 180, 0)
	: FRotator(0, 0, 0);
	// Tick 0 - azonnali set
	ThisPiece->SetActorRotation(FinalRot);

	// Tick 1
	FTimerHandle Timer1;
	ThisPiece->GetWorld()->GetTimerManager().SetTimer(
		Timer1,
		FTimerDelegate::CreateLambda([this, FinalRot]()
		{
			if (ThisPiece)
				ThisPiece->SetActorRotation(FinalRot);

			// Tick 2
			FTimerHandle Timer2;
			ThisPiece->GetWorld()->GetTimerManager().SetTimer(
				Timer2,
				FTimerDelegate::CreateLambda([this, FinalRot]()
				{
					if (ThisPiece)
						ThisPiece->SetActorRotation(FinalRot);

				}),
				0.01f, false
			);
		}),
		0.01f, false
	);
}