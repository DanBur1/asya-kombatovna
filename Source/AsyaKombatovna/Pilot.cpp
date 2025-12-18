// Fill out your copyright notice in the Description page of Project Settings.


#include "Pilot.h"

APilot::APilot() {
  PrimaryActorTick.bCanEverTick = true;
  // Setup mapping context
  static ConstructorHelpers::FObjectFinder<UInputMappingContext>
      IMCObject(
      TEXT("/Game/Input/IMC_AKMainContol.IMC_AKMainContol"));
  UE_LOG(LogTemp, Warning, TEXT("MAP!"));
  if (IMCObject.Succeeded()) {
    InputMappingContext = IMCObject.Object;
    UE_LOG(LogTemp, Warning, TEXT("YAY!^_^"));
  }
  // Bind action asset to action class

  // Turn camera
  static ConstructorHelpers::FObjectFinder<UInputAction> CameraActionObj(
      TEXT("/Game/Input/Actions/IA_Camera.IA_Camera"));
  if (CameraActionObj.Succeeded()) {
    CameraAction = CameraActionObj.Object;
  }
  // Fire guns
  static ConstructorHelpers::FObjectFinder<UInputAction> GunsActionObj(
      TEXT("/Game/Input/Actions/IA_Guns.IA_Guns"));
  if (GunsActionObj.Succeeded()) {
    GunsAction = GunsActionObj.Object;
  }
  // Launch missile
  static ConstructorHelpers::FObjectFinder<UInputAction> LaunchActionObj(
      TEXT("/Game/Input/Actions/IA_Launch.IA_Launch"));
  if (LaunchActionObj.Succeeded()) {
    LaunchAction = LaunchActionObj.Object;
  }
  // Landing
  static ConstructorHelpers::FObjectFinder<UInputAction> LandingActionObj(
      TEXT("/Game/Input/Actions/IA_Landing.IA_Landing"));
  if (LandingActionObj.Succeeded()) {
    LandingAction = LandingActionObj.Object;
  }
  // Lock
  static ConstructorHelpers::FObjectFinder<UInputAction> LockActionObj(
      TEXT("/Game/Input/Actions/IA_Lock.IA_Lock"));
  if (LockActionObj.Succeeded()) {
    LockAction = LockActionObj.Object;
  }
  // Pause
  static ConstructorHelpers::FObjectFinder<UInputAction> PauseActionObj(
      TEXT("/Game/Input/Actions/IA_Pause.IA_Pause"));
  if (PauseActionObj.Succeeded()) {
    PauseAction = PauseActionObj.Object;
  }
  // Pitch
  static ConstructorHelpers::FObjectFinder<UInputAction> PitchActionObj(
      TEXT("/Game/Input/Actions/IA_Pitch.IA_Pitch"));
  if (PitchActionObj.Succeeded()) {
    PitchAction = PitchActionObj.Object;
    UE_LOG(LogTemp, Warning, TEXT("IN ASS WE PITCH!"));
  }
  // Roll
  static ConstructorHelpers::FObjectFinder<UInputAction> RollActionObj(
      TEXT("/Game/Input/Actions/IA_Roll.IA_Roll"));
  if (RollActionObj.Succeeded()) {
    RollAction = RollActionObj.Object;
    UE_LOG(LogTemp, Warning, TEXT("IN ASS WE ROLL!"));
  }
  // Thrust
  static ConstructorHelpers::FObjectFinder<UInputAction> ThrustActionObj(
      TEXT("/Game/Input/Actions/IA_Thrust.IA_Thrust"));
  if (ThrustActionObj.Succeeded()) {
    ThrustAction = ThrustActionObj.Object;
    UE_LOG(LogTemp, Warning, TEXT("IN ASS WE THRUST!"));
  }
  static ConstructorHelpers::FObjectFinder<UInputAction> YawActionObj(
      TEXT("/Game/Input/Actions/IA_Yaw.IA_Yaw"));
  if (YawActionObj.Succeeded()) {
    YawAction = YawActionObj.Object;
    UE_LOG(LogTemp, Warning, TEXT("IN ASS WE YAW!"));
  }
  // Other actions

  // Find widget for pause action
  static ConstructorHelpers::FClassFinder<UUserWidget> PauseWidgetBPClass(
      TEXT("/Game/UI/WBP_Pause"));
  if (PauseWidgetBPClass.Succeeded()) {
    PauseWidgetClass = PauseWidgetBPClass.Class;
  }
  // Find widget for HUD
  static ConstructorHelpers::FClassFinder<UUserWidget> HudWidgetBPClass(
      TEXT("/Game/UI/WBP_Hud"));
  if (HudWidgetBPClass.Succeeded()) {
    HUDWidgetClass = HudWidgetBPClass.Class;
    UE_LOG(LogTemp, Warning, TEXT("HUD!"));
  }
}

