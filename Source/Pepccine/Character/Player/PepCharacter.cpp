
#include "PepCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Pepccine/Character/Controller/PepccinePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
<<<<<<< Updated upstream
=======
#include "CrosshairHUDComponent.h"
#include "Character/Animation/PepccineMontagesComponent.h"
>>>>>>> Stashed changes
#include "GameFramework/SpringArmComponent.h"

APepCharacter::APepCharacter()
{
  PrimaryActorTick.bCanEverTick = true;

  SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
  SpringArmComp->SetupAttachment(RootComponent);
  SpringArmComp->TargetArmLength = CameraArmLength;
  SpringArmComp->bUsePawnControlRotation = true;

  CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
  CameraComp->bUsePawnControlRotation = false;

  PlayerStatComponent = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("PlayerStatComponent"));
<<<<<<< Updated upstream
  
=======

  CrosshairHUDComponent = CreateDefaultSubobject<UCrosshairHUDComponent>(TEXT("CrosshairHUDComponent"));

  PepccineMontagesComponent = CreateDefaultSubobject<UPepccineMontagesComponent>(TEXT("PepccineMontagesComponent"));
>>>>>>> Stashed changes
}

void APepCharacter::BeginPlay()
{
  Super::BeginPlay();

  DefineCharacterMovement();
}

void APepCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

void APepCharacter::DefineCharacterMovement()
{
  if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
  {
    MovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;
    MovementComponent->MaxWalkSpeed = PlayerStatComponent->MovementSpeed;
    MovementComponent->JumpZVelocity = PlayerStatComponent->JumpZVelocity;
  }
}

void APepCharacter::Move(const FInputActionValue& Value)
{
  FVector2D MoveInput = Value.Get<FVector2D>();

  UE_LOG(LogTemp, Log, TEXT("MovementVector: [%s]"), *MoveInput.ToString());

  bool bWasMoving = bIsMoving;
  bIsMoving = !FMath::IsNearlyZero(MoveInput.X) || !FMath::IsNearlyZero(MoveInput.Y); // 1, 1, 1, 0

  if (bWasMoving && !bIsMoving)
  {
    OnMovementStopped();
  }

  if (!FMath::IsNearlyZero(MoveInput.X))
  {
    AddMovementInput(GetActorForwardVector(), MoveInput.X);
  }

  if (!FMath::IsNearlyZero(MoveInput.Y))
  {
    AddMovementInput(GetActorRightVector(), MoveInput.Y);
  }
}

void APepCharacter::OnMovementStopped()
{
  UE_LOG(LogTemp, Log, TEXT("Movement Stopped!"));
}

void APepCharacter::JumpStart()
{
  Super::Jump();

  UE_LOG(LogTemp, Log, TEXT("JumpStart!"));
  bIsJumping = true;
  Jump();
}

void APepCharacter::JumpStop()
{
  Super::StopJumping();
  bIsJumping = false;
  StopJumping();
  UE_LOG(LogTemp, Log, TEXT("JumpStop!"));
}

void APepCharacter::UseItem()
{
  UE_LOG(LogTemp, Log, TEXT("UseItem!"));
}

void APepCharacter::Look(const FInputActionValue& value)
{
  FVector2D LookInput = value.Get<FVector2D>();

  UE_LOG(LogTemp, Log, TEXT("LookInput[%s]"), *LookInput.ToString());

  AddControllerYawInput(LookInput.X);
  AddControllerPitchInput(LookInput.Y);
}

void APepCharacter::StartSprint(const FInputActionValue& value)
{
  if (bIsRollable)
  {
    SprintHoldStartTime = GetWorld()->GetTimeSeconds();
    UE_LOG(LogTemp, Log, TEXT("StartSprint HoldTime! [%f]"), SprintHoldStartTime);
  }

  if (!bIsSprintable) return;

  bIsSpringting = true;

  UE_LOG(LogTemp, Log, TEXT("StartSprint!"));

  if (GetCharacterMovement())
  {
    GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->SprintSpeed;
  }
}

void APepCharacter::StopSprint(const FInputActionValue& value)
{
  float HoldTime = GetWorld()->GetTimeSeconds() - SprintHoldStartTime;
  UE_LOG(LogTemp, Log, TEXT("SprintHoldStartTime! [%f]"), SprintHoldStartTime);
  UE_LOG(LogTemp, Log, TEXT("HoldTime! [%f]"), HoldTime);

  if (HoldTime > SprintHoldThreshold)
  {
    UE_LOG(LogTemp, Log, TEXT("StopSprint!"));
    bIsSpringting = false;

    if (GetCharacterMovement())
    {
      GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->MovementSpeed;
    }
  }
  else
  {
    UE_LOG(LogTemp, Log, TEXT("Rolling!"));
    Roll();
  }

  SprintHoldStartTime = 0.0f; // ÃÊ±âÈ­
}

void APepCharacter::Roll()
{
  if (!GetCharacterMovement() || bIsRolling) return;

  bIsRolling = true;

  GetCharacterMovement()->AddImpulse(GetRollDirection(), true);
  GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APepCharacter::EndRoll, 0.1f, false);
}

FVector APepCharacter::GetRollDirection()
{
  FVector Velocity = GetCharacterMovement()->Velocity;
  FVector RollDirection;

  if (!Velocity.IsNearlyZero())
  {
    RollDirection = Velocity.GetSafeNormal() * PlayerStatComponent->RollingDistance;
  }
  else
  {
    RollDirection = GetActorForwardVector() * PlayerStatComponent->RollingDistance;
  }

  UE_LOG(LogTemp, Log, TEXT("RollDirection! [%s]"), *RollDirection.ToString());

  return RollDirection;
}

