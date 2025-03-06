#include "Monster/Component/HitReactionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

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
                UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
                if (MovementComp)
                {
                    MovementComp->StopMovementImmediately();
                    MovementComp->SetMovementMode(MOVE_None);  // Disable movement
                }
                
                AnimInstance->Montage_Play(HitReactionMontage);
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(this, &UHitReactionComponent::OnHitReactionMontageEnded);
                AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactionMontage);
            }
        }

        HitCount = 0;
    }

    // �˹� ����
    /*ApplyKnockback(HitDirection);*/  // �ʿ��ϸ� �˹� ���� Ȱ��ȭ
}

void UHitReactionComponent::OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
        if (MovementComp)
        {
            MovementComp->SetMovementMode(MOVE_Walking);  // Re-enable movement
        }
    }
}