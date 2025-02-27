// HitReactionComponent.cpp
#include "Monster/Component/HitReactionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UHitReactionComponent::UHitReactionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHitReactionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UHitReactionComponent::HandleHitReaction(float DamageAmount)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;

    HitCount++;

    // ��Ʈ ī��Ʈ�� �ִ밪�� �������� ���� �ִϸ��̼� ���
    if (HitCount >= MaxHitCount)
    {
        if (HitReactionMontage)
        {
            UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                AnimInstance->Montage_Play(HitReactionMontage);
            }
        }

        HitCount = 0;
    }

    // �˹� ����
    /*ApplyKnockback(HitDirection);*/  // �ʿ��ϸ� �˹� ���� Ȱ��ȭ
}
