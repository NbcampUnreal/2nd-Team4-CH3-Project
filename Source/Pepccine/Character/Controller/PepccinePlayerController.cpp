#include "PepccinePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/Components/CrosshairHUDComponent.h"

APepccinePlayerController::APepccinePlayerController()
{
  InputMappingContext = nullptr;
  MoveAction = nullptr;
  JumpAction = nullptr;
  LookAction = nullptr;
  SprintAction = nullptr;
  ItemUseAction = nullptr;
  CrouchAction = nullptr;
  ReloadingAction = nullptr;
  SwapAction = nullptr;
  InteractiveAction = nullptr;
  MenuAction = nullptr;
  InventoryAction = nullptr;
  FireAction = nullptr;
  ZoomAction = nullptr;
}

void APepccinePlayerController::BeginPlay()
{
	Super::BeginPlay();

  AddMappingContext();
}

void APepccinePlayerController::AddMappingContext()
{
  if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
  {
    if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
      if (InputMappingContext)
      {
        SubSystem->AddMappingContext(InputMappingContext, 0);
      }
    }
  }
}

void APepccinePlayerController::SetupInputComponent()
{
  UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
  if (!EnhancedInputComponent) return;

  // BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent, UObject* Object, FName FunctionName)
  EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Started, this, &APepccinePlayerController::OpenMenu);
}

void APepccinePlayerController::OpenMenu()
{
  UE_LOG(LogTemp, Log, TEXT("Menu Opened!"));
}