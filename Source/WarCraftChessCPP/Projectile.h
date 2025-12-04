// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterPiece.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjectileImpact);
UCLASS()
class WARCRAFTCHESSCPP_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	
	UPROPERTY()
	AMasterPiece* Target;

	UPROPERTY()
	float Speed;
	
	UPROPERTY(BlueprintAssignable, Category="Projectile")
	FOnProjectileImpact OnImpact;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
