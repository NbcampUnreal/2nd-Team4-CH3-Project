#include "Character/Components/PlayerStatComponent.h"

#include "Character/Data/CharacterSaveManager.h"
#include "Character/Data/PlayerStatDataAsset.h"

UPlayerStatComponent::UPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	StartRepeatingTimer();
	InitializeStats();
}

void UPlayerStatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(IncreaseStaminaTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DecreaseHealthTimerHandle);
}

void UPlayerStatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckHealthStatus();
}

// Stat
#pragma region
void UPlayerStatComponent::InitializeStats()
{
	// Modifier 총합 초기화
	CurrentTotalAdd = FPlayerStats();
	CurrentTotalAdd.HealthStats.CurrentHealth = 0.0f;
	CurrentTotalAdd.HealthStats.MaxHealth = 0.0f;
	CurrentTotalAdd.HealthStats.HealthDecelerationSpeed = 0.0f;
	CurrentTotalAdd.HealthStats.HealthDecelerationAmount = 0.0f;

	CurrentTotalAdd.StaminaStats.CurrentStamina = 0.0f;
	CurrentTotalAdd.StaminaStats.MaxStamina = 0.0f;
	CurrentTotalAdd.StaminaStats.StaminaRecoveryRate = 0.0f;
	CurrentTotalAdd.StaminaStats.StaminaRecoveryTime = 0.0f;

	CurrentTotalAdd.CombatStats.AttackDamage = 0.0f;
	CurrentTotalAdd.CombatStats.InvincibilityTime = 0.0f;
	CurrentTotalAdd.CombatStats.Defence = 0.0f;

	CurrentTotalAdd.MovementStats.MovementSpeed = 0.0f;
	CurrentTotalAdd.MovementStats.SprintSpeed = 0.0f;
	CurrentTotalAdd.MovementStats.CrouchSpeed = 0.0f;
	CurrentTotalAdd.MovementStats.RollingDistance = 0.0f;
	CurrentTotalAdd.MovementStats.JumpZVelocity = 0.0f;
	CurrentTotalAdd.MovementStats.RollElapsedTime = 0.0f;
	CurrentTotalAdd.MovementStats.Strength = 0.0f;

	CurrentTotalMul = FPlayerStats();
	CurrentTotalMul.HealthStats.CurrentHealth = 1.0f;
	CurrentTotalMul.HealthStats.MaxHealth = 1.0f;
	CurrentTotalMul.HealthStats.HealthDecelerationSpeed = 1.0f;
	CurrentTotalMul.HealthStats.HealthDecelerationAmount = 1.0f;

	CurrentTotalMul.StaminaStats.CurrentStamina = 1.0f;
	CurrentTotalMul.StaminaStats.MaxStamina = 1.0f;
	CurrentTotalMul.StaminaStats.StaminaRecoveryRate = 1.0f;
	CurrentTotalMul.StaminaStats.StaminaRecoveryTime = 1.0f;

	CurrentTotalMul.CombatStats.AttackDamage = 1.0f;
	CurrentTotalMul.CombatStats.InvincibilityTime = 1.0f;
	CurrentTotalMul.CombatStats.Defence = 1.0f;

	CurrentTotalMul.MovementStats.MovementSpeed = 1.0f;
	CurrentTotalMul.MovementStats.SprintSpeed = 1.0f;
	CurrentTotalMul.MovementStats.CrouchSpeed = 1.0f;
	CurrentTotalMul.MovementStats.RollingDistance = 1.0f;
	CurrentTotalMul.MovementStats.JumpZVelocity = 1.0f;
	CurrentTotalMul.MovementStats.RollElapsedTime = 1.0f;
	CurrentTotalMul.MovementStats.Strength = 1.0f;
}

