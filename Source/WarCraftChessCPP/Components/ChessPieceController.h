// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WarCraftChessCPP/ChessInstance.h"
#include "WarCraftChessCPP/MasterPiece.h"
#include "WarCraftChessCPP/Projectile.h"
#include "WarCraftChessCPP/Components/TileComponent.h"
#include "ChessPieceController.generated.h"


UCLASS( ClassGroup=(Chess), meta=(BlueprintSpawnableComponent) )
class WARCRAFTCHESSCPP_API UChessPieceController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChessPieceController();
	
	UPROPERTY()
	UTileComponent* Tile;
	
	UPROPERTY()
	USkeletalMeshComponent* Mesh;
	
	UPROPERTY()
	TArray<FString> CurrentMoves;
	
	UPROPERTY()
	TArray<FString> CurrentAttacks;
	
	UPROPERTY()
	AMasterPiece* ThisPiece;
	
	UPROPERTY()
	class AChessBoard* ChessBoardReference;
	
	UPROPERTY()
	bool HasMoved = false;
	
	UPROPERTY()
	AMasterPiece* TargetPiece;
	
	UPROPERTY()
	TMap<FString, EDirections> CastleingPoints;
	
	UPROPERTY()
	UChessInstance* ChessInstance;
	
	UPROPERTY()
	bool IsAI;
	
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Types", meta=(DisplayPriority=-10))
	EPieceTypes Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Types", DisplayName="Attack Type", meta=(DisplayPriority=-10))
	EAttackTypes AttackType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Animations", meta=(DisplayPriority=-10))
	UAnimationAsset* Attack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Animations", meta=(DisplayPriority=-10))
	UAnimationAsset* Death;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Animations",DisplayName="Pre-Attack", meta=(DisplayPriority=-10))
	UAnimationAsset* PreAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Animation Montages",DisplayName="Attack Montage", meta=(DisplayPriority=-10))
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Animation Montages",DisplayName="Death Montage", meta=(DisplayPriority=-10))
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Projectile", meta=(DisplayPriority=-10))
	TSubclassOf<AProjectile> Projectile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Projectile",DisplayName="Projectile Socket Name", meta=(DisplayPriority=-10))
	FName ProjectileSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings.Projectile",DisplayName="Projectile Speed", meta=(DisplayPriority=-10))
	float ProjectileSpeed;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma region MovementFunctions
	UFUNCTION(BlueprintCallable)
	void Move(UTileComponent* TargetTile);
	
	UFUNCTION()
	void MoveWithoutAttack() const;
	
	UFUNCTION()
	void FindAttackPosition(const AMasterPiece* Target) const;
	
	UFUNCTION()
	void DoMeeleAttack();
	
	UFUNCTION()
	void AfterAttack();
	
	UFUNCTION()
	void GetKilled() const;
	
	UFUNCTION()
	void DoRangedAttack() ;
	
	UFUNCTION()
	void ShootProjectile(const AMasterPiece* Target, FName SocketName, TSubclassOf<AProjectile> ProjectileClass, float Speed);
	
	UFUNCTION()
	void NotifyAttack();
	
	UFUNCTION()
	void MeeleNotifyAttack();
	
#pragma endregion 	
	
#pragma  region  CalculationFunctions
	UFUNCTION()
	FMovements GetAllPossibleMoves();
	
	UFUNCTION()
	void SetAllPossibleMoves(const TArray<FString>& Moves, const TArray<FString>& Attacks);
	
	UFUNCTION()
	void RefreshMoves();
	
	UFUNCTION()
	TArray<FString> FindValidPath(int32 Length, EDirections Direction) const;
	
	UFUNCTION()
	bool AttackCheck(const FString& Position) const;
	
	UFUNCTION()
	bool MovementCheck(const FString& Position) const;
	
	UFUNCTION()
	FMovements PawnPossibleMoves() const;
	
	UFUNCTION()
	FMovements RookPossibleMoves() const;
	
	UFUNCTION()
	FMovements KnightPossibleMoves() const;
	
	UFUNCTION()
	FMovements BishopPossibleMoves() const;
	
	UFUNCTION()
	FMovements QueenPossibleMoves() const;
	
	UFUNCTION()
	FMovements KingPossibleMoves();
	
	UFUNCTION()
	FFindPiece CheckTargetTilesOccupied(UTileComponent* TargetTile);
	
	UFUNCTION()
	FVector GetMoveDestination(UTileComponent* TargetTile) const;
	
	UFUNCTION()
	FVector FindAtkPos(const AMasterPiece* AttackedPiece) const;
	
	UFUNCTION()
	bool CanCastle(EDirections Direction) const;
	
	UFUNCTION()
	bool IsCheckMate(const TArray<FString>& ThreateningMoves) const;
	
	UFUNCTION(BlueprintCallable)
	bool CanPromote() const;
	
	UFUNCTION(BlueprintCallable)
	void Promote(EPieceTypes PromoteTo) const;
	
	UFUNCTION()
	void PromotePiece(TSubclassOf<AMasterPiece> White, TSubclassOf<AMasterPiece> Black) const;
#pragma endregion
};
