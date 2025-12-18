// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AKPawnInterface.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DestructableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASYAKOMBATOVNA_API UDestructableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
  //Variables

  // Flag for whether the character is dead or object is destroyed
  bool dead=false;
  // Flag that decides whether the component owner should be completely
  // destroyed (F) or process death differently (T)
  bool has_debris=false;
  
  //Functions
  
	// Sets default values for this component's properties
	UDestructableComponent();

protected:
  // Constants

  // Upper border of character's health or object durability
  float max_health=100;

  // Variables

  UPROPERTY(EditAnywhere, Category = "Effects")
  UParticleSystem *DeathParticleEffect;
  // Character health or object durabiliy
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Variables")
  float health=100;
  // Indicator for all target in radar zone class
  UPROPERTY(EditAnywhere, Category = "UI")
  TSubclassOf<UUserWidget> RadarIndicatorWidgetClass;
  // Radar indicator itself
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
  class UWidgetComponent *RadarIndicatorWidgetInstance = nullptr;

  public:
  // Unreal events

  //Tick event
  virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                  FActorComponentTickFunction *ThisTickFunction) override;

  //Functions

  // Subtracts damage from health
  void changeHealth(float damage);
  // Removes the green square when actor is out of range
  void hideRadarWidget();
  // Processes death of character or destruction of objects
  void kill();
  // Sets maximum value for health
  void setMaxHealth(float value);
  // Sets new value for health
  void setHealth(float value);
  // Adds the green square when actor is in range
  void showRadarWidget();
 };
