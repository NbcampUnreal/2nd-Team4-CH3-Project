#include "Monster/Component/BaseSkillComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

UBaseSkillComponent::UBaseSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    LastUsedTime = 0; // ó���� ��Ÿ���� ����ǵ��� 0���� ����
}

void UBaseSkillComponent::BeginPlay()
{
	Super::BeginPlay();
}


bool UBaseSkillComponent::IsCooldownOver() const
{
    return (GetWorld()->GetTimeSeconds() - LastUsedTime) >= CooldownTime;
}

void UBaseSkillComponent::StartCooldown()
{
    LastUsedTime = GetWorld()->GetTimeSeconds();
}

void UBaseSkillComponent::PlaySkillMontage()
{
    if (SkillMontage && GetOwner())
    {
        AActor* Owner = GetOwner();
        USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();

        if (MeshComp && MeshComp->GetAnimInstance())
        {
            UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
            AnimInstance->Montage_Play(SkillMontage);
        }
    }
}

void UBaseSkillComponent::ApplyDamageToTarget(AActor* Target)
{
    if (Target)
    {
        // ���� �ڵ�: Ÿ�ٿ� �������� �ִ� ���� (�ּ� ó��)
        /*
        float DamageAmount = 20.0f; // ���� ������ ��ġ
        UGameplayStatics::ApplyDamage(Target, DamageAmount, nullptr, GetOwner(), nullptr);
        */
    }
}