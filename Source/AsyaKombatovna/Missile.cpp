#include "Missile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
AMissile::AMissile() {
  PrimaryActorTick.bCanEverTick = false;
  // Visual
  MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
      TEXT("/Game/Art/Models/Mesh/SM_R-13.SM_R-13"));
  if (MeshAsset.Succeeded()) {
    MissileMesh->SetStaticMesh(MeshAsset.Object);
    RootComponent =
        CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = MissileMesh;
  }

  // Collision
  //MissileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  MissileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  MissileMesh->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
  MissileMesh->SetNotifyRigidBodyCollision(true);
  MissileMesh->OnComponentHit.AddDynamic(this, &AMissile::OnHit);

    // Projectile settings
  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(
      TEXT("ProjectileMovement"));
  ProjectileMovement->InitialSpeed = 0.f;
  ProjectileMovement->MaxSpeed = 2000.f;
  ProjectileMovement->bIsHomingProjectile = false;
  ProjectileMovement->StopMovementImmediately();
  ProjectileMovement->ProjectileGravityScale = 0.f;
  ProjectileMovement->UpdatedComponent = MissileMesh;
  static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionParticleAsset(
      TEXT("ParticleSystem'/Game/FX/Visual/Cascade/P_Explosion.P_Explosion'"));
  if (ExplosionParticleAsset.Succeeded()) {
    ExplosionParticleEffect =
        static_cast<UParticleSystem *>(ExplosionParticleAsset.Object);
  }
}

void AMissile::BeginPlay() {
  Super::BeginPlay();
  // Collision extra settings
  // Moved em here cause they didn't work in constructor
}

void AMissile::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
                     UPrimitiveComponent *OtherComp, FVector NormalImpulse,
                     const FHitResult &Hit) {
  UE_LOG(LogTemp, Warning, TEXT("Hit actor is %s"), *Hit.GetActor()->GetName());
  UGameplayStatics::SpawnEmitterAtLocation(
      GetWorld(), ExplosionParticleEffect, GetActorLocation(),
      FRotator::ZeroRotator, FVector(1), true);
  TArray<AActor *> IgnoreActors;
  IgnoreActors.Add(GetOwner());
  UGameplayStatics::ApplyRadialDamage(GetWorld(), damage, Hit.Location, radius,
                                      nullptr, IgnoreActors, this,
                                      GetInstigatorController());
  Destroy();
}

bool AMissile::launch(USceneComponent *TargetComponent) {
  if (TargetComponent && GetOwner()) {
    MissileMesh->IgnoreActorWhenMoving(GetOwner(), true);
    ProjectileMovement->HomingTargetComponent = TargetComponent;
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = 3000.f;
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->Velocity =
        GetActorForwardVector() * ProjectileMovement->InitialSpeed;
    MissileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MissileMesh->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
    MissileMesh->SetNotifyRigidBodyCollision(true);
    ProjectileMovement->SetActive(true);
  }
  return true;
}