void APepCharacter::EndRoll()
{
  bIsRolling = false;
  UE_LOG(LogTemp, Log, TEXT("Roll Ended!"));
}

void APepCharacter::Crouching()
{
  if (!GetCharacterMovement())
    return;

  bIsCrouching = GetCharacterMovement()->IsCrouching();

  if (bIsCrouching)
  {
    UnCrouch();
    UE_LOG(LogTemp, Log, TEXT("UnCrouch! [%d][%f]"), bIsCrouching, PlayerStatComponent->MovementSpeed);
    GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->MovementSpeed;
  }
  else
  {
    Crouch();
    UE_LOG(LogTemp, Log, TEXT("Crouch! [%d][%f]"), bIsCrouching, PlayerStatComponent->CrouchSpeed);
    GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->CrouchSpeed;
  }
}

void APepCharacter::Reload()
{
  UE_LOG(LogTemp, Log, TEXT("Reload!"));

  if (bIsReloading)
  {

  }
  else
  {
      if (PepccineMontagesComponent)
      {
          PepccineMontagesComponent->PlayReloadMontage();
      }
  }
}

void APepCharacter::Interactive()
{
  UE_LOG(LogTemp, Log, TEXT("Interactive!"));

  if (bIsInteracting)
  {

  }
  else
  {

  }
}

void APepCharacter::OpenInventory()
{
  UE_LOG(LogTemp, Log, TEXT("OpenInventory!"));

  // HUD
  if (bIsInventoryOpened)
  {
    
  }
  else
  {

  }
}

void APepCharacter::SwapItem(const FInputActionValue& value)
{
  UE_LOG(LogTemp, Warning, TEXT("SwapItem"));
  float ScrollValue = value.Get<float>();

  if (ScrollValue > 0.0f)
  {
    UE_LOG(LogTemp, Log, TEXT("Swapping Forward [%f]"), ScrollValue);
  }
  else if (ScrollValue < 0.0f)
  {
    UE_LOG(LogTemp, Log, TEXT("Swapping Backward [%f]"), ScrollValue);
  }
}

<<<<<<< Updated upstream
=======
void APepCharacter::Fire()
{
  UE_LOG(LogTemp, Log, TEXT("Fire!"));
  PepccineMontagesComponent->PlayFireMontage();
}

void APepCharacter::ZoomIn()
{
  UE_LOG(LogTemp, Log, TEXT("ZoomIn!"));

  bIsZooming = true;
  ToggleCameraView();
}

void APepCharacter::ZoomOut()
{
  UE_LOG(LogTemp, Log, TEXT("ZoomOut!"));

  bIsZooming = false;
  ToggleCameraView();
}

void APepCharacter::ToggleCameraView()
{
  if (!PlayerController) return;

  bIsFirstPersonView = !bIsFirstPersonView;

  FirstPersonCamera->SetActive(bIsFirstPersonView);
  ThirdPersonCamera->SetActive(!bIsFirstPersonView);
}
#pragma endregion

// Key Mapping
#pragma region
>>>>>>> Stashed changes
void APepCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
  if (!EnhancedInput) return;

  APepccinePlayerController* PlayerController = Cast<APepccinePlayerController>(GetController());
  if (!PlayerController) return;

  // MoveAction: W A S D
  if (PlayerController->MoveAction)
  {
    EnhancedInput->BindAction(
      PlayerController->MoveAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::Move
    );
  }

  // LookAction: Mouse2D
  if (PlayerController->LookAction)
  {
    EnhancedInput->BindAction(
      PlayerController->LookAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::Look
    );
  }

  // Sprint: Shift
  if (PlayerController->SprintAction)
  {
    EnhancedInput->BindAction(
      PlayerController->SprintAction,
      ETriggerEvent::Started,
      this,
      &APepCharacter::StartSprint
    );
  }

  if (PlayerController->SprintAction)
  {
    EnhancedInput->BindAction(
      PlayerController->SprintAction,
      ETriggerEvent::Completed,
      this,
      &APepCharacter::StopSprint
    );
  }

  // Jump: Space
  if (PlayerController->JumpAction)
  {
    EnhancedInput->BindAction(
      PlayerController->JumpAction,
      ETriggerEvent::Started,
      this,
      &APepCharacter::JumpStart
    );
  }

  if (PlayerController->JumpAction)
  {
    EnhancedInput->BindAction(
      PlayerController->JumpAction,
      ETriggerEvent::Completed,
      this,
      &APepCharacter::JumpStop
    );
  }

  // UseItem: Q
  if (PlayerController->ItemUseAction)
  {
    EnhancedInput->BindAction(
      PlayerController->ItemUseAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::UseItem
    );
  }

  // Crouch: Ctrl
  if (PlayerController->CrouchAction)
  {
    EnhancedInput->BindAction(
      PlayerController->CrouchAction,
      ETriggerEvent::Started,
      this,
      &APepCharacter::Crouching
    );
  }

  // Reload: R
  if (PlayerController->ReloadingAction)
  {
    EnhancedInput->BindAction(
      PlayerController->ReloadingAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::Reload
    );
  }

  // Interactive: E
  if (PlayerController->InteractiveAction)
  {
    EnhancedInput->BindAction(
      PlayerController->InteractiveAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::Interactive
    );
  }

  // Inventory: Tab
  if (PlayerController->InventoryAction)
  {
    EnhancedInput->BindAction(
      PlayerController->InventoryAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::OpenInventory
    );
  }

  // Swap: Mouse Wheel
  if (PlayerController->SwapAction)
  {
    EnhancedInput->BindAction(
      PlayerController->SwapAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::SwapItem
    );
  }
}