#include "Monster/Component/ChargeAttackComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"

UChargeAttackComponent::UChargeAttackComponent()
{
    PrimaryComponentTick.bCanEverTick = true;  // Tick Ȱ��ȭ
    bIsCharging = false;
    ChargeStartTime = 0.0f;
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
    PlaySkillMontage();  // ���� charge �ִϸ��̼� ���
    StartCharge();
}

void UChargeAttackComponent::StartCharge()
{
    ACharacter* OwnerMonster = Cast<ACharacter>(GetOwner());
    if (OwnerMonster)
    {
        // ���� ���� �� �ӵ� ����
        OwnerMonster->GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;

        bIsCharging = true;
        ChargeStartTime = GetWorld()->GetTimeSeconds();

        // ���� �ð� �� ���� ����
        GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UChargeAttackComponent::StopCharge, ChargeDuration, false);
    }
}

void UChargeAttackComponent::StopCharge()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    }

    // �ִϸ��̼� ������ (���� ���� �� ���� �ִϸ��̼� ���)
    PlayEndAnimation();

    bIsCharging = false;
}

void UChargeAttackComponent::PlayEndAnimation()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && ChargeSkillEndMontage)  // EndAnimationMontage�� �����Ϳ��� ������ ���� �ִϸ��̼� ��Ÿ��
        {
            AnimInstance->Montage_Play(ChargeSkillEndMontage);
        }
    }
}

void UChargeAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsCharging)
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            // ���� �� ��� �̵� ó��
            FVector ForwardDirection = OwnerCharacter->GetActorForwardVector();
            OwnerCharacter->AddMovementInput(ForwardDirection, 1.0f);
        }
    }
}

void UChargeAttackComponent::OnHit(AActor* HitActor)
{
    if (HitActor)
    {
        UGameplayStatics::ApplyDamage(HitActor, Damage, nullptr, GetOwner(), nullptr);
        StopCharge();  // �浹 �� ���� ����
    }
}