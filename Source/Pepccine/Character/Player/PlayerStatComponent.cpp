// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Player/PlayerStatComponent.h"

UPlayerStatComponent::UPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100;
	MovementSpeed = 400;
	AttackDamage = 100;
	CrouchSpeed = MovementSpeed / 2;
	SprintSpeed = MovementSpeed * 2;
	CurrentHealth = MaxHealth;
	HealthDecelerationSpeed = 0;
	
	MaxStamina = 100.0f;
	Stamina = MaxStamina;
	StaminaRecoveryRate = 2.0f;
	StaminaRecoveryTime = 2.0f;

	InvincibilityTime = 0;
	Defence = 0;  
	FireRate = 0;
	RollingDistance = 10000.0f;
	JumpZVelocity = 600.0f;
}

void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();
	StartRepeatingTimer();
}

void UPlayerStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerStatComponent::StartRepeatingTimer()
{
	GetWorld()->GetTimerManager().SetTimer(IncreaseStaminaTimerHandle,
		[this]() { this->IncreaseStamina(StaminaRecoveryRate); },
		StaminaRecoveryTime,
		true);
}


void UPlayerStatComponent::IncreaseStamina(float Amount)
{
	if (Amount <= 0.0f) return;

	Stamina = FMath::Clamp(Stamina + Amount, 0.0f, MaxStamina);

	NotifyStaminaObservers();
}

void UPlayerStatComponent::IncreaseStaminaByPercentage(float Percentage)
{
	if (Percentage <= 0.0f || Percentage > 100.0f) return;

	float Amount = (MaxStamina * (Percentage / 100.0f));
	IncreaseStamina(Amount);
}

bool UPlayerStatComponent::DecreaseStamina(float Amount)
{
	if (Amount <= 0.0f) return false;
	if (Stamina < Amount) return false;

	Stamina = FMath::Clamp(Stamina - Amount, 0.0f, MaxStamina);

	NotifyStaminaObservers();

	return true;
}

bool UPlayerStatComponent::DecreaseStaminaByPercentage(float Percentage)
{
	if (Percentage <= 0.0f || Percentage > 100.0f) return false;
	
	float Amount = (MaxStamina * (Percentage / 100.0f));

	return DecreaseStamina(Amount);
}

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
			UE_LOG(LogTemp, Warning, TEXT("NotifyStaminaObservers"));
			Observer->OnStaminaChanged(Stamina, MaxStamina);
		}
	}
}