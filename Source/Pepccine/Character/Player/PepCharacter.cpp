#include "PepCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "Item/PepccineItemManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Components/CrosshairHUDComponent.h"
#include "Character/Components/PrograssBarHUDComponent.h"
#include "Character/Components/ItemIconHUDComponent.h"
#include "Character/Components/CollisionRadarComponent.h"
#include "Character/Components/InventoryComponent.h"

#include "Character/Controller/PepccineCameraModifier.h"

#include "GameFramework/SpringArmComponent.h"
#include "Character/Animation/PepccineMontageComponent.h"
#include "Character/Animation/PepccineHitReactionComponent.h"
#include "Character/Components/BattleComponent.h"
#include "Character/Data/ActorInfo.h"
#include "Character/Data/CharacterSaveManager.h"
#include "Item/PepccineDropItem.h"
#include "Item/Passive/PepccinePassiveItemData.h"
#include "Item/Resource/PepccineResourceItemData.h"
#include "Monster/Class/ZombieGirl.h"

APepCharacter::APepCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmCompThird = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmThird"));
	SpringArmCompThird->SetupAttachment(RootComponent);
	SpringArmCompThird->TargetArmLength = CameraArmLength;
	SpringArmCompThird->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArmCompThird, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	SpringArmCompFirst = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmFirst"));
	SpringArmCompFirst->SetupAttachment(RootComponent);
	SpringArmCompFirst->TargetArmLength = CameraArmLength;
	SpringArmCompFirst->bUsePawnControlRotation = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(SpringArmCompFirst);
	FirstPersonCamera->bUsePawnControlRotation = false;

	PlayerStatComponent = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("PlayerStatComponent"));
	CrosshairComponent = CreateDefaultSubobject<UCrosshairHUDComponent>(TEXT("CrosshairHUDComponent"));
	PrograssBarComponent = CreateDefaultSubobject<UPrograssBarHUDComponent>(TEXT("PrograssBarComponent"));
	ItemIconComponent = CreateDefaultSubobject<UItemIconHUDComponent>(TEXT("ItemIconComponent"));

	EnhancedRadarComponent = CreateDefaultSubobject<UCollisionRadarComponent>(TEXT("EnhancedRadarComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	PepccineMontageComponent = CreateDefaultSubobject<UPepccineMontageComponent>(TEXT("MontageComponent"));
	ItemManagerComponent = CreateDefaultSubobject<UPepccineItemManagerComponent>(TEXT("ItemManagerComponent"));

	HitReactionComponent = CreateDefaultSubobject<UPepccineHitReactionComponent>(TEXT("HitReactionComponent"));
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
}

void APepCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeCharacterMovement();
	AddObservers();

	UCharacterSaveManager* SaveManager = GetGameInstance()->GetSubsystem<UCharacterSaveManager>();
	if (bool FirstTimePlay = SaveManager->GetIsFirstTimeLoaded())
	{
		UE_LOG(LogTemp, Warning, TEXT("First Time Loaded [%d]"), FirstTimePlay);
		SaveManager->SetIsFirstTimeLoaded(!FirstTimePlay);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("N Time Loaded [%d]"), FirstTimePlay);
		SaveManager->LoadPlayerStats(PlayerStatComponent->CurrentStats);
		bIsLoaded = true;
	}
	
	UpdateWeaponUI();
	
	// UE_LOG(LogTemp, Warning, TEXT("Player Stats Loaded [%s]"), *PlayerStatComponent->PrintStats());
}

void APepCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UCharacterSaveManager* SaveManager = GetGameInstance()->GetSubsystem<UCharacterSaveManager>();
	SaveManager->SavePlayerStats(PlayerStatComponent->CurrentStats);
}

void APepCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckSprinting();
	CheckRolling(DeltaTime);
}

// Initialize Character Status
#pragma region
void APepCharacter::InitializeCharacterMovement() const
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		if (!PlayerStatComponent)
		{
			return;
		}
		MovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;
		MovementComponent->MaxWalkSpeed = PlayerStatComponent->GetCurrentStats().MovementStats.MovementSpeed;
		MovementComponent->JumpZVelocity = PlayerStatComponent->GetCurrentStats().MovementStats.JumpZVelocity;
	}
}
#pragma endregion

