// Fill out your copyright notice in the Description page of Project Settings.

#include "AsyaKombatovaMainMode.h"
#include "PlayerJet.h"
#include "Pilot.h"

//Constructor
AAsyaKombatovaMainMode::AAsyaKombatovaMainMode() {
  //Sets PlayerJet as our player
  DefaultPawnClass = APlayerJet::StaticClass();
  //Sets PlayerController as controller
  PlayerControllerClass = APilot::StaticClass();
}

void AAsyaKombatovaMainMode::RestartPlayer(AController *NewPlayer) {
    if (NewPlayer == nullptr || NewPlayer->GetPawn())
      return;

    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;
    APawn *NewPawn = nullptr;

    AActor *PlayerStartActor = FindPlayerStart(NewPlayer);
    if (PlayerStartActor) {
      APlayerStart *PlayerStart = Cast<APlayerStart>(PlayerStartActor);
      if (PlayerStart) {
        SpawnLocation = PlayerStart->GetActorLocation();
        SpawnRotation = PlayerStart->GetActorRotation();
      } else {
        SpawnLocation = PlayerStartActor->GetActorLocation();
        SpawnRotation = PlayerStartActor->GetActorRotation();
      }
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation,
                                            SpawnRotation, SpawnParams);

    if (NewPawn) {
      NewPlayer->Possess(NewPawn);
    }
  }

void AAsyaKombatovaMainMode::BeginPlay() {
  Super::BeginPlay();
}