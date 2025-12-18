#include "Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABullet::ABullet() {
  PrimaryActorTick.bCanEverTick = true;
  //Visual
  BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
      TEXT("/Game/Art/Models/Mesh/SM_Bullet.SM_Bullet"));
  if (MeshAsset.Succeeded()) {
    BulletMesh->SetStaticMesh(MeshAsset.Object);
    BulletMesh->SetWorldScale3D(FVector(0.05f));
    BulletMesh->SetWorldRotation(FRotator(90.f, 0, 0));
    RootComponent =
        CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = BulletMesh;
  }
  //Collision
  BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  BulletMesh->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
  BulletMesh->SetNotifyRigidBodyCollision(true);
  BulletMesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
  // Movement
  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(
      TEXT("ProjectileMovement"));
  ProjectileMovement->UpdatedComponent = BulletMesh;
  ProjectileMovement->InitialSpeed = 2000.f;
  ProjectileMovement->MaxSpeed = 5000.f;
  ProjectileMovement->bRotationFollowsVelocity = true;
  ProjectileMovement->bShouldBounce = true;
  ProjectileMovement->ProjectileGravityScale = 1.f;
  // Particles
  static ConstructorHelpers::FObjectFinder<UParticleSystem> HitParticleAsset(
      TEXT("ParticleSystem'/Game/FX/Visual/Cascade/P_Explosion.P_Explosion'"));
  if (HitParticleAsset.Succeeded()) {
    HitParticleEffect = static_cast<UParticleSystem *>(HitParticleAsset.Object);
  }
}

void ABullet::BeginPlay() {
  Super::BeginPlay();
  if (GetOwner()) {
    BulletMesh->IgnoreActorWhenMoving(GetOwner(), true);
  }
  if ((ProjectileMovement) && (!BarrelDirection.IsZero())) {
    ProjectileMovement->Velocity =
        BarrelDirection * (ProjectileMovement->InitialSpeed + speed_of_gun);
    UE_LOG(LogTemp, Warning,
           TEXT("Spawned bullet at %s, initial speed is %s, "),
           *GetActorLocation().ToString(),
           *ProjectileMovement->Velocity.ToString());

    ProjectileMovement->SetActive(true);
  }
}

void ABullet::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
                    UPrimitiveComponent *OtherComp, FVector NormalImpulse,
                    const FHitResult &Hit) {
  UGameplayStatics::SpawnEmitterAtLocation(
      GetWorld(), HitParticleEffect, GetActorLocation(),
      FRotator::ZeroRotator, FVector(1), true);
  ApplyRadialDamageAtLocation(Hit.ImpactPoint);
  Destroy();
}

void ABullet::ApplyRadialDamageAtLocation(const FVector &Location) {
  TArray<AActor *> IgnoreActors;
  IgnoreActors.Add(GetOwner());
  UGameplayStatics::ApplyRadialDamage(
      GetWorld(), DamageAmount, Location, DamageRadius,
      nullptr,
      IgnoreActors,
      this,
      GetInstigatorController());
}

void ABullet::Tick(float DeltaTime) { Super::Tick(DeltaTime); }