void UPlayerStatComponent::LoadAndApplyPlayerStatDataAsset()
{
	FString AssetPath = TEXT("/Game/Pepccine/Character/Player/Data/PlayerStatDataAsset.PlayerStatDataAsset");
	UPlayerStatDataAsset* LoadedStatAsset = Cast<UPlayerStatDataAsset>(
		StaticLoadObject(UPlayerStatDataAsset::StaticClass(), nullptr, *AssetPath));

	if (LoadedStatAsset)
	{
		SetPlayerStatDataAsset(LoadedStatAsset);
		UE_LOG(LogTemp, Log, TEXT("PlayerStatDataAsset이 성공적으로 로드 및 적용되었습니다!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatDataAsset 로드 실패! 경로를 확인하세요: %s"), *AssetPath);
	}
}

void UPlayerStatComponent::SetPlayerStatDataAsset(UPlayerStatDataAsset* NewDataAsset)
{
	if (!NewDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetPlayerStatDataAsset() 실패: 전달된 DataAsset이 NULL입니다!"));
		CurrentStats = FPlayerStats();
		return;
	}

	PlayerStatDataAsset = NewDataAsset;
	CurrentStats = PlayerStatDataAsset->DefaultStats;
	
	InitializeStats();

	UE_LOG(LogTemp, Log, TEXT("새로운 PlayerStatDataAsset이 적용되었습니다."));
}

void UPlayerStatComponent::RemoveStatModifier(const FStatModifier& Modifier)
{
	ActiveModifiers.Remove(Modifier);
	InitializeStats();

	if (ActiveModifiers.Num() == 0)
	{
		ClampStats();
		return;
	}

	TArray<FStatModifier> ModifiersCopy = ActiveModifiers;

	for (const FStatModifier& Mod : ModifiersCopy)
	{
		ApplyStatModifier(Mod);
	}
}

void UPlayerStatComponent::ApplyStatModifier(const FStatModifier& Modifier)
{
	ActiveModifiers.Add(Modifier);

	FPlayerStats* Add = &CurrentTotalAdd;
	FPlayerStats* Mul = &CurrentTotalMul;

	switch (Modifier.StatType)
	{
	// 체력 관련 스탯
	case EPepccineCharacterStatName::EPCSN_CurrentHealth:
		Add->HealthStats.CurrentHealth += Modifier.AdditiveValue;
		Mul->HealthStats.CurrentHealth *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_MaxHealth:
		Add->HealthStats.MaxHealth += Modifier.AdditiveValue;
		Mul->HealthStats.MaxHealth *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_HealthDecelerationSpeed:
		Add->HealthStats.HealthDecelerationSpeed += Modifier.AdditiveValue;
		Mul->HealthStats.HealthDecelerationSpeed *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_HealthDecelerationAmount:
		Add->HealthStats.HealthDecelerationAmount += Modifier.AdditiveValue;
		Mul->HealthStats.HealthDecelerationAmount *= Modifier.MultiplicativeValue;
		break;

	// 스태미나 관련 스탯
	case EPepccineCharacterStatName::EPCSN_Stamina:
		Add->StaminaStats.CurrentStamina += Modifier.AdditiveValue;
		Mul->StaminaStats.CurrentStamina *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_MaxStamina:
		Add->StaminaStats.MaxStamina += Modifier.AdditiveValue;
		Mul->StaminaStats.MaxStamina *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_StaminaRecoveryRate:
		Add->StaminaStats.StaminaRecoveryRate += Modifier.AdditiveValue;
		Mul->StaminaStats.StaminaRecoveryRate *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_StaminaRecoveryTime:
		Add->StaminaStats.StaminaRecoveryTime += Modifier.AdditiveValue;
		Mul->StaminaStats.StaminaRecoveryTime *= Modifier.MultiplicativeValue;
		break;

	// 전투 관련 스탯
	case EPepccineCharacterStatName::EPCSN_AttackDamage:
		Add->CombatStats.AttackDamage += Modifier.AdditiveValue;
		Mul->CombatStats.AttackDamage *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_Defence:
		Add->CombatStats.Defence += Modifier.AdditiveValue;
		Mul->CombatStats.Defence *= Modifier.MultiplicativeValue;
		break;

	// 이동 관련 스탯
	case EPepccineCharacterStatName::EPCSN_MovementSpeed:
		Add->MovementStats.MovementSpeed += Modifier.AdditiveValue;
		Mul->MovementStats.MovementSpeed *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_SprintSpeed:
		Add->MovementStats.SprintSpeed += Modifier.AdditiveValue;
		Mul->MovementStats.SprintSpeed *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_CrouchSpeed:
		Add->MovementStats.CrouchSpeed += Modifier.AdditiveValue;
		Mul->MovementStats.CrouchSpeed *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_RollingDistance:
		Add->MovementStats.RollingDistance += Modifier.AdditiveValue;
		Mul->MovementStats.RollingDistance *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_JumpZVelocity:
		Add->MovementStats.JumpZVelocity += Modifier.AdditiveValue;
		Mul->MovementStats.JumpZVelocity *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_RollElapsedTime:
		Add->MovementStats.RollElapsedTime += Modifier.AdditiveValue;
		Mul->MovementStats.RollElapsedTime *= Modifier.MultiplicativeValue;
		break;

	case EPepccineCharacterStatName::EPCSN_Strength:
		Add->MovementStats.Strength += Modifier.AdditiveValue;
		Mul->MovementStats.Strength *= Modifier.MultiplicativeValue;
		break;
	
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown StatType: %d"), static_cast<int32>(Modifier.StatType));
		break;
	}

	// 스탯 재계산
	RecalculateStats();
}

