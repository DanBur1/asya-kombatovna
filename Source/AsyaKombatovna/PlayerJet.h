// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AKPawnInterface.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "Jet.h"
#include "PlayerJet.generated.h"


/**
 * 
 */
UCLASS()
class ASYAKOMBATOVNA_API APlayerJet : public AJet{
	GENERATED_BODY()
  public:
    // Unreal generated events

     APlayerJet();
    void Tick(float DeltaTime) override;
     void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Interfaces

    virtual bool isPlayer_Implementation() override;
  protected:
    // Variables

    // Camera boom
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent *SpringArm;
    // Player camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent *PlayerCameraComponent;
    // Dead zone for camera following the movement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxCameraOffset = 600.f;
    // Smoothens the camera following the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraLerpSpeed = 500.f;
};
