// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerStart.h"
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AsyaKombatovaMainMode.generated.h"
/**
 * 
 */
UCLASS()
class ASYAKOMBATOVNA_API AAsyaKombatovaMainMode : public AGameMode
{
	GENERATED_BODY()
public:
  // Variables

  // Functions

  virtual void RestartPlayer(AController *NewPlayer) override;
  // Constructor
  AAsyaKombatovaMainMode();
 
  // Events
  
  // BeginPlay
  virtual void BeginPlay() override;
};
