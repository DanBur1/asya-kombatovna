// Fill out your copyright notice in the Description page of Project Settings.


#include "Jet.h"

// Sets default values
AJet::AJet()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  // Setting up the mesh
  JetMesh = CreateDefaultSubobject<class USkeletalMeshComponent>(TEXT("JetMesh"));
  RootComponent = JetMesh;
  
  // Working with files
  
  // Skeletal mesh
  static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
      TEXT("/Game/Characters/SkeletalMesh/SKM_Su-37Animated.SKM_Su-37Animated"));
  if (MeshAsset.Succeeded()) {
    JetMesh->SetSkeletalMesh(MeshAsset.Object);
  }

  // Animation blueprint
  static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(
      TEXT("/Game/Characters/Animations/AB_Su-37"));
  if (AnimBPClass.Succeeded()) {
    JetMesh->SetAnimInstanceClass(AnimBPClass.Class);
  }

  // Lift
  static ConstructorHelpers::FObjectFinder<UCurveFloat> LiftObject(
      TEXT("/Game/Core/Curves/C_LiftEfficiency.C_LiftEfficiency"));
  if (LiftObject.Succeeded()) {
    LiftEfficiency = LiftObject.Object;
  }

  // AirDensity
  static ConstructorHelpers::FObjectFinder<UCurveFloat> AirDensityObject(
      TEXT("/Game/Core/Curves/C_AirDensity.C_AirDensity"));
  if (AirDensityObject.Succeeded()) {
    AirDensity = AirDensityObject.Object;
  }

  // Health
  DamageSystem = CreateDefaultSubobject<UDestructableComponent>(
      TEXT("DestructableComponent"));
  DamageSystem->setMaxHealth(100);
  DamageSystem->setHealth(100);
  DamageSystem->has_debris = false;

  // Collisions
  if (!landing_mode) {
    JetMesh->SetNotifyRigidBodyCollision(true);
    JetMesh->BodyInstance.SetCollisionProfileName("Pawn");
    JetMesh->OnComponentHit.AddDynamic(this, &AJet::OnHit);
  }

  // Effects
  static ConstructorHelpers::FObjectFinder<UParticleSystem> HitParticleAsset(
      TEXT("/Game/FX/Visual/Particles/P_ImpactSparks.P_ImpactSparks"));
  if (HitParticleAsset.Succeeded()) {
    HitParticleEffect = HitParticleAsset.Object;
  }
  static ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleParticleAsset(
      TEXT("ParticleSystem'/Game/FX/Visual/Cascade/P_Explosion.P_Explosion'"));
  if (MuzzleParticleAsset.Succeeded()) {
    MuzzleFlashParticleEffect = MuzzleParticleAsset.Object;
  }
  JetR =
      CreateDefaultSubobject<class UStaticMeshComponent>(TEXT("JetR"));
  JetL = CreateDefaultSubobject<class UStaticMeshComponent>(TEXT("JetL"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> JetstreamMeshAsset(TEXT(
      "/Game/Art/Models/Mesh/SM_Jet.SM_Jet"));
  if (JetstreamMeshAsset.Succeeded()) {
    JetR->SetStaticMesh(JetstreamMeshAsset.Object);
    JetL->SetStaticMesh(JetstreamMeshAsset.Object);
    JetR->SetupAttachment(JetMesh, FName("nozzle_r"));
    JetL->SetupAttachment(JetMesh, FName("nozzle_l"));
    JetR->SetRelativeScale3D(FVector(0.f));
    JetL->SetRelativeScale3D(FVector(0.f));
    JetR->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    JetL->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
  }
  // Physics

  JetMesh->SetSimulatePhysics(true);

  JetMesh->SetEnableGravity(true);
  JetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

  // Weapons
  ammunition = max_ammo;
  Radar = CreateDefaultSubobject<URadarComponent>(TEXT("RadarComponent"));
 }

void AJet::Afterburner(float input_value) {
  throttle = FMath::Clamp(throttle + 0.5*input_value * throttle_lerp, 0, 1.7);
  UE_LOG(LogTemp, Warning, TEXT("Afterburner engaged. Throttle %f"), throttle);
}

void AJet::AirBrake() {
  FVector ForwardVector = JetMesh->GetForwardVector();
  float ForwardSpeed =
      FVector::DotProduct(JetMesh->GetComponentVelocity(), ForwardVector);
  if (ForwardSpeed > 1.f) {
    FVector BrakeForce = -ForwardVector * airbrake_force;
    //JetMesh->AddForce(BrakeForce);
    UE_LOG(LogTemp, Warning, TEXT("Airbrake engaged"));
  } else
    airbrake_on = false;
}

// Called when the game starts or when spawned
void AJet::BeginPlay()
{
	Super::BeginPlay();
  reload(GetWorld());
  JetMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
  JetMesh->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
}

void AJet::CalculateAOA() {
  plane_pitch = JetMesh->GetComponentRotation().Pitch;
  float dot = FVector::DotProduct(VectorOfAttack, FVector(VectorOfAttack.X, VectorOfAttack.Y, 0));
  float degrees;
  if (plane_pitch == 0)
    degrees = 0;
  else degrees = plane_pitch / abs(plane_pitch) * FMath::RadiansToDegrees(FMath::Acos(dot));
  if (plane_pitch >= 0) {
    angle_of_attack = degrees;
  } else
    angle_of_attack = -degrees;
}

void AJet::ChangeThrottle(float input_value) {
  if (input_value < 0) {
    airbrake_on = true;
    if (throttle == 0.f)
      AirBrake();
    throttle = FMath::Clamp(throttle + 4*input_value * throttle_lerp, 0, 1);
  } else if (input_value > 0) {
    airbrake_on = false;
    if (throttle == 1.f)
      Afterburner(input_value);
    else
    throttle = FMath::Clamp(throttle + 2*input_value * throttle_lerp, 0, 1);
  }
  JetR->SetRelativeScale3D(FVector(0.2f, throttle, 0.2f));
  JetL->SetRelativeScale3D(FVector(0.2f, throttle, 0.2f));
}

void AJet::fire() {
  if (ammunition > 0) {

    UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(), MuzzleFlashParticleEffect,
        JetMesh->GetSocketLocation("GunSocket"),
        FRotator::ZeroRotator,
        FVector(0.2f), true);
    const FTransform SpawnTransform = JetMesh->GetSocketTransform("GunSocket");
    UE_LOG(LogTemp, Warning, TEXT("Before spawn."));
    ABullet *Bullet = GetWorld()->SpawnActorDeferred<ABullet>(
        ABullet::StaticClass(), SpawnTransform, this, nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    UE_LOG(LogTemp, Warning, TEXT("After spawn started"));
    if (Bullet) {
      UE_LOG(LogTemp, Warning, TEXT("Initializing the bullet"));
      Bullet->BarrelDirection = VectorOfAttack;
      Bullet->speed_of_gun = air_speed;
      JetMesh->IgnoreActorWhenMoving(Bullet, true);
      UGameplayStatics::FinishSpawningActor(Bullet, SpawnTransform);
    }
      UE_LOG(LogTemp, Warning, TEXT("Spawn finished"));
    ammunition--;
  }
}

FVector AJet::generateLift(float DeltaTime) {
  //float energy_of_lift = FMath::Clamp((pow(air_speed, 2) / 2 * multipier), 0,
  //                                  max_lift);
  //return JetMesh->GetForwardVector() * energy_of_lift *
  //    EfficiencyCurve->GetFloatValue(angle_of_attack);
  float lift_force;
  
  // Find force generated by the wing using F=Cy*p*V^2/2*S formula
  lift_force = LiftEfficiency->GetFloatValue(angle_of_attack) *
               AirDensity->GetFloatValue(altitude) * air_speed * air_speed *
               wing_area / 2.f;
  lift_force =  FMath::Clamp(lift_force * DeltaTime, 0, max_lift);
  
  return FVector(0.f, 0.f, 1.f)
  *lift_force;
}

FVector AJet::getThrustVector(float thrust, float DeltaTime) {
  FVector ThrustVector = JetMesh->GetForwardVector() * JetMesh->GetMass() *
                         thrust *
                         acceleration_rate * DeltaTime;
  return ThrustVector;
}

float AJet::GetThrust() {
  //return JetMesh->GetMass()*acceleration*20;
  return max_thrust * throttle;
}

void AJet::launchMissile() {
  for (int i = 0; i < Missiles.Num(); ++i) {
    AMissile *Missile = Missiles[i];
    if (Missile) {
      if (Radar->CurrentLockTarget) {
        Missile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Missile->launch(Radar->CurrentLockTarget->GetRootComponent());
        Missiles[i] = nullptr;
        return;
      }
    }
  }
  return;
}

void AJet::lockOn() { Radar->lockOn(); }

void AJet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, FVector NormalImpulse,
  const FHitResult& Hit) {
  if (HitParticleEffect) {
    UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(), HitParticleEffect, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(1), true);
  }
  DamageSystem->changeHealth(air_speed*0.03);
}

