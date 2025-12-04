// Fill out your copyright notice in the Description page of Project Settings.


#include "WarCraftChessCPP/Components/TileComponent.h"

UTileComponent::UTileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionResponseToAllChannels(ECR_Block);
	SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // Click trace ezt használja

	SetGenerateOverlapEvents(true);
	bReturnMaterialOnMove = true;

	// Hogy a komponens kaphasson inputot:
	bSelectable = true;
}
void UTileComponent::BeginPlay()
{
	Super::BeginPlay();

	OnClicked.AddDynamic(this, &UTileComponent::OnTileClicked);
}

void UTileComponent::OnTileClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed)
{
	
}