void UPlayerStatComponent::RecalculateStats()
{
	FPlayerStats* Base = &CurrentStats;
	FPlayerStats* Add = &CurrentTotalAdd;
	FPlayerStats* Mul = &CurrentTotalMul;

	// 체력 관련 스탯
	Base->HealthStats.CurrentHealth = (Base->HealthStats.CurrentHealth + Add->HealthStats.CurrentHealth) * Mul->HealthStats.CurrentHealth;
	Base->HealthStats.MaxHealth = (Base->HealthStats.MaxHealth + Add->HealthStats.MaxHealth) * Mul->HealthStats.MaxHealth;
	Base->HealthStats.HealthDecelerationSpeed = (Base->HealthStats.HealthDecelerationSpeed + Add->HealthStats.HealthDecelerationSpeed) * Mul->HealthStats.HealthDecelerationSpeed;
	Base->HealthStats.HealthDecelerationAmount = (Base->HealthStats.HealthDecelerationAmount + Add->HealthStats.HealthDecelerationAmount) * Mul->HealthStats.HealthDecelerationAmount;

	// 스태미나 관련 스탯
	Base->StaminaStats.CurrentStamina = (Base->StaminaStats.CurrentStamina + Add->StaminaStats.CurrentStamina) * Mul->StaminaStats.CurrentStamina;
	Base->StaminaStats.MaxStamina = (Base->StaminaStats.MaxStamina + Add->StaminaStats.MaxStamina) * Mul->StaminaStats.MaxStamina;
	Base->StaminaStats.StaminaRecoveryRate = (Base->StaminaStats.StaminaRecoveryRate + Add->StaminaStats.StaminaRecoveryRate) * Mul->StaminaStats.StaminaRecoveryRate;
	Base->StaminaStats.StaminaRecoveryTime = (Base->StaminaStats.StaminaRecoveryTime + Add->StaminaStats.StaminaRecoveryTime) * Mul->StaminaStats.StaminaRecoveryTime;

	// 전투 관련 스탯
	Base->CombatStats.AttackDamage = (Base->CombatStats.AttackDamage + Add->CombatStats.AttackDamage) * Mul->CombatStats.AttackDamage;
	Base->CombatStats.Defence = (Base->CombatStats.Defence + Add->CombatStats.Defence) * Mul->CombatStats.Defence;
	Base->CombatStats.InvincibilityTime = (Base->CombatStats.InvincibilityTime + Add->CombatStats.InvincibilityTime) * Mul->CombatStats.InvincibilityTime;

	// 이동 관련 스탯
	Base->MovementStats.MovementSpeed = (Base->MovementStats.MovementSpeed + Add->MovementStats.MovementSpeed) * Mul->MovementStats.MovementSpeed;
	Base->MovementStats.SprintSpeed = (Base->MovementStats.SprintSpeed + Add->MovementStats.SprintSpeed) * Mul->MovementStats.SprintSpeed;
	Base->MovementStats.CrouchSpeed = (Base->MovementStats.CrouchSpeed + Add->MovementStats.CrouchSpeed) * Mul->MovementStats.CrouchSpeed;
	Base->MovementStats.RollingDistance = (Base->MovementStats.RollingDistance + Add->MovementStats.RollingDistance) * Mul->MovementStats.RollingDistance;
	Base->MovementStats.JumpZVelocity = (Base->MovementStats.JumpZVelocity + Add->MovementStats.JumpZVelocity) * Mul->MovementStats.JumpZVelocity;
	Base->MovementStats.RollElapsedTime = (Base->MovementStats.RollElapsedTime + Add->MovementStats.RollElapsedTime) * Mul->MovementStats.RollElapsedTime;
	Base->MovementStats.Strength = (Base->MovementStats.Strength + Add->MovementStats.Strength) * Mul->MovementStats.Strength;
	
	// 값 보정
	ClampStats();
}

