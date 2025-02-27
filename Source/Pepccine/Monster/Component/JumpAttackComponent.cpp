#include "Monster/Component/JumpAttackComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h" // UGameplayStatics �ʿ�

void UJumpAttackComponent::ActivateSkill()
{
    if (!IsCooldownOver())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cooldown not over yet!"));
        return;
    }

    StartCooldown();
    PlaySkillMontage();

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
