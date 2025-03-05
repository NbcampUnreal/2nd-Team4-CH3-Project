#include "Monster/Component/JumpAttackComponent.h"
#include "Character/Components/BattleComponent.h"
#include "Character/Player/PepCharacter.h"
#include "GameFramework/Character.h"

UJumpAttackComponent::UJumpAttackComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UJumpAttackComponent::ActivateSkill()
{
    if (!IsCooldownOver())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cooldown not over yet!"));
        return;
    }

    StartCooldown();
    PlaySkillMontage();
}

void UJumpAttackComponent::LaunchMonster()
{
    ACharacter* OwnerMonster = Cast<ACharacter>(GetOwner());
    if (OwnerMonster)
    {

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController found"));

            APawn* PlayerPawn = PlayerController->GetPawn();
            if (PlayerPawn)
            {
                // �÷��̾�� ������ ���� ��ġ�� ���
                FVector PlayerLocation = PlayerPawn->GetActorLocation();
                FVector MonsterLocation = OwnerMonster->GetActorLocation();

                // ��ǥ ��ġ������ ���� ���
                FVector DirectionToPlayer = (PlayerLocation - MonsterLocation).GetSafeNormal();

                // ������ ����� ���� ���
                FVector LaunchDirection = DirectionToPlayer * JumpPower;

                // ���� ���̵� ���� (Z��)
                LaunchDirection.Z = JumpHeight;

                // ���� ����
                OwnerMonster->LaunchCharacter(LaunchDirection, true, true);

                UE_LOG(LogTemp, Warning, TEXT("Jump launched towards player at: %s"), *PlayerLocation.ToString());
            }
        }
    }
}

void UJumpAttackComponent::SkillTrace()
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("JumpAttackComponent: No Owner found!"));
        return;
    }

    FVector Center = Owner->GetActorLocation(); // ���� �߽�
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner); // �ڱ� �ڽ� ����


    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Center, // ���� ��ġ (���� �߽�)
        Center, // ���� ��ġ (���� ��ġ���� �ݰ游 üũ)
        FQuat::Identity, // ȸ�� ����
        ECC_GameTraceChannel1, // �浹 ä��
        FCollisionShape::MakeSphere(AttackRadius), // ���� (��ü)
        QueryParams
    );

    DrawDebugSphere(GetWorld(), Center, AttackRadius, 12, FColor::Green, false, 2.0f);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitActor->GetName());

            DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 2.0f);

            UBattleComponent* TargetBattleComponent = HitActor->FindComponentByClass<UBattleComponent>();
            if (TargetBattleComponent)
            {
                TargetBattleComponent->SendHitResult(HitActor, Damage, HitResult, EMonsterSkill::JumpAttack);
            }
        }
    }
}

