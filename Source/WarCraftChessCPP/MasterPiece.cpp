// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterPiece.h"
#include "ChessBoard.h"
#include "Kismet/GameplayStatics.h"
#include "ChessCharacter.h"
#include "Components/ChessPieceController.h"

// Sets default values
AMasterPiece::AMasterPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
	GetMesh()->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mount = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mount"));
	Mount->SetupAttachment(RootComponent);
	Mount->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
	Mount->SetWorldScale3D(FVector(0.75f, 0.75f, 0.75f));
	
	ArmorMesh1  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorMesh1"));
	ArmorMesh2  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorMesh2"));
	Helm        = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Helm"));
	Buckle      = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buckle"));
	RShoulder   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RShoulder"));
	LShoulder   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LShoulder"));
	RHand       = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RHand"));
	LHand       = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LHand"));
	RBracer     = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RBracer"));
	LBracer     = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LBracer"));
	RKneepad    = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RKneepad"));
	LKneepad    = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LKneepad"));
	RToes       = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RToes"));
	LToes       = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LToes"));
	WeaponOnBack= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponOnBack"));
	
	
	ArmorMesh1->  SetupAttachment(GetMesh());
	ArmorMesh2->  SetupAttachment(GetMesh());
	Helm->        SetupAttachment(GetMesh());
	Buckle->      SetupAttachment(GetMesh());
	RShoulder->   SetupAttachment(GetMesh());
	LShoulder->   SetupAttachment(GetMesh());
	RHand->       SetupAttachment(GetMesh());
	LHand->       SetupAttachment(GetMesh());
	RBracer->     SetupAttachment(GetMesh());
	LBracer->     SetupAttachment(GetMesh());
	RKneepad->    SetupAttachment(GetMesh());
	LKneepad->    SetupAttachment(GetMesh());
	RToes->       SetupAttachment(GetMesh());
	LToes->       SetupAttachment(GetMesh());
	WeaponOnBack->SetupAttachment(GetMesh());
	
	ArmorMesh1->  SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmorMesh2->  SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Helm->        SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Buckle->      SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RShoulder->   SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LShoulder->   SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RHand->       SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LHand->       SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RBracer->     SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LBracer->     SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RKneepad->    SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LKneepad->    SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RToes->       SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	LToes->       SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponOnBack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(RootComponent);
	Widget->SetVisibility(false);
	Widget->SetRelativeTransform(FTransform(FRotator(90.f,0.f,0.f),FVector(0.f,0.f,-85.f),FVector(0.4f,0.4f,0.4f)));
	
	ChessPieceController = CreateDefaultSubobject<UChessPieceController>(TEXT("ChessPieceController"));
	
	
}

// Called when the game starts or when spawned
void AMasterPiece::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacterReference =  Cast<AChessCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}

// Called every frame
void AMasterPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Widget->IsVisible()) RotateSelection(DeltaTime,30.f);
}

void AMasterPiece::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!GetMesh()) return;
	
	const FAttachmentTransformRules AttachRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::KeepRelative,
	
	true);
	ArmorMesh1->SetLeaderPoseComponent(GetMesh(),false,false);
	ArmorMesh2->SetLeaderPoseComponent(GetMesh(),false,false);
	if (Mount->DoesSocketExist(TEXT("Saddle")))
		GetMesh()-> AttachToComponent(Mount, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Saddle"));
	
	Helm->        AttachToComponent(GetMesh(), AttachRules, TEXT("Helm"));
	Buckle->      AttachToComponent(GetMesh(), AttachRules, TEXT("Buckle"));
	RShoulder->   AttachToComponent(GetMesh(), AttachRules, TEXT("RShoulder"));
	LShoulder->   AttachToComponent(GetMesh(), AttachRules, TEXT("LShoulder"));
	RHand->       AttachToComponent(GetMesh(), AttachRules, TEXT("RHand"));
	LHand->       AttachToComponent(GetMesh(), AttachRules, TEXT("LHand"));
	RBracer->     AttachToComponent(GetMesh(), AttachRules, TEXT("RBracer"));
	LBracer->     AttachToComponent(GetMesh(), AttachRules, TEXT("LBracer"));
	RKneepad->	  AttachToComponent(GetMesh(), AttachRules, TEXT("RKneepad"));
	LKneepad->    AttachToComponent(GetMesh(), AttachRules, TEXT("LKneepad"));
	RToes->       AttachToComponent(GetMesh(), AttachRules, TEXT("RToes"));
	LToes->       AttachToComponent(GetMesh(), AttachRules, TEXT("LToes"));
	WeaponOnBack->AttachToComponent(GetMesh(), AttachRules, TEXT("WeaponOnBack"));
	
}

void AMasterPiece::Initalize(const ETeams InTeam, FString InPosition, class AChessBoard* InChessBoardReference)
{
	Team = InTeam;
	Position = InPosition;
	ChessBoardReference = InChessBoardReference;
	if (ChessPieceController)
		ChessPieceController->ChessBoardReference = InChessBoardReference;
}

void AMasterPiece::SetSelected(bool bNewSelected)
{
	bIsSelected = bNewSelected;
	if (Widget)
	{
		Widget->SetVisibility(bIsSelected);
	}
}


void AMasterPiece::NotifyActorOnClicked(FKey ButtonPressed)
{
	if (!ChessBoardReference || Team != PlayerCharacterReference->CurrentTeam) return;
	
	if (ChessBoardReference->CurrentPiece && ChessBoardReference->CurrentPiece != this)
	{
		ChessBoardReference->CurrentPiece->SetSelected(false);
	}
	
	ChessBoardReference->CurrentPiece = this;
	SetSelected(true);
	TArray<FString> ValidPoints;
	ValidPoints.Append(ChessPieceController->CurrentMoves);
	ValidPoints.Append(ChessPieceController->CurrentAttacks);
	ChessBoardReference->OnPieceSelected(this,ValidPoints);
	
}
void AMasterPiece::RotateSelection(const float Delta, const float Speed) const
{
	Widget->SetWorldRotation(Widget->GetComponentRotation()+ FRotator(0.f,Delta*Speed,0.f));
}