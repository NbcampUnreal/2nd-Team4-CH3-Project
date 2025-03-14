#include "CrosshairHUDComponent.h"
#include "Character/Player/PepCharacter.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

UCrosshairHUDComponent::UCrosshairHUDComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

  CrosshairWidget = nullptr;
  AimSize = 0;
  MaxAimSize = 1000;
  MinAimSize = 100;
}

void UCrosshairHUDComponent::BeginPlay()
{
	Super::BeginPlay();

  if (CrosshairWidget) CrosshairWidget->AddToViewport();
  CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UCrosshairHUDComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  UpdateCrosshair(DeltaTime);
}

void UCrosshairHUDComponent::UpdateCrosshair(float DeltaTime)
{
    if (!CrosshairWidget) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    FVector Velocity = Owner->GetVelocity();
    float Speed = Velocity.Size();
    if (Speed == 0 && FMath::IsNearlyZero(AimSize, 1.0f)) return;

    APepCharacter* PepCharacter = Cast<APepCharacter>(Owner);
    if (!PepCharacter) return;

    bool& IsFiring = PepCharacter->bIsFiring;
    UCharacterMovementComponent* CharacterMovementComponent = PepCharacter->GetCharacterMovement();
    if (!CharacterMovementComponent) return;

    if (CharacterMovementComponent->IsFalling()) {
        AimSize += 100.f * DeltaTime;
    }
    else {
        if (Speed > 300)
        {
            AimSize += Speed * DeltaTime;
        }
        else if (Speed > 0)
        {
            AimSize -= FMath::Clamp(Speed * DeltaTime * 50, 0.0f, AimSize);
        }
        else
        {
            if (IsFiring)
            {
                AimSize +=  200.0f * DeltaTime; 
            }
            else
            {
                AimSize -= FMath::Clamp(2000.f * DeltaTime, 0.0f, AimSize);
            }
        }
    }

    AimSize = FMath::Clamp(AimSize, MinAimSize, MaxAimSize);

    CrosshairWidget->UpdateCrosshairSize(AimSize);
}

void UCrosshairHUDComponent::ShowCrosshair()
{
  CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
}

void UCrosshairHUDComponent::HideCrosshair()
{
  CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
}