void UPlayerStatComponent::ClampStats()
{
	CurrentStats.HealthStats.MaxHealth = FMath::Clamp(CurrentStats.HealthStats.MaxHealth, 50.0f, 1000.0f);
	CurrentStats.HealthStats.HealthDecelerationAmount = FMath::Clamp(CurrentStats.HealthStats.HealthDecelerationAmount, 0.0f, 20.0f);
	CurrentStats.HealthStats.HealthDecelerationSpeed = FMath::Clamp(CurrentStats.HealthStats.HealthDecelerationSpeed, 0.0f, 1.0f);

	CurrentStats.StaminaStats.StaminaRecoveryTime = FMath::Clamp(CurrentStats.StaminaStats.StaminaRecoveryTime, 1.0f, 2.0f);
	CurrentStats.StaminaStats.MaxStamina = FMath::Clamp(CurrentStats.StaminaStats.MaxStamina, 50.0f, 500.0f);
	CurrentStats.StaminaStats.StaminaRecoveryRate = FMath::Clamp(CurrentStats.StaminaStats.StaminaRecoveryRate, 1.0f,
	                                                             100.0f);

	CurrentStats.CombatStats.InvincibilityTime =
		FMath::Clamp(CurrentStats.CombatStats.InvincibilityTime, 0.0f, 100.0f);
	CurrentStats.CombatStats.AttackDamage = FMath::Clamp(CurrentStats.CombatStats.AttackDamage, 1.0f, 100000.0f);
	CurrentStats.CombatStats.Defence = FMath::Clamp(CurrentStats.CombatStats.Defence, 0.0f, 100.0f);

	CurrentStats.MovementStats.MovementSpeed = FMath::Clamp(CurrentStats.MovementStats.MovementSpeed, 200.0f, 800.0f);
	CurrentStats.MovementStats.SprintSpeed = FMath::Clamp(CurrentStats.MovementStats.SprintSpeed, 400.0f, 1500.0f);
	CurrentStats.MovementStats.CrouchSpeed = FMath::Clamp(CurrentStats.MovementStats.CrouchSpeed, 50.0f, 50.0f);
	CurrentStats.MovementStats.RollingDistance = FMath::Clamp(CurrentStats.MovementStats.RollingDistance, 100.0f, 400.0f);
	CurrentStats.MovementStats.JumpZVelocity = FMath::Clamp(CurrentStats.MovementStats.JumpZVelocity, 200.0f, 1500.0f);
	CurrentStats.MovementStats.RollElapsedTime = FMath::Clamp(CurrentStats.MovementStats.RollElapsedTime, 0.0f, 2.0f);
	CurrentStats.MovementStats.Strength = FMath::Clamp(CurrentStats.MovementStats.Strength, 0.0f, 1000.0f);
}
#pragma endregion

// Timer
#pragma region
void UPlayerStatComponent::StartRepeatingTimer()
{
	GetWorld()->GetTimerManager().SetTimer(IncreaseStaminaTimerHandle,
	                                       this,
	                                       &UPlayerStatComponent::IncreaseStamina_Timer,
	                                       CurrentStats.StaminaStats.StaminaRecoveryTime,
	                                       true);

	GetWorld()->GetTimerManager().SetTimer(DecreaseHealthTimerHandle,
	                                       this,
	                                       &UPlayerStatComponent::DecreaseHealth_Timer,
	                                       CurrentStats.HealthStats.HealthDecelerationSpeed,
	                                       true);
}