void APilot::SetupInputComponent() {
  Super::SetupInputComponent();
  UE_LOG(LogTemp, Warning, TEXT("%s"), *PauseAction->GetPathName());
  UE_LOG(LogTemp, Warning, TEXT("%s"), *ThrustAction->GetPathName());
  // Bind action class to function
  if (UEnhancedInputComponent *EnhancedInputComponent =
          Cast<UEnhancedInputComponent>(InputComponent)) {
    if (CameraAction) {
      EnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Triggered,
                                         this, &APilot::OnCameraAction);
    }
    if (GunsAction) {
      EnhancedInputComponent->BindAction(GunsAction, ETriggerEvent::Started,
                                         this, &APilot::OnGunsActionStarted);
      EnhancedInputComponent->BindAction(GunsAction, ETriggerEvent::Completed,
                                         this, &APilot::OnGunsActionCompleted);
    }
    if (LaunchAction) {
      EnhancedInputComponent->BindAction(LaunchAction, ETriggerEvent::Started,
                                         this, &APilot::OnLaunchAction);
    }
    if (LandingAction) {
      EnhancedInputComponent->BindAction(LandingAction, ETriggerEvent::Started,
                                         this, &APilot::OnLandingAction);
    }
    if (LockAction) {
      EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started,
                                         this, &APilot::OnLockActionStart);
      EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Completed,
                                         this, &APilot::OnLockActionEnd);
    }
    if (PauseAction) 
    {
      EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started,
                                         this,
                                         &APilot::OnPauseAction);
    }
    if (PitchAction) {
      EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered,
                                         this, &APilot::OnPitchAction);
    }
    if (RollAction) {
      EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered,
                                         this, &APilot::OnRollAction);
    }
    if (ThrustAction) {
      EnhancedInputComponent->BindAction(ThrustAction, ETriggerEvent::Triggered,
                                         this, &APilot::OnThrustAction);
    }
    if (YawAction) {
      EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered,
                                         this, &APilot::OnYawAction);
    }
  }
}

void APilot::BeginPlay() {
  Super::BeginPlay();
  // Adds IMC_AKMainControll
  if (ULocalPlayer *LocalPlayer = GetLocalPlayer()) {
    if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                LocalPlayer)) {
      Subsystem->AddMappingContext(InputMappingContext, 0);
    }
  }
  // Creates HUD
  if (HUDWidgetClass) {
    HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
    if (HUDWidget) {
      HUDWidget->AddToViewport();
    }
  }
}

void APilot::defaultRotation() {
  if (ControlledJet) {
    FRotator CurrentRotation = GetControlRotation();
    FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation, ControlledJet->VectorOfAttack.Rotation(),
                         GetWorld()->GetDeltaSeconds(), camera_speed);
    SetControlRotation(NewRotation);
  }
}

void APilot::followControls(){
  FRotator CurrentRotation = GetControlRotation();
  FRotator NewRotation = FMath::RInterpTo(
      CurrentRotation, FRotator(camera_y * 90, camera_x * 180, 0.f),
                       GetWorld()->GetDeltaSeconds(), camera_speed);
  SetControlRotation(NewRotation);
}

void APilot::followMovement() {
  FRotator CurrentRotation = GetControlRotation();
  FRotator NewRotation = ControlledJet->Direction.Rotation();
  NewRotation = FMath::RInterpTo(CurrentRotation, FRotator(NewRotation.Pitch, NewRotation.Yaw, ControlledJet->GetActorRotation().Roll),
      GetWorld()->GetDeltaSeconds(), camera_speed);
  SetControlRotation(NewRotation);
}