// Tick Method
#pragma region
void APepCharacter::CheckSprinting()
{
	if (!PlayerStatComponent)
	{
		return;
	}
	if (bIsSprinting)
	{
		if (!PlayerStatComponent->DecreaseStamina(0.25))
		{
			bIsSprinting = false;
			return;
		}
		SetCharacterSpeed(PlayerStatComponent->GetCurrentStats().MovementStats.SprintSpeed - LooseWeight);
	}
	else
	{
		SetCharacterSpeed(PlayerStatComponent->GetCurrentStats().MovementStats.MovementSpeed - LooseWeight);
	}
}

void APepCharacter::SetCharacterSpeed(float Speed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(Speed, 0.0f, 1000.0f);
	}
}

void APepCharacter::SetCharacterMovement()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		if (!MovementComponent)
		{
			return;
		}

		const float WalkSpeed = PlayerStatComponent->GetCurrentStats().MovementStats.MovementSpeed - LooseWeight;
		SetCharacterSpeed(WalkSpeed);

		const float JumpZVelocity = PlayerStatComponent->GetCurrentStats().MovementStats.JumpZVelocity - LooseWeight;
		MovementComponent->JumpZVelocity = FMath::Clamp(JumpZVelocity, 0.0f, 1000.0f);
	}
}

void APepCharacter::SetWeight()
{
	if (!PlayerStatComponent)
	{
		return;
	}
	const float WeaponWeight = ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemStats().Weight;
	const float PlayerStrength = PlayerStatComponent->GetCurrentStats().MovementStats.Strength;

	if (PlayerStrength > WeaponWeight)
	{
		LooseWeight = 0.0f;
		return;
	}

	LooseWeight = WeaponWeight - PlayerStrength;

	SetCharacterMovement();
}

void APepCharacter::CheckRolling(float DeltaTime)
{
	if (bIsRolling && PlayerStatComponent)
	{
		float RollTime = PlayerStatComponent->GetCurrentStats().MovementStats.RollElapsedTime;
		RollTime += DeltaTime;

		float RollSpeed = PlayerStatComponent->GetCurrentStats().MovementStats.RollingDistance;
		AddMovementInput(RollDirection, RollSpeed * DeltaTime);
	}
}
#pragma endregion

// Observers
#pragma region
void APepCharacter::AddObservers()
{
	if (PlayerStatComponent)
	{
		PlayerStatComponent->AddStaminaObserver(this);
		PlayerStatComponent->OnHealthChanged.AddDynamic(this, &APepCharacter::OnHealthChanged);
	}

	if (EnhancedRadarComponent)
	{
		EnhancedRadarComponent->OnActorDetectedEnhanced.AddDynamic(this, &APepCharacter::OnActorDetectedEnhanced);
	}

	if (BattleComponent)
	{
		BattleComponent->OnCharacterHited.AddDynamic(this, &APepCharacter::OnPlayerHit);
	}
}

void APepCharacter::OnPlayerHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult,
                                EMonsterSkill SkillType)
{
	if (bIsRolling || !HitReactionComponent || !PepccineMontageComponent)
	{
		return;
	}

	PlayerStatComponent->DecreaseHealth(DamageAmount);

	FVector HitDirection = HitResult.ImpactNormal;

	//HitReactionComponent->HitReaction("Spine", HitDirection);
	Stumble(DamageCauser);
}

void APepCharacter::Stumble(AActor* DamageCauser)
{
	if (!PepccineMontageComponent)
	{
		return;
	}

	bIsStunning = true;

	constexpr float YForce = 500.0f;
	constexpr float ZFore = 300.0f;

	FVector KnockbackForce = GetKnockbackDirection(DamageCauser, this) * YForce;
	GetCharacterMovement()->AddImpulse(KnockbackForce + FVector(0, 0, ZFore), true);

	PepccineMontageComponent->Stumble(2.0);
	TriggerCameraShake(10, 10, 0.5);
}

