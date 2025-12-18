#include "RadarComponent.h"
#include "DestructableComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

URadarComponent::URadarComponent() {
  CurrentLockTarget = nullptr;
  Sphere = FCollisionShape::MakeSphere(radar_range);
  TargetsInRange.Empty();
}

void URadarComponent::BeginPlay() { Super::BeginPlay(); }

bool URadarComponent::isInRadarRange(const AActor *Actor) const {
  if (!Actor || !GetOwner())
    return false;
  return FVector::DistSquared(Actor->GetActorLocation(),
                              GetOwner()->GetActorLocation()) <=
         radar_range * radar_range;
}

UDestructableComponent *
URadarComponent::getDestructableComponent(const AActor *Actor) const {
  if (!Actor)
    return nullptr;
  return Actor->FindComponentByClass<UDestructableComponent>();
}

void URadarComponent::scan() {
  if (!GetWorld() || !GetOwner())
    return;
  FVector OwnerLocation = GetOwner()->GetActorLocation();

  bool bOverlapFound = GetWorld()->OverlapMultiByObjectType(
      Overlaps, OwnerLocation, FQuat::Identity,
      FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn), Sphere);

  TArray<AActor *> NewTargets;

  if (bOverlapFound) {
    for (const FOverlapResult &Result : Overlaps) {
      AActor *OverlappedActor = Result.GetActor();
      UDestructableComponent * DestructableComponent = getDestructableComponent(OverlappedActor);
      if (OverlappedActor && (DestructableComponent != nullptr) && OverlappedActor != GetOwner()) {
        if (NewTargets.AddUnique(OverlappedActor) && OverlappedActor->GetClass()->ImplementsInterface(UAKPawnInterface::StaticClass())) {
          if (!IAKPawnInterface::Execute_isPlayer(OverlappedActor))
          DestructableComponent->showRadarWidget();
        }
      }
    }
  }

  // Adds new targets
  for (AActor *Target : NewTargets) {
    if (!TargetsInRange.Contains(Target)&&(Target!=GetOwner())) {
      TargetsInRange.Add(Target);
    }
  }

  // Deletes targets outside of radar range
  for (int32 i = TargetsInRange.Num() - 1; i >= 0; --i) {
    UDestructableComponent *DestructableComponent =
        getDestructableComponent(TargetsInRange[i]);
    if (!isInRadarRange(TargetsInRange[i])){
      if (CurrentLockTarget == TargetsInRange[i]) {
        CurrentLockTarget = nullptr;
      }
      if (DestructableComponent != nullptr)
        DestructableComponent->hideRadarWidget();
      TargetsInRange.RemoveAt(i);
    }
  }

  if (!isInRadarRange(CurrentLockTarget)) {
    CurrentLockTarget = nullptr;
  }
}

FVector URadarComponent::getForwardVector() const {
  if (!GetOwner())
    return FVector::ForwardVector;
  return GetOwner()->GetActorForwardVector();
}

bool URadarComponent::isInLockOnCone(const AActor *Actor) const {
  if (!GetOwner() || !Actor)
    return false;

  FVector ToTarget =
      (Actor->GetActorLocation() - GetOwner()->GetActorLocation())
          .GetSafeNormal();
  FVector Forward = getForwardVector();

  float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(lock_on_cone_half_angle));
  float Dot = FVector::DotProduct(Forward, ToTarget);

  return Dot >= CosHalfAngle;
}

void URadarComponent::lockOn() {
  if (TargetsInRange.Num() == 0) {
    CurrentLockTarget = nullptr;
    return;
  }

  TArray<AActor *> LockOnCandidates;

  for (AActor *Target : TargetsInRange) {
    if (isInLockOnCone(Target)) {
      LockOnCandidates.Add(Target);
    }
  }

  if (LockOnCandidates.Num() == 0) {
    // Chooses closest target to lock if there is nobody in the radar cone
    AActor *Nearest = nullptr;
    float MinDist = TNumericLimits<float>::Max();

    for (AActor *Target : TargetsInRange) {
      float Dist = FVector::DistSquared(GetOwner()->GetActorLocation(),
                                        Target->GetActorLocation());
      if (Dist < MinDist) {
        Nearest = Target;
        MinDist = Dist;
      }
    }
    CurrentLockTarget = Nearest;
    return;
  }

  // Finds target in radar cone
  AActor *BestTarget = nullptr;
  float BestDot = -1.f;
  FVector Forward = getForwardVector();

  for (AActor *Target : LockOnCandidates) {
    FVector ToTarget =
        (Target->GetActorLocation() - GetOwner()->GetActorLocation())
            .GetSafeNormal();
    float Dot = FVector::DotProduct(Forward, ToTarget);
    if (Dot > BestDot) {
      BestDot = Dot;
      BestTarget = Target;
    }
  }

  CurrentLockTarget = BestTarget;
}