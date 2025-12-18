// Fill out your copyright notice in the Description page of Project Settings.


#include "SkySphere.h"

// Sets default values
ASkySphere::ASkySphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  SphereMesh =
  CreateDefaultSubobject<class UStaticMeshComponent>(TEXT("SphereMesh"));
  SphereMesh->SetupAttachment(RootComponent);
  static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
      TEXT("/Engine/MapTemplates/Sky/SM_SkySphere.SM_SkySphere"));
  if (MeshAsset.Succeeded()) {
    SphereMesh->SetStaticMesh(MeshAsset.Object);
  }
  static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatAsset(
      TEXT("/Engine/MapTemplates/Sky/M_Procedural_Sky_Daytime.M_Procedural_Sky_Daytime"));
  if (MatAsset.Succeeded()) {
    SkyMaterial = MatAsset.Object;
    SphereMesh->SetMaterial(0, SkyMaterial); // Назначаем материал на первый слот
  }
}

// Called when the game starts or when spawned
void ASkySphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkySphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

