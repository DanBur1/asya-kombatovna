// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkySphere.generated.h"

UCLASS()
class ASYAKOMBATOVNA_API ASkySphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkySphere();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent *SphereMesh;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
  UMaterialInterface *SkyMaterial;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
