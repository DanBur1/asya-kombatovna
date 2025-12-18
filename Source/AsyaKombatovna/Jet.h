// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Missile.h"
#include "RadarComponent.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "DestructableComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Jet.generated.h"

UCLASS()
class ASYAKOMBATOVNA_API AJet : public APawn, public IAKPawnInterface {
	GENERATED_BODY()

protected:
  // Default unreal events

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  // Properties
  
  // Jet's model. Presented as a skeletal mesh to animate control surfaces
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  USkeletalMeshComponent *JetMesh;

public:
  // Constants
  
  // How much speed jet gains based on throttle
  const float acceleration_rate = 2.f;
  // How much force has to be applied to simulate air brake resistance
  const float airbrake_force = 100000;
  // Array of references to the missiles the jet can launch
  
  // Full ammo belt capacity
  const int max_ammo = 1500;
  // Ammount of carry points
  const int MaxMissiles = 12;
  // How much lift can plane accumulate
  const float max_lift = 5000;
  // How much thrust can plane create
  const float max_thrust = 50.f;
  // Max speed approximately - 2500 kph / 3600 * 100 000 / 4.4
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float max_speed = 15782;
  // How much torque has to be applied to simulate control surfaces turining
  const float PitchTorque = 40.f;
  const float RollTorque = 80.f;
  const float YawTorque = 20.f;
  // How much throttle changes at a time
  const float throttle_lerp=0.01;
  // Hypothetical area that generates lift in meters
  const float wing_area = 8350.f;

  // Variables

  // Location of the plane on Z-axis
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float altitude;
  // Number of shots plane can make
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSystems")
  int ammunition;
  // Center of mass
  FVector com;
  // Curve which details the change in air density depending on the altitude
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  UCurveFloat *AirDensity; 
  // Scalar speed of the vehicle in centimeters per second
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float air_speed=0;
  // Angle between planes direction and air flow (since game is simplified it's just a XY plane)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float angle_of_attack;
  // Class of the bullet
  // Contains health and damage processing of the plane
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
  UDestructableComponent *DamageSystem;
  // Normalized vector velocity. Supposed to represent the direction the plane is going
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  FVector Direction;
  // Timer that handles plane's autocannon operations
  FTimerHandle GunTimer;
  // Particle effect for hits
  UPROPERTY(EditAnywhere, Category = "Effects")
  UParticleSystem *HitParticleEffect;
  // Meshes for the jetstreams
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UStaticMeshComponent *JetR;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UStaticMeshComponent *JetL;
  // Muzzle flash
  UPROPERTY(EditAnywhere, Category = "Effects")
  UParticleSystem *MuzzleFlashParticleEffect;
  // Prevents plane from exploding on impact
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  bool landing_mode = false;
  // How much upward force the plane produces
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float lift;
  // Curve which details how much AOA affects lift
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  UCurveFloat *LiftEfficiency;
  // Weapons attached to the carrypoints
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
  TArray<AMissile *> Missiles;
  // Actual pitch of the aircraft
  float pitch;
  // Component responsible for plane's targetting
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
  URadarComponent *Radar;
  // Locked enemy
  AJet *Target;
  // Fuel in the system. (Not actual fuel, used for movement system)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float throttle = 0;
  // Forward vector of the mesh
  FVector VectorOfAttack;
  // Vector velocity
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  FVector Velocity;

    // Following variables are only used to read input values in the animation
  // blueprint
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  bool airbrake_on = false;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float pitch_value = 0;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float roll_value = 0;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetMovement")
  float yaw_value = 0;

  //Interfaces
  virtual bool isPlayer_Implementation() override;

  //Functions
  
  // Sets default values for this pawn's properties
	AJet();
  // Lets jet go over its maximum throttle to accelerate faster
  void Afterburner(float input_value);
  // Adds counterforce to stop the plane faster
  void AirBrake();
  // Calculates angle between nose cone and XY plane (windflow is asummed to be perfectly straight without direction)
  void CalculateAOA();
  // Changes the power at which the engine works
  void ChangeThrottle(float input_value);
  // Fires a single bullet
  void fire();
  // [UNUSED} Calculates the force created by the plane
  FVector generateLift(float DeltaTime);
  // Calculates thrust generated by the engine
  float GetThrust();
  // Calculates the force with which the plane is supposed to move
  FVector getThrustVector(float thrust, float DeltaTime);
  // Sends a AMissile projectile after locked enemy
  void launchMissile();
  // Changes the target
  void lockOn();
  // Event that processes collisions
  UFUNCTION()
  void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
             UPrimitiveComponent *OtherComp, FVector NormalImpulse,
             const FHitResult &Hit);
  // Turns the plane up and down
  void Pitch(float Value);
  // Respawns missiles and resets ammo count to max
  void reload(UWorld *World);
  // Rotates the plane left and right
  void Roll(float Value);
  // Starts the timer which calls the fire function
  void startFiring();
  // Stops the timer which calls the fire function
  void stopFiring();
  //Useless
  virtual void SetupPlayerInputComponent(
      class UInputComponent *PlayerInputComponent) override;
  // Default function for damage event
  virtual float TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent,
                           AController *EventInstigator,
                           AActor *DamageCauser) override;
  // Default function called every frame
  virtual void Tick(float DeltaTime) override;
  // Changes landing mode
  void toggleLanding();
  // Turns the plane left and right
  void Yaw(float Value);
};