void AJet::pitch(float Value) {
  pitch_value = Value;
  if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) {
    UE_LOG(LogTemp, Warning, TEXT("Catching not pitching %f"), pitch_value);
    FVector TorqueAxis = JetMesh->GetRightVector();
    FVector Torque = TorqueAxis * pitch_value * PitchTorque;
    JetMesh->AddTorqueInRadians(Torque);
  }
}

void AJet::reload(UWorld *World) {
  if (!World || !JetMesh)
    return;
  TSubclassOf<AMissile> MissileClass;
  if (Missiles.Num() != MaxMissiles)
    Missiles.SetNum(MaxMissiles);

  for (int i = 0; i < MaxMissiles; ++i) {
    if (Missiles[i] == nullptr) {

      FTransform SpawnTransform = GetActorTransform();
      UE_LOG(LogTemp, Warning, TEXT("%s transform is ¹%s"),
             *GetName(),
             *GetActorTransform().ToString());
      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = GetInstigator();
      SpawnParams.SpawnCollisionHandlingOverride =
          ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

      AMissile *NewMissile = World->SpawnActor<AMissile>(
          AMissile::StaticClass(), SpawnTransform, SpawnParams);
       if (NewMissile) {
        FString SocketName = FString::Printf(TEXT("carry_point%d"), i + 1);
        FName SocketFName(*SocketName);
        UE_LOG(LogTemp, Warning, TEXT("Missile ¹%d loaded"), i+1);
        NewMissile->AttachToComponent(
            JetMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,
            SocketFName);
        Missiles[i] = NewMissile;
      }
    }
  }
}

