#include "Monster/Component/MonsterAttackComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Player/PepCharacter.h" 
#include "DrawDebugHelpers.h"

UMonsterAttackComponent::UMonsterAttackComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterAttackComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UMonsterAttackComponent::PerformAttack()
{
    // �����ڸ� ACharacter�� ��ȯ
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner is NOT an ACharacter!"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("%s is attacking!"), *OwnerCharacter->GetName());

    if (AttackMontage)
    {
        OwnerCharacter->PlayAnimMontage(AttackMontage);
    }
}

void UMonsterAttackComponent::ApplyDamageToTarget(AActor* Target, float DamageAmount)
{
    if (Target)
    {
        // ������ ����
        UGameplayStatics::ApplyDamage(Target, DamageAmount, nullptr, GetOwner(), nullptr);

        APepCharacter* Player = Cast<APepCharacter>(Target);
        if (Player)
        {
            // �÷��̾� �� �ǰ� ����
        }
    }
}

void UMonsterAttackComponent::PlayAttackMontage()
{
    if (AttackMontage && GetOwner())
    {
        AActor* Owner = GetOwner();
        USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();

        if (MeshComp && MeshComp->GetAnimInstance())
        {
            UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
            AnimInstance->Montage_Play(AttackMontage);
        }
    }
}

void UMonsterAttackComponent::AttackTrace()
{
    ACharacter* OwnerMonster = Cast<ACharacter>(GetOwner());
    if (OwnerMonster == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner is NOT an ACharacter!"));
        return;
    }

    FVector StartLocation = OwnerMonster->GetActorLocation(); 
    FVector ForwardVector = OwnerMonster->GetActorForwardVector(); 
    FVector EndLocation = StartLocation + (ForwardVector * 100.0f);  // ���� ���� 

    // ĸ�� Ʈ���̽� �Ű�����
    float CapsuleRadius = 30.0f; 
    float CapsuleHalfHeight = 100.0f; 

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(OwnerMonster); // ���ʹ� �浹 ����

    // ĸ�� Ʈ���̽� ����
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        FQuat::Identity,
        ECC_Pawn, 
        FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), 
        CollisionParams
    );

    // �浹 �� ó��
    if (bHit)
    {
        FVector ImpactPoint = HitResult.ImpactPoint;

        // �浹 ������ ���� ���� �׷��� �ð������� ǥ�� (�浹�� ���� ����)
        DrawDebugSphere(
            GetWorld(),
            ImpactPoint,
            CapsuleRadius,
            12, 
            FColor::Green,  
            false, 
            1.0f  
        );

        // �浹 ����� �÷��̾��� ��� ������ ����
        if (APepCharacter* Player = Cast<APepCharacter>(HitResult.GetActor()))
        {
            ApplyDamageToTarget(Player, 20.0f);  // ����: 20�� ������ ����
        }
    }
    else
    {
        DrawDebugCapsule(
            GetWorld(),
            EndLocation, 
            CapsuleHalfHeight, 
            CapsuleRadius,
            FQuat::Identity,
            FColor::Red,
            false, 
            1.0f,  
            0,     
            1.0f  
        );

        // �α� ���
        UE_LOG(LogTemp, Log, TEXT("No hit detected. Trace from (%s) to (%s)"), *StartLocation.ToString(), *EndLocation.ToString());
    }
}
