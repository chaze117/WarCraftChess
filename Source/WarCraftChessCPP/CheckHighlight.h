// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckHighlight.generated.h"

UCLASS()
class WARCRAFTCHESSCPP_API ACheckHighlight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckHighlight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
