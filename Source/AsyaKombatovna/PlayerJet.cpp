// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerJet.h"
//Constructor
APlayerJet::APlayerJet() :AJet() {
  PrimaryActorTick.bCanEverTick = true;
  //Camera class
  PlayerCameraComponent =
      CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
  // Boom class
  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
  SpringArm->SetupAttachment(RootComponent);
  SpringArm->TargetArmLength = 600.f; // Camera boom length
  SpringArm->bUsePawnControlRotation = true;
  //Camera position
  PlayerCameraComponent->SetupAttachment(SpringArm,
                                         USpringArmComponent::SocketName);

  //Camera FOV
  PlayerCameraComponent->FieldOfView = 90.f;

  // Health
  //DamageSystem->has_debris = true;
}

void APlayerJet::Tick(float DeltaTime){
  Super::Tick(DeltaTime);
  /*
  if (air_speed > KINDA_SMALL_NUMBER) {
    UE_LOG(LogTemp, Warning, TEXT("Direction %s"),
           *direction.ToString());
    // Позиционный оффсет камеры назад от направления движения
    FVector DesiredOffset = -direction * MaxCameraOffset;

    // Плавное сглаживание смещения камеры
    FVector CurrentOffset = SpringArm->SocketOffset;
    FVector NewOffset = FMath::VInterpTo(CurrentOffset, DesiredOffset,
                                         DeltaTime, CameraLerpSpeed);
    SpringArm->SocketOffset = NewOffset;
    UE_LOG(LogTemp, Warning, TEXT("Camera offset %s"),
           *SpringArm->SocketOffset.ToString());
    // Поворот SpringArm в сторону движения (чтобы камера смотрела по движению)
    FRotator CurrentRotation = SpringArm->GetComponentRotation();
    FRotator TargetRotation = direction.Rotation();
    UE_LOG(LogTemp, Warning, TEXT("Camera rotation %s"),
           *TargetRotation.ToString());
    //FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation,
    //                                        DeltaTime, CameraLerpSpeed);
    SpringArm->SetWorldRotation(TargetRotation);
    UE_LOG(LogTemp, Warning, TEXT("Camera rotation %s"), *SpringArm->GetComponentRotation().ToString());
  } else {
    // Если объект почти не движется, возвращаем камеру в нулевой оффсет и
    // нулевой поворот
    FVector CurrentOffset = SpringArm->SocketOffset;
    FVector NewOffset = FMath::VInterpTo(CurrentOffset, FVector::ZeroVector,
                                         DeltaTime, CameraLerpSpeed);
    SpringArm->SocketOffset = NewOffset;

    FRotator CurrentRotation = SpringArm->GetComponentRotation();
    FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation, FRotator::ZeroRotator, DeltaTime, CameraLerpSpeed);
    SpringArm->SetWorldRotation(NewRotation);
  }*/
}

void APlayerJet::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
  UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit,
                                 true);
}


// Interfaces

bool APlayerJet::isPlayer_Implementation() { return true; }