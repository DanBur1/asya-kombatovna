#pragma once

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class ASYAKOMBATOVNA_API AMissile : public AActor {
  GENERATED_BODY()

public:
  // Unreal generated events

  // Constructor
  AMissile();
  // Plays when the game starts or the object gets added
  virtual void BeginPlay() override;

  // Constants

  // Damage at the center of the explosion
  const float damage = 60.f;
  // Radius of the explosion of the missile in cm
  const float radius = 300.f;

  // Variables

  // Particle effect of the explosion
  UPROPERTY(EditAnywhere, Category = "Effects")
  class UParticleSystem *ExplosionParticleEffect = nullptr;
  // Model of the missile
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UStaticMeshComponent *MissileMesh;
  // Projectile component
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UProjectileMovementComponent *ProjectileMovement;

  // Functions

  // Creates explosion. Called OnHit
  UFUNCTION()
  void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
             UPrimitiveComponent *OtherComp, FVector NormalImpulse,
             const FHitResult &Hit);
  // Launches the missile
  bool launch(USceneComponent *TargetComponent);
};