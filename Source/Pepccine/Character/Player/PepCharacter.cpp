
#include "PepCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Pepccine/Character/Controller/PepccinePlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APepCharacter::APepCharacter()
{
  PrimaryActorTick.bCanEverTick = true;

  SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
  SpringArmComp->SetupAttachment(RootComponent);
  SpringArmComp->TargetArmLength = 300.0f;
  SpringArmComp->bUsePawnControlRotation = true;

  CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
  CameraComp->bUsePawnControlRotation = false;
}

void APepCharacter::BeginPlay()
{
  Super::BeginPlay();

}

void APepCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

void APepCharacter::Move(const FInputActionValue& Value)
{
  FVector2D MovementVector = Value.Get<FVector2D>();

  UE_LOG(LogTemp, Log, TEXT("MovementVector: [%s]"), *MovementVector.ToString());

  // test
  AddMovementInput(FVector(MovementVector.X, MovementVector.Y, 0.0f));
}

void APepCharacter::JumpStart()
{
  Super::Jump();

  UE_LOG(LogTemp, Log, TEXT("JumpStart!"));
}

void APepCharacter::JumpStop()
{
  Super::StopJumping();

  UE_LOG(LogTemp, Log, TEXT("JumpStop!"));
}

void APepCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
  if (!EnhancedInput) return;

  APepccinePlayerController* PlayerController = Cast<APepccinePlayerController>(GetController());
  if (!PlayerController) return;

  // MoveAction
  if (PlayerController->MoveAction)
  {
    EnhancedInput->BindAction(
      PlayerController->MoveAction,
      ETriggerEvent::Triggered,
      this,
      &APepCharacter::Move
    );
  }

  // Jump
  if (PlayerController->JumpAction)
  {
    EnhancedInput->BindAction(
      PlayerController->JumpAction,
      ETriggerEvent::Triggered,
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
}
