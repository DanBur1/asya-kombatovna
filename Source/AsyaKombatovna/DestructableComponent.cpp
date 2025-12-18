// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableComponent.h"

// Default unreal events
// 
// Sets default values for this component's properties
UDestructableComponent::UDestructableComponent()
{
 // Tick function is only used for widget adjustment. If there are conditions
 // that prevent radar widget from being shown. Tick function shouldn't be called
  if (GetOwner()) {

    if (GetOwner()->GetClass()->ImplementsInterface(
            UAKPawnInterface::StaticClass()))
      if (IAKPawnInterface::Execute_isPlayer(GetOwner())) {
        UE_LOG(LogTemp, Warning, TEXT("Player damage is ticking"));
        PrimaryComponentTick.bCanEverTick = false;
      } else {
        UE_LOG(LogTemp, Warning, TEXT("Non player damage is ticking"));
        PrimaryComponentTick.bCanEverTick = true;
      }
  }
  static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(
      TEXT("ParticleSystem'/Game/FX/Visual/Cascade/P_Explosion.P_Explosion'"));
  if (ParticleAsset.Succeeded()) {
    DeathParticleEffect = ParticleAsset.Object;
  }
  // Widgets
  RadarIndicatorWidgetInstance = CreateDefaultSubobject<UWidgetComponent>(
      TEXT("DestructableComponent"));
  static ConstructorHelpers::FClassFinder<UUserWidget>
      RadarIndicatorWidgetBPClass(
      TEXT("/Game/UI/WBP_RadarSpotted"));
  if (RadarIndicatorWidgetBPClass.Succeeded()) {
    RadarIndicatorWidgetClass = RadarIndicatorWidgetBPClass.Class;
  }
}

void UDestructableComponent::TickComponent(
  float DeltaTime, ELevelTick TickType,
  FActorComponentTickFunction* ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  if (RadarIndicatorWidgetInstance) {
    APlayerController *PlayerController =
        GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
      return;
    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
    FVector WidgetLocation =
        RadarIndicatorWidgetInstance->GetComponentLocation();
    FRotator LookAtRotation =
        -1*CameraRotation; //(CameraLocation - WidgetLocation).Rotation();
    RadarIndicatorWidgetInstance->SetWorldRotation(LookAtRotation);
    float DistanceToCamera = FVector::Distance(CameraLocation, WidgetLocation);
    const float BaseDistance = 5000.f;
    float ScaleFactor =
        DistanceToCamera > 0.f ? (DistanceToCamera / BaseDistance) : 1.f;
    FVector NewScale(ScaleFactor, ScaleFactor, ScaleFactor);
    RadarIndicatorWidgetInstance->SetWorldScale3D(NewScale);
  }
}

// Functions

void UDestructableComponent::changeHealth(float damage) {
  health = health - damage;
	if (health < 0) {
    kill();
	}
}

void UDestructableComponent::hideRadarWidget() {
  if (RadarIndicatorWidgetInstance) {
    RadarIndicatorWidgetInstance->DestroyComponent();
    RadarIndicatorWidgetInstance = nullptr;
  }
}

void UDestructableComponent::kill() {
  // If the actor has debris, death animation or etc. Game sets the dead flag so
	// actor can process its death on its own.
	// If the actor does nothing after its death it's completely removed
  if (DeathParticleEffect) {
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticleEffect,
                                             GetOwner()->GetActorLocation(),
                                             FRotator::ZeroRotator, FVector(3, 3, 3), true);
  }
  dead = true;
  if (!has_debris) {
    GetOwner()->Destroy();
  }
}

void UDestructableComponent::setMaxHealth(float value) { max_health = value; }

void UDestructableComponent::setHealth(float value) {
	health = value;
}


void UDestructableComponent::showRadarWidget() {
  if (RadarIndicatorWidgetClass) {
    RadarIndicatorWidgetInstance->SetWidgetClass(RadarIndicatorWidgetClass);
    RadarIndicatorWidgetInstance
          ->RegisterComponent();
    RadarIndicatorWidgetInstance->AttachToComponent(
        GetOwner()->GetRootComponent(),
          FAttachmentTransformRules::KeepRelativeTransform);
    RadarIndicatorWidgetInstance->SetWidgetClass(RadarIndicatorWidgetClass);
    RadarIndicatorWidgetInstance->SetDrawSize(
          FVector2D(500, 500));
    RadarIndicatorWidgetInstance->SetVisibility(true);
    RadarIndicatorWidgetInstance->SetDrawAtDesiredSize(true);
      RadarIndicatorWidgetInstance->SetTwoSided(true);
    }
}

