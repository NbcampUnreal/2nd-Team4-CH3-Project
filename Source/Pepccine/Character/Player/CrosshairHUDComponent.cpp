#include "Character/Player/CrosshairHUDComponent.h"
#include "Character/Controller/PepccinePlayerController.h"
#include "Components/Image.h"

UCrosshairHUDComponent::UCrosshairHUDComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCrosshairHUDComponent::BeginPlay()
{
	Super::BeginPlay();

  GetPlayerController();
}

void UCrosshairHUDComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCrosshairHUDComponent::GetPlayerController()
{
  APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
  if (!PlayerController) return;
  APepccinePlayerController* PepccinePlayerController = Cast<APepccinePlayerController>(PlayerController);
  if (!PepccinePlayerController) return;

  if (CrosshairWidget)
  {
    CrosshairWidget->RemoveFromParent();
    CrosshairWidget = nullptr;
  }

  //if (CrosshairWidgetClass)
  //{
  //  CrosshairWidget = CreateWidget<UUserWidget>(PepccinePlayerController, CrosshairWidgetClass);

  //  UImage* CrosshairImage = Cast<UImage>(CrosshairWidget->GetWidgetFromName(TEXT("CrosshairImage")))

  //   CrosshairImage->AddToViewport();
  //   CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
  //}
}

void UCrosshairHUDComponent::ShowCrosshair()
{
  if (CrosshairWidget)
  {
    CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
  }
}

void UCrosshairHUDComponent::HideCrosshair()
{
  if (CrosshairWidget)
  {
    CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
  }
}

