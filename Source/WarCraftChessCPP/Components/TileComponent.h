// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "WarCraftChessCPP/Enums.h"
#include "TileComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Chess), meta=(BlueprintSpawnableComponent))
class WARCRAFTCHESSCPP_API UTileComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTileClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed);

public:
	UTileComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileName TileName;
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FString GetTileName()
	{
		return StaticEnum<ETileName>()->GetNameStringByValue(static_cast<int64>(TileName));

	}
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FString GetTileLetter()
	{
		return StaticEnum<ETileName>()->GetNameStringByValue(static_cast<int64>(TileName)).Left(1);
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetTileNumber()
	{
		return StaticEnum<ETileName>()->GetNameStringByValue(static_cast<int64>(TileName)).Right(1);
	}
};