void APilot::followTarget() {
  AActor *Bastard= ControlledJet->Target;
  if (Bastard) {
    FVector EnemyLocation = Bastard->GetActorLocation();

    FVector Direction = EnemyLocation - ControlledJet->GetActorLocation();

    FRotator CurrentRotation = GetControlRotation();
    FRotator NewRotation =
        FMath::RInterpTo(CurrentRotation, Direction.Rotation(),
                         GetWorld()->GetDeltaSeconds(), camera_speed);
    SetControlRotation(NewRotation);
  }
}

void APilot::OnCameraAction(const FInputActionValue &Value) {
  FVector2D MovementVector = Value.Get<FVector2D>();
  camera_x = MovementVector.X;
  camera_y = MovementVector.Y;
}

void APilot::OnGunsActionStarted() {
  if (ControlledJet)
    ControlledJet->startFiring();
}

void APilot::OnGunsActionCompleted() {
  if (ControlledJet)
    ControlledJet->stopFiring();
}

void APilot::OnLandingAction() {
  if (ControlledJet) {
    ControlledJet->toggleLanding();
  }
}

void APilot::OnLaunchAction() {
  UE_LOG(LogTemp, Warning, TEXT("OnLaunchAction"));
  if (ControlledJet)
    ControlledJet->launchMissile();
}

void APilot::OnLockActionStart() {
  GetWorldTimerManager().SetTimer(HoldTimerHandle, this,
                                  &APilot::startFollowingTarget,
                                  hold_threshold, false);
}

void APilot::OnLockActionEnd() {
  if (GetWorldTimerManager().IsTimerActive(HoldTimerHandle)) {
    GetWorldTimerManager().ClearTimer(HoldTimerHandle);
    ControlledJet->lockOn();
  }
  follow_target = false;
}

void APilot::OnPauseAction() {
  UE_LOG(LogTemp, Warning, TEXT("Pause you fucking faggot!"));
  if (!PauseWidget && PauseWidgetClass) {
    PauseWidget = CreateWidget<UUserWidget>(this, PauseWidgetClass);
    if (PauseWidget) {
      PauseWidget->AddToViewport();

      SetPause(true);

      FInputModeUIOnly InputModeData;
      InputModeData.SetWidgetToFocus(PauseWidget->TakeWidget());
      SetInputMode(InputModeData);
      bShowMouseCursor = true;
      
    }
  }
  UE_LOG(LogTemp, Warning, TEXT("I'm done!"));
}

void APilot::OnPitchAction(const FInputActionValue& Value) {
  float pitch = Value.Get<float>();
  if (ControlledJet) {
    ControlledJet->Pitch(pitch);
  }
}

void APilot::OnRollAction(const FInputActionValue &Value) {
  float roll = Value.Get<float>();
  if (ControlledJet) {
    ControlledJet->Roll(roll);
  }
}

void APilot::OnThrustAction(const FInputActionValue &Value) {
  float Throttle = Value.Get<float>();
  if (ControlledJet) {
    ControlledJet->ChangeThrottle(Throttle);
  }
}

void APilot::OnYawAction(const FInputActionValue &Value) {
  float yaw = Value.Get<float>();
  if (ControlledJet) {
    ControlledJet->Yaw(yaw);
  }
}

void APilot::OnPossess(APawn *InPawn){
  Super::OnPossess(InPawn);

  if (InPawn) {
    // Get possesed pawn
    ControlledPawn = InPawn;
    ControlledJet = Cast<APlayerJet>(ControlledPawn);
  }
}

void APilot::startFollowingTarget() {
  follow_target = true;
}

void APilot::Tick(float DeltaTime) {
  if (ControlledJet) {
    Super::Tick(DeltaTime);
    // Check how to control the camera
    if (follow_target) {
      followTarget();
    } else if ((abs(camera_x) > 0.01) || (abs(camera_y) > 0.01)) {
      followControls();
    } else if (ControlledJet->air_speed > 10)
      followMovement();
    else
      defaultRotation();
  }
}