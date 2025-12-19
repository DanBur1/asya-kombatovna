// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <EnhancedInputSubsystems.h>
#include <InputMappingContext.h>
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "PlayerJet.h"
#include "Pilot.generated.h"


/**
 * 
 */
UCLASS()
class ASYAKOMBATOVNA_API APilot : public APlayerController
{
	GENERATED_BODY()
public:

    // Enumerations
  //UENUM(BlueprintType)
  enum class ECameraMode : uint8 {
    First UMETA(DisplayName = "First Person"),
    Third UMETA(DisplayName = "Third Person")
  };

  // Constants

  // Speed used for camera movement interpolation
  const float camera_speed = 5;
  // Time it takes for the game to recognize button holding
  const float hold_threshold = 0.5f;

  // Variables

  // Checks whether it should be following the target it locked on
  bool follow_target;
  // Camera mode
  ECameraMode camera;
  float camera_x;
  float camera_y;
  // Player references for different classes
  APawn *ControlledPawn;
  APlayerJet *ControlledJet;
  // Handle button holding
  FTimerHandle HoldTimerHandle;
  // HUD
  UUserWidget *HUDWidget = NULL;
  TSubclassOf<UUserWidget> HUDWidgetClass;
  UUserWidget *PauseWidget = NULL;
  TSubclassOf<UUserWidget> PauseWidgetClass;
  // Inputs
  // IA_Camera
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *CameraAction;
  // IA_Guns
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *GunsAction;
  // IA_Launch
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *LaunchAction;
  // IA_Landing
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *LandingAction;
  // IA_Lock
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *LockAction;
  // IA_Pause
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *PauseAction;
  // IA_Pitch
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *PitchAction;
  // IA_Roll
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *RollAction;
  // IA_Thrust
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *ThrustAction;
  // IA_Yaw
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputAction *YawAction;

  // Function
  
  // Constructor
  APilot();
  // Functions for camera controls
  // Turns controller object in its original position returning the camera behind the plane
  void defaultRotation();
  // Turns controller in a way that makes camera point at whatever player is pointing
  void followControls();
  // Turns controller in way that makes camera point at the direction player moves
  void followMovement();
  // Turns controller to face the target
  void followTarget();
  // Standart function to set inputs
  virtual void SetupInputComponent() override;
  // Reaction to IA_Camera
  void OnCameraAction(const FInputActionValue &Value);
  // Reaction to IA_Guns Starting
  void OnGunsActionStarted();
  // Reaction to IA_Guns ending
  void OnGunsActionCompleted();
  // Reaction to IA_Launch
  void OnLaunchAction();
  // Reaction to IA_Landing Started
  void OnLandingAction();
  // Reaction to IA_Lock Started
  void OnLockActionStart();
  // Reaction to IA_Lock Ended
  void OnLockActionEnd();
  // Reaction to IA_Pause
  void OnPauseAction();
  // Reaction to IA_Pitch
  void OnPitchAction(const FInputActionValue &Value);
  // Reaction to IA_Pitch completed
  void onPitchActionEnded();
  // Reaction to IA_Roll
  void OnRollAction(const FInputActionValue &Value);
  // Reaction to IA_Roll completed
  void onRollActionEnded();
  // Reaction to IA_Thrust
  void OnThrustAction(const FInputActionValue &Value);
  // Reaction to IA_Yaw triggered
  void OnYawAction(const FInputActionValue &Value);
  // Reaction to IA_Yaw completed
  void onYawActionEnded();
  // Bruh are you for real? a function that literally sets one bool as T that's cringe af bruh
  void startFollowingTarget();

  // Events
  
  // Begin play
  virtual void BeginPlay() override;
  // On posses
  virtual void OnPossess(APawn *InPawn) override;
  // Tick
  virtual void Tick(float DeltaTime) override;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  UInputMappingContext *InputMappingContext;
};
