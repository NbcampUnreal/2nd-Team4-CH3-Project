#include "Monster/Component/ChargeAttackComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UChargeAttackComponent::UChargeAttackComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UChargeAttackComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UChargeAttackComponent::ActivateSkill()
{
    if (!IsCooldownOver())
    {
        return;
    }

    StartCooldown();
    PlaySkillMontage();
    StartCharge();
}

void UChargeAttackComponent::StartCharge()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // ������ ���� �̵� �������� ����
        FVector ForwardDirection = OwnerCharacter->GetActorForwardVector();

        // �̵� �ӵ� ���� (MaxWalkSpeed�� �ƴ� CharacterMovementComponent�� AddMovementInput ���)
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;

        // LaunchCharacter�� ����Ͽ� �̵�
        FVector LaunchVelocity = ForwardDirection * ChargeSpeed;
        OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);  // �������� ���� ���� ĳ���� �̵�

        // ���� �ð� �� ���� ����
        GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UChargeAttackComponent::StopCharge, ChargeDuration, false);
    }
}

void UChargeAttackComponent::StopCharge()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // ������ ���߰�, �ٽ� �⺻ �ӵ��� �̵�
        OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 400.0f;  // �⺻ �̵� �ӵ�
    }
}

void UChargeAttackComponent::OnHit(AActor* HitActor)
{
    if (HitActor)
    {
        UGameplayStatics::ApplyDamage(HitActor, ChargeDamage, nullptr, GetOwner(), nullptr);
    }
}
