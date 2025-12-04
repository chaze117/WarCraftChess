// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "MasterPiece.generated.h"

UCLASS()
class WARCRAFTCHESSCPP_API AMasterPiece : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterPiece();

	virtual void Tick(float DeltaTime) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPieceTypes Type;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Mount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ArmorMesh1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ArmorMesh2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Buckle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Helm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RShoulder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LShoulder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RBracer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LBracer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RKneepad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LKneepad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RToes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LToes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponOnBack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* Widget ;
	
	UPROPERTY()
	ETeams Team;
	
	UPROPERTY()
	FString Position;
	
	UPROPERTY()
	class AChessBoard* ChessBoardReference;
	
	UPROPERTY()
	bool bIsSelected = false;
	
	UPROPERTY()
	class AChessCharacter* PlayerCharacterReference;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UChessPieceController* ChessPieceController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> PromotionWidgetClass;
	
	UFUNCTION()
	void Initalize(ETeams InTeam, FString InPosition, class AChessBoard* InChessBoardReference);
	
	UFUNCTION()
	void SetSelected(const bool bSelected);
	
	UFUNCTION()
	void RotateSelection(float Delta, float Speed = 30.f) const;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	

};