void AJet::roll(float Value) {
  roll_value = Value;
  if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) {
    UE_LOG(LogTemp, Warning, TEXT("Rollin n scratchin %f"), roll_value);
    FVector TorqueAxis = JetMesh->GetForwardVector();
    FVector Torque = TorqueAxis * roll_value * RollTorque;
    JetMesh->AddTorqueInRadians(Torque);
  }
}

void AJet::startFiring() {
  if (!GetWorld()->GetTimerManager().IsTimerActive(GunTimer)) {
    GetWorld()->GetTimerManager().SetTimer(GunTimer, this, &AJet::fire, 0.033f,
                                           true, 0.f);
  }
}

void AJet::stopFiring() {
  if (GetWorld()->GetTimerManager().IsTimerActive(GunTimer)) {
    GetWorld()->GetTimerManager().ClearTimer(GunTimer);
  }
}


void AJet::stopPitching(){ pitch_value = 0.f;}

void AJet::stopRolling(){ roll_value = 0.f;}

void AJet::stopTurning() { yaw_value = 0.f; }

float AJet::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent,
                         AController *EventInstigator, AActor *DamageCauser) {
  float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent,
                                         EventInstigator, DamageCauser);
  DamageSystem->changeHealth(DamageAmount);
  return ActualDamage;
}

    // Called every frame
void AJet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  /*
  
  //
  //
  //
  
  pitch = JetMesh->GetComponentRotation().Pitch;
  CalculateAOA();
  Velocity = GetThrustVector(DeltaTime);
  direction = Velocity.GetSafeNormal();
  air_speed = Velocity.Length();
  AddActorWorldOffset(Velocity, true);

  //  JetMesh->AddForce(GetThrustVector(GetThrust()), NAME_Root);
  //JetMesh->AddForce(generateLift(2000, LiftEfficiency));
  */
  // Runs radar to get all new pings
  Radar->scan();

  // Gets all variables needed to calculate thrust
  VectorOfAttack = JetMesh->GetForwardVector();
  CalculateAOA();
  Velocity = GetVelocity();
  Direction = Velocity.GetSafeNormal();
  air_speed = Velocity.Length();
  altitude = GetActorLocation().Z;
  FVector DragForce =
      -Velocity.GetSafeNormal() * air_speed * air_speed * DeltaTime;

  // Applies force using thrust produced by the jet
  JetMesh->AddForce(getThrustVector(GetThrust(), DeltaTime));
  JetMesh->AddForce(generateLift(DeltaTime));
  JetMesh->AddForce(DragForce);
}

void AJet::toggleLanding() {
}

void AJet::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJet::yaw(float Value) {
  yaw_value = Value;
  UE_LOG(LogTemp, Warning, TEXT("Yawing %f"), yaw_value);
  if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) {
    FVector TorqueAxis = JetMesh->GetUpVector();
    FVector Torque = TorqueAxis * yaw_value * YawTorque;
    JetMesh->AddTorqueInRadians(Torque);
  }
  //yaw_value = Value;
  // if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) {
  //  FRotator Torque =
  //      FRotator(0, Value * YawTorque * GetWorld()->GetDeltaSeconds(),
  //                             0); // GetActorRotation();
  //  // Torque.Yaw = Torque.Yaw + Value * YawTorque;
  //   AddActorLocalRotation(Torque);
  //}
 
}

// Interface

bool AJet::isPlayer_Implementation() { return false; }