void UPlayerStatComponent::IncreaseStamina_Timer()
{
	IncreaseStamina(CurrentStats.StaminaStats.StaminaRecoveryRate);
}

void UPlayerStatComponent::DecreaseHealth_Timer()
{
	DecreaseHealth(CurrentStats.HealthStats.HealthDecelerationAmount);
}
#pragma endregion

// Health
#pragma region
void UPlayerStatComponent::CheckHealthStatus() const
{
	if (CurrentStats.HealthStats.CurrentHealth == 0)
	{
		OnHealthChanged.Broadcast(CurrentStats.HealthStats.CurrentHealth, CurrentStats.HealthStats.MaxHealth);
	}
}

void UPlayerStatComponent::DecreaseHealth(float Amount)
{
	if (Amount <= 0.0f || CurrentStats.HealthStats.CurrentHealth <= 0)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Stat Decrease [%f]"), Amount);

	CurrentStats.HealthStats.CurrentHealth = FMath::Clamp(CurrentStats.HealthStats.CurrentHealth - Amount, 0.0f,
	                                                      CurrentStats.HealthStats.MaxHealth);

	OnHealthChanged.Broadcast(CurrentStats.HealthStats.CurrentHealth, CurrentStats.HealthStats.MaxHealth);
}
#pragma endregion

// Stamina
#pragma region
void UPlayerStatComponent::IncreaseStamina(float Amount)
{
	if (Amount <= 0.0f || CurrentStats.StaminaStats.CurrentStamina >= CurrentStats.StaminaStats.MaxStamina)
	{
		return;
	}

	CurrentStats.StaminaStats.CurrentStamina = FMath::Clamp(CurrentStats.StaminaStats.CurrentStamina + Amount, 0.0f,
	                                                        CurrentStats.StaminaStats.MaxStamina);

	NotifyStaminaObservers();
}

void UPlayerStatComponent::IncreaseStaminaByPercentage(float Percentage)
{
	if (Percentage <= 0.0f || Percentage > 100.0f)
	{
		return;
	}

	float Amount = (CurrentStats.StaminaStats.MaxStamina * (Percentage / 100.0f));
	IncreaseStamina(Amount);
}

bool UPlayerStatComponent::DecreaseStamina(float Amount)
{
	if (Amount <= 0.0f || CurrentStats.StaminaStats.CurrentStamina < Amount)
	{
		return false;
	}

	CurrentStats.StaminaStats.CurrentStamina = FMath::Clamp(CurrentStats.StaminaStats.CurrentStamina - Amount, 0.0f,
	                                                        CurrentStats.StaminaStats.MaxStamina);

	NotifyStaminaObservers();

	return true;
}

bool UPlayerStatComponent::DecreaseStaminaByPercentage(float Percentage)
{
	if (Percentage <= 0.0f || Percentage > 100.0f)
	{
		return false;
	}

	float Amount = (CurrentStats.StaminaStats.MaxStamina * (Percentage / 100.0f));

	return DecreaseStamina(Amount);
}
#pragma endregion

// Observers
#pragma region
void UPlayerStatComponent::AddStaminaObserver(IIStaminaObserver* Observer)
{
	if (Observer && !StaminaObservers.Contains(Observer))
	{
		StaminaObservers.Add(Observer);
	}
}

void UPlayerStatComponent::RemoveStaminaObserver(IIStaminaObserver* Observer)
{
	if (Observer)
	{
		StaminaObservers.Remove(Observer);
	}
}

void UPlayerStatComponent::NotifyStaminaObservers()
{
	for (IIStaminaObserver* Observer : StaminaObservers)
	{
		if (Observer)
		{
			Observer->OnStaminaChanged(CurrentStats.StaminaStats.CurrentStamina, CurrentStats.StaminaStats.MaxStamina);
		}
	}
}
#pragma endregion
