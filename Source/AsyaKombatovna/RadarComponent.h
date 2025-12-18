#pragma once

#include "AKPawnInterface.h"
#include "Engine/OverlapResult.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "RadarComponent.generated.h"

class UDestructableComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ASYAKOMBATOVNA_API URadarComponent : public UActorComponent {
  GENERATED_BODY()

public:
  // Unreal generated events

  // Constructor
  URadarComponent();

  // Variables

  // The target that the radar currently tracks
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  AActor *CurrentLockTarget;
  // Radar range
  static constexpr float radar_range = 3000000.f;
  // Array of targets within radar range
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<AActor *> TargetsInRange;
  // Array of all actors withtin collision sphere
  TArray<FOverlapResult> Overlaps;
  // Sphere that simulates radar 
  FCollisionShape Sphere;
  // Max angle of radar lock
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float lock_on_cone_half_angle = 30.f;

  // Functions

  // Scans for targets. In order for an actor to classify as target it needs to have a destructable component and an AK Pawn Interface.
  void scan();
  // Writes down an object from the array as a current target
  // if it's in radar cone or closest of all targets
  void lockOn();

protected:
  // Unreal generated events

  // Called after start
  virtual void BeginPlay() override;

private:
  // Functions

  // Returns Destructable component of the object if it has one
  UDestructableComponent *getDestructableComponent(const AActor *Actor) const;
  // Returns vector that represents the direction the object is facing
  FVector getForwardVector() const;
  // Checks whether the target is in the radar's cone
  bool isInLockOnCone(const AActor *Actor) const;
  // Checks whether the object is in range. Returns T/F
  bool isInRadarRange(const AActor *Actor) const;
};