FVector APepCharacter::GetKnockbackDirection(AActor* DamageSource, AActor* Victim)
{
	if (!DamageSource || !Victim)
	{
		return FVector::ZeroVector;
	}

	// 피해를 받은 캐릭터의 위치
	FVector VictimLocation = Victim->GetActorLocation();
	FVector SourceLocation = DamageSource->GetActorLocation();

	// 피해 방향 (공격자 -> 피해자 벡터)
	FVector KnockbackDir = (VictimLocation - SourceLocation).GetSafeNormal();
	return KnockbackDir;
}

void APepCharacter::OnHealthChanged(const float NewHealth, const float MaxHealth)
{
	if (!PrograssBarComponent) return;
	
	if (NewHealth == 0)
	{
		Dead();
	}

	PrograssBarComponent->SetHealth(NewHealth, MaxHealth);
}

void APepCharacter::OnStaminaChanged(float NewStamina, float MaxStamina)
{
	if (!PrograssBarComponent) return;
	
	//UE_LOG(LogTemp, Warning, TEXT("Stamina Updated: %f / %f"), NewStamina, MaxStamina);
	PrograssBarComponent->SetStamina(NewStamina, MaxStamina);
}

void APepCharacter::OnActorDetectedEnhanced(FDetectedActorList& DetectedActors)
{
	if (DetectedActors.DetectedActors.Num() == 0) return;

	const FDetectedActorInfo* MinDistActor = Algo::MinElement(DetectedActors.DetectedActors,
	                                                          [](const FDetectedActorInfo& InfoA,
	                                                             const FDetectedActorInfo& InfoB)
	                                                          {
		                                                          return InfoA.Distance < InfoB.Distance;
	                                                          });

	if (APepccineDropItem* DropItem = Cast<APepccineDropItem>(MinDistActor->Actor))
	{
		DropItem->ShowInteractWidget(true);

		if (CurrentDropItem && CurrentDropItem != DropItem)
		{
			CurrentDropItem->ShowInteractWidget(false);
		}

		CurrentDropItem = DropItem;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Number of DectedActor: %d"), DetectedActors.DetectedActors.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Detected Actors: [%s]"), *MinDistActor->Actor->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Detected Actors Loc: [%f]"), MinDistActor->Distance);
}
#pragma endregion

// Action
#pragma region
void APepCharacter::Dead()
{
	if (!bIsPlayerAlive)
	{
		return;
	}
	bIsPlayerAlive = false;

	if (APepccinePlayerController* PepccinePlayerController = Cast<APepccinePlayerController>(PlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Dead"));
		// 플레이어 입력 차단
		PepccinePlayerController->DisableInput(PepccinePlayerController);

		// 컨트롤러 회전 입력 차단
		PepccinePlayerController->SetIgnoreLookInput(true);
		PepccinePlayerController->SetIgnoreMoveInput(true);

		// 게임오버 UI 노출
		ShowMenu();
		PepccinePlayerController->ShowGameOver(true);
	}

	if (GetCharacterMovement())
	{
		UE_LOG(LogTemp, Warning, TEXT("Disable Movement"));
		// 이동 멈추기
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}

	PepccineMontageComponent->Death();
}

void APepCharacter::Move(const FInputActionValue& Value)
{
	if (bIsRolling || !bIsPlayerAlive || bIsStunning || bIsClimbing)
	{
		return;
	}
	FVector2D MoveInput = Value.Get<FVector2D>();

	//UE_LOG(LogTemp, Log, TEXT("MovementVector: [%s]"), *MoveInput.ToString());

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

void APepCharacter::Climb()
{
	bIsClimbing = true;
	
	FVector ClimbDirection = GetActorForwardVector() * 300.f;
	FVector ClimbUp = FVector(0.f, 0.f, 700);

	LaunchCharacter(ClimbDirection + ClimbUp, true, true);
	// TriggerCameraShake(3.0f, 3.0f, 0.3f);
	
	PepccineMontageComponent->Climbing();
}

void APepCharacter::JumpStart()
{
	if (bIsRolling || !bIsPlayerAlive || !PepccineMontageComponent || !EnhancedRadarComponent || GetCharacterMovement()->
		IsFalling() || bIsStunning || bIsClimbing) return;

	if (PlayerStatComponent->DecreaseStaminaByPercentage(5))
	{
		if (EnhancedRadarComponent->IsAbleToClimb())
		{
			Climb();
		}
		else
		{
			Super::Jump();
			Jump();
		}
	}
}

void APepCharacter::JumpStop()
{
	Super::StopJumping();

	StopJumping();
}

void APepCharacter::UseItem()
{
	if (!bIsPlayerAlive || bIsStunning)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UseItem!"));
	UpdateWeaponUI();
}

void APepCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	ShotStack = 0;

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void APepCharacter::StartSprint(const FInputActionValue& value)
{
	if (bIsRolling || !bIsPlayerAlive || bIsStunning || bIsClimbing) return;

	if (bIsRollable)
	{
		SprintHoldStartTime = GetWorld()->GetTimeSeconds();
	}

	if (!bIsSprintable) return;
	bIsSprinting = true;
}

void APepCharacter::StopSprint(const FInputActionValue& value)
{
	float HoldTime = GetWorld()->GetTimeSeconds() - SprintHoldStartTime;
	bIsSprinting = false;

	if (HoldTime <= SprintHoldThreshold)
	{
		Roll();
	}

	SprintHoldStartTime = 0.0f;
}

void APepCharacter::Roll()
{
	if (!GetCharacterMovement() || bIsRolling || !PlayerStatComponent || GetCharacterMovement()->IsFalling() || !
		bIsPlayerAlive || bIsStunning || bIsClimbing)
	{
		return;
	}

	bIsRolling = true;
	RollDirection = GetRollDirection();

	if (!PlayerStatComponent->DecreaseStaminaByPercentage(20))
	{
		bIsRolling = false;
		return;
	}

	PepccineMontageComponent->Roll(GetRollDirection(), GetActorRotation());
	GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APepCharacter::EndRoll, 0.5f, false);
}

void APepCharacter::EndRoll()
{
	bIsRolling = false;

	if (GetCharacterMovement())
	{
		SetCharacterSpeed(PlayerStatComponent->GetCurrentStats().MovementStats.MovementSpeed - LooseWeight);
	}
}

FVector APepCharacter::GetRollDirection()
{
	if (!PlayerStatComponent)
	{
		return FVector::ZeroVector;
	}

	FVector Velocity = GetCharacterMovement()->Velocity;

	if (!Velocity.IsNearlyZero())
	{
		RollDirection = Velocity.GetSafeNormal() * PlayerStatComponent->GetCurrentStats().MovementStats.RollingDistance;
	}
	else
	{
		RollDirection = GetActorForwardVector() * PlayerStatComponent->GetCurrentStats().MovementStats.RollingDistance;
	}

	return RollDirection;
}

void APepCharacter::Crouching()
{
	if (!GetCharacterMovement() || bIsRolling || !PlayerStatComponent | !bIsPlayerAlive || bIsStunning || bIsClimbing) return;

	bIsCrouching = GetCharacterMovement()->IsCrouching();

	if (bIsCrouching)
	{
		UnCrouch();
		float WalkSpeed = PlayerStatComponent->GetCurrentStats().MovementStats.MovementSpeed - LooseWeight;
		SetCharacterSpeed(WalkSpeed);
	}
	else
	{
		Crouch();
		float CrouchSpeed = PlayerStatComponent->GetCurrentStats().MovementStats.CrouchSpeed - LooseWeight;
		SetCharacterSpeed(CrouchSpeed);
	}
}

void APepCharacter::Reload()
{
	if (!bIsPlayerAlive || bIsStunning || bIsClimbing || bIsReloading) return;

	//HitReactionComponent->EnterRagdoll(5);

	if (ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemStats().MagazineSize
		== ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemStats().MagazineAmmo) return;

	bIsReloading = true;

	ItemManagerComponent->ReloadWeapon();

	const float WeaponReloadSpeed = ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemStats().ReloadSpeed;
	PepccineMontageComponent->Reloading(WeaponReloadSpeed);

	UpdateWeaponUI();
}

void APepCharacter::Interactive()
{
	if (!bIsPlayerAlive || !PlayerStatComponent || !PepccineMontageComponent || bIsStunning || bIsClimbing) return;

	float ItemWeight = ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemStats().Weight;

	// 아이템 인벤토리에 추가
	if (CurrentDropItem)
	{
		CurrentDropItem->PickUpItem(ItemManagerComponent);
		if (UPepccinePassiveItemData* PassiveItem = Cast<UPepccinePassiveItemData>(CurrentDropItem->GetDropItemData()))
		{
			// 패시브 아이템
			UE_LOG(LogTemp, Warning, TEXT("패시브 아이템 [%s]"), *PlayerStatComponent->PrintStats());

			TArray<FPepccineCharacterStatModifier> CharacterStatModifiers = PassiveItem->GetCharacterStatModifiers();
			for (const FPepccineCharacterStatModifier& Modifier : CharacterStatModifiers)
			{
				float Amount = Modifier.StatModifierDefault.StatModifyValue;
				//UE_LOG(LogTemp, Warning, TEXT("Amount: %f"), Amount);

				EPepccineCharacterStatName CharacterStatName = Modifier.CharacterStatName;
				FString StatName = UEnum::GetValueAsString(CharacterStatName);
				//UE_LOG(LogTemp, Warning, TEXT("CharacterStatName: %s"), *StatName);

				EPepccineStatModifyType ModifyType = Modifier.StatModifierDefault.StatModifyType;
				FString MT = UEnum::GetValueAsString(ModifyType);
				//UE_LOG(LogTemp, Warning, TEXT("ModifyType: %s"), *MT);

				switch (ModifyType)
				{
				case EPepccineStatModifyType::EPSMT_Add:
					{
						//UE_LOG(LogTemp, Warning, TEXT("EPSMT_Add: %f"), Amount);
						FStatModifier AddStatModifier(CharacterStatName, Amount, 1.0f);
						PlayerStatComponent->ApplyStatModifier(AddStatModifier);
						break;
					}
				case EPepccineStatModifyType::EPSMT_Multiply:
					{
						//UE_LOG(LogTemp, Warning, TEXT("EPSMT_Multiply: %f"), Amount);
						FStatModifier MulStatModifier(CharacterStatName, 0.0f, Amount);
						PlayerStatComponent->ApplyStatModifier(MulStatModifier);
						break;
					}
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("패시브 아이템 먹은 후 [%s]"), *PlayerStatComponent->PrintStats());
			// 인벤토리에 추가
			InventoryComponent->AddItem(PassiveItem->GetIconTexture(), PassiveItem->GetDisplayName(),
																	PassiveItem->GetDescription(), PlayerStatComponent->PrintStats());

			TArray<FPepccineCharacterFeature> CharacterFeatures = PassiveItem->GetCharacterFeatures();
			for (const FPepccineCharacterFeature& Feature : CharacterFeatures)
			{
				switch (Feature.CharacterFeatureName)
				{
				case EPepccineCharacterFeatureName::EPCFN_Roll:
					bIsRollable = true;
					break;
				case EPepccineCharacterFeatureName::EPCFN_Sprint:
					bIsSprintable = true;
					break;
				}
			}
		}
		else if (UPepccineWeaponItemData* WeaponItem = Cast<UPepccineWeaponItemData>(CurrentDropItem->GetDropItemData()))
		{
			// 무기류 아이템
			UpdateWeaponUI();
			SetWeight();
		}
		else if (UPepccineResourceItemData* DropItem = Cast<UPepccineResourceItemData>(CurrentDropItem->GetDropItemData()))
		{
			// 리소스 아이템
			switch (DropItem->GetResourceItemType())
			{
			case EPepccineResourceItemType::EPRIT_HealingPotion:
				{
					int32 Amount = DropItem->GetResourceAmount();
					const FStatModifier AddStatModifier(EPepccineCharacterStatName::EPCSN_CurrentHealth, Amount, 1.0f);
					PlayerStatComponent->ApplyStatModifier(AddStatModifier);
					break;
				}
			default:
				break;
			}
		}

		PepccineMontageComponent->Pick();
		/*
		else if (CurrentDropItem->IsA(UPepccineWeaponItemData::StaticClass()))
		{
			// 액티브 아이템
			const UPepccineWeaponItemData* DropItem = Cast<UPepccineWeaponItemData>(CurrentDropItem->GetDropItemData());
		}
		*/
	}

	// Delay 있는 상호작용 전용
	if (bIsInteracting)
	{
	}
	else
	{
	}

	CurrentDropItem = nullptr;
}

void APepCharacter::UpdateWeaponUI()
{
	if (!ItemManagerComponent || !ItemIconComponent) return;

	// 주무기 정보
	UPepccineWeaponItemData* MainWeaponData = ItemManagerComponent->
		GetWeaponItemData(EPepccineWeaponItemType::EPWIT_Main);
	FString MainWeaponName = MainWeaponData ? MainWeaponData->GetDisplayName() : FString("None");
	int32 MainWeaponAmmo = MainWeaponData ? MainWeaponData->GetWeaponItemStats().MagazineAmmo : 0;
	int32 MainSpareAmmo = MainWeaponData ? MainWeaponData->GetWeaponItemStats().SpareAmmo : 0;
	UTexture2D* MainWeaponImage = MainWeaponData ? MainWeaponData->GetIconTexture() : nullptr;

	// 보조무기 정보
	UPepccineWeaponItemData* SubWeaponData = ItemManagerComponent->GetWeaponItemData(EPepccineWeaponItemType::EPWIT_Sub);
	FString SubWeaponName = SubWeaponData ? SubWeaponData->GetDisplayName() : FString("None");
	int32 SubWeaponAmmo = SubWeaponData ? SubWeaponData->GetWeaponItemStats().MagazineAmmo : 0;
	int32 SubWeaponMaxAmmo = SubWeaponData ? SubWeaponData->GetWeaponItemStats().SpareAmmo : 0;
	UTexture2D* SubWeaponImage = SubWeaponData ? SubWeaponData->GetIconTexture() : nullptr;

	// 현재 장착된 무기가 주무기인지 확인
	if (ItemManagerComponent->GetEquippedWeaponItemData())
	{
		bIsMainWeaponEquipped = ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemType() ==
		EPepccineWeaponItemType::EPWIT_Main;

		// WeaponWidget 업데이트
		ItemIconComponent->SetWeaponItem(
			MainWeaponImage,
			SubWeaponImage,
			bIsMainWeaponEquipped ? MainWeaponName : SubWeaponName,
			bIsMainWeaponEquipped ? MainWeaponAmmo : SubWeaponAmmo,
			bIsMainWeaponEquipped ? MainSpareAmmo : SubWeaponMaxAmmo,
			bIsMainWeaponEquipped
		);
	}
}

void APepCharacter::OpenInventory()
{
	if (bIsRolling || !InventoryComponent || bIsStunning || !ItemManagerComponent) return;
	
	if (bIsLoaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 로딩중"));
		// 아이템 매니저에서 패시브 아이템 목록 가져와서 인벤토리에 넣는 작업 1회성
		TMap<int32, UPepccinePassiveItemData*> PassiveItems = ItemManagerComponent->GetPassiveItemDatas();

		for (const auto& PassiveItem : PassiveItems)
		{
			InventoryComponent->AddItem(PassiveItem.Value->GetIconTexture(), PassiveItem.Value->GetDisplayName(),
																	PassiveItem.Value->GetDescription(), PlayerStatComponent->PrintStats());
		}
		
		bIsLoaded = false;
	}

	bIsInventoryOpened = !bIsInventoryOpened;
	InventoryComponent->ToggleInventory();

	if (PlayerController)
	{
		if (bIsInventoryOpened)
		{
			PlayerController->bShowMouseCursor = true;
			PlayerController->SetInputMode(FInputModeGameAndUI());
		}
		else
		{
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
	}
}

void APepCharacter::SwapItem(const FInputActionValue& value)
{
	if (!bIsPlayerAlive || bIsReloading || bIsStunning || bIsClimbing) return;
	
	float ScrollValue = value.Get<float>();
	bIsSwapping = true;

	if (ItemManagerComponent->GetWeaponItemData(EPepccineWeaponItemType::EPWIT_Main) != nullptr &&
		ItemManagerComponent->GetWeaponItemData(EPepccineWeaponItemType::EPWIT_Sub) != nullptr)
	{
		if (bIsMainWeaponEquipped)
		{
			ItemManagerComponent->SwapWeapon(EPepccineWeaponItemType::EPWIT_Sub);
		}
		else
		{
			ItemManagerComponent->SwapWeapon(EPepccineWeaponItemType::EPWIT_Main);
		}

		SetWeight();
		UpdateWeaponUI();

		PepccineMontageComponent->Draw();
	}
}

void APepCharacter::StopFire()
{
	bIsFiring = false;
}

void APepCharacter::Fire()
{
	if (bIsRolling | !bIsPlayerAlive || !PepccineMontageComponent || bIsReloading || bIsStunning || bIsClimbing || bIsSprinting) return;
	bIsFiring = true;

	if (!ItemManagerComponent->GetEquippedWeaponItemData()) return;
	
	float CurrentAmmo = ItemManagerComponent->GetEquippedWeaponItemData()->GetWeaponItemStats().MagazineAmmo;
	if (CurrentAmmo <= 0)
	{
		PepccineMontageComponent->Attack();
		// 근거리 공격 추가
	}
	else
	{
		PepccineMontageComponent->Fire();
		ItemManagerComponent->FireWeapon(PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
	}
	
	UpdateWeaponUI();
}

void APepCharacter::ZoomIn()
{
	if (bIsRolling || !bIsPlayerAlive || bIsStunning || !ItemManagerComponent->GetEquippedWeaponItemData()) return;
	
	bIsZooming = true;

	ToggleCameraView();

	if (!CrosshairComponent)
	{
		return;
	}
	CrosshairComponent->ShowCrosshair();
}

void APepCharacter::ZoomOut()
{
	bIsZooming = false;

	ToggleCameraView();

	if (!CrosshairComponent)
	{
		return;
	}
	CrosshairComponent->HideCrosshair();
}

void APepCharacter::ToggleCameraView()
{
	if (!PlayerController)
	{
		return;
	}
	bIsFirstPersonView = !bIsFirstPersonView;

	FirstPersonCamera->SetActive(bIsFirstPersonView);
	ThirdPersonCamera->SetActive(!bIsFirstPersonView);
}

void APepCharacter::TriggerCameraShake(float Amplitude, float Frequency, float Duration)
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		UPepccineCameraModifier* Modifier
			= Cast<UPepccineCameraModifier>(
				PlayerController->PlayerCameraManager->FindCameraModifierByClass(UPepccineCameraModifier::StaticClass()));
		if (!Modifier)
		{
			Modifier = Cast<UPepccineCameraModifier>(
				PlayerController->PlayerCameraManager->AddNewCameraModifier(UPepccineCameraModifier::StaticClass()));
		}

		if (Modifier)
		{
			Modifier->StartShake(Amplitude, Frequency, Duration);
		}
	}
}

void APepCharacter::ShowMenu()
{
	if (!PlayerController)
	{
		return;
	}
	PlayerController->ToggleExitMenu();
}
#pragma endregion

// Key Mapping
#pragma region
void APepCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	PlayerController = Cast<APepccinePlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

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
			ETriggerEvent::Started,
			this,
			&APepCharacter::Reload
		);
	}

	// Interactive: E
	if (PlayerController->InteractiveAction)
	{
		EnhancedInput->BindAction(
			PlayerController->InteractiveAction,
			ETriggerEvent::Started,
			this,
			&APepCharacter::Interactive
		);
	}

	// Inventory: Tab
	if (PlayerController->InventoryAction)
	{
		EnhancedInput->BindAction(
			PlayerController->InventoryAction,
			ETriggerEvent::Started,
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

	// Fire: Mouse Left
	if (PlayerController->FireAction)
	{
		EnhancedInput->BindAction(
			PlayerController->FireAction,
			ETriggerEvent::Triggered,
			this,
			&APepCharacter::Fire
		);

		EnhancedInput->BindAction(
			PlayerController->FireAction,
			ETriggerEvent::Completed,
			this,
			&APepCharacter::StopFire
		);
	}

	// Zoom: Mouse Right
	if (PlayerController->ZoomAction)
	{
		EnhancedInput->BindAction(
			PlayerController->ZoomAction,
			ETriggerEvent::Started,
			this,
			&APepCharacter::ZoomIn
		);
	}

	// Zoom: Mouse Right
	if (PlayerController->ZoomAction)
	{
		EnhancedInput->BindAction(
			PlayerController->ZoomAction,
			ETriggerEvent::Completed,
			this,
			&APepCharacter::ZoomOut
		);
	}

	// Menu: Escape
	if (PlayerController->MenuAction)
	{
		EnhancedInput->BindAction(
			PlayerController->MenuAction,
			ETriggerEvent::Started,
			this,
			&APepCharacter::ShowMenu
		);
	}
}
#pragma endregion

// Test Code
#pragma region
void APepCharacter::TestApplyStatModifier()
{
	if (!PlayerStatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComponent가 없습니다!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("== 1. 스탯 적용 =="));
	UE_LOG(LogTemp, Log, TEXT("현재 공격력: %f"), PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
	UE_LOG(LogTemp, Log, TEXT("적용 예정: 10.0f, 1.2f"));

	FStatModifier AttackModifier(EPepccineCharacterStatName::EPCSN_AttackDamage, 10.0f, 1.2f);
	PlayerStatComponent->ApplyStatModifier(AttackModifier);

	UE_LOG(LogTemp, Log, TEXT("== 2. 스탯 적용 =="));
	UE_LOG(LogTemp, Log, TEXT("현재 공격력: %f"), PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
	UE_LOG(LogTemp, Log, TEXT("적용 예정: 50.0f, 1.1f"));

	FStatModifier AttackModifierA(EPepccineCharacterStatName::EPCSN_AttackDamage, 50.0f, 1.1f);
	PlayerStatComponent->ApplyStatModifier(AttackModifierA);

	UE_LOG(LogTemp, Log, TEXT("== 3. 스탯 적용 =="));
	UE_LOG(LogTemp, Log, TEXT("현재 공격력: %f"), PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
}

void APepCharacter::TestRemoveStatModifier()
{
	if (!PlayerStatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComponent가 없습니다!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("== 1. 스탯 제거 =="));
	UE_LOG(LogTemp, Log, TEXT("현재 공격력: %f"), PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
	UE_LOG(LogTemp, Log, TEXT("적용 예정: 10.0f, 1.2f"));

	FStatModifier AttackModifier(EPepccineCharacterStatName::EPCSN_AttackDamage, 50.0f, 1.1f);
	PlayerStatComponent->RemoveStatModifier(AttackModifier);

	UE_LOG(LogTemp, Log, TEXT("== 2. 스탯 제거 =="));
	UE_LOG(LogTemp, Log, TEXT("현재 공격력: %f"), PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
	UE_LOG(LogTemp, Log, TEXT("적용 예정: 50.0f, 1.1f"));

	FStatModifier AttackModifierA(EPepccineCharacterStatName::EPCSN_AttackDamage, 10.0f, 1.2f);
	PlayerStatComponent->RemoveStatModifier(AttackModifierA);

	UE_LOG(LogTemp, Log, TEXT("== 3. 스탯 제거 =="));
	UE_LOG(LogTemp, Log, TEXT("현재 공격력: %f"), PlayerStatComponent->GetCurrentStats().CombatStats.AttackDamage);
}
#pragma endregion
