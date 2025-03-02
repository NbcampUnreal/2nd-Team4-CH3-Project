#include "Monster/Component/SpawnSkillComponent.h"
#include "Monster/Class/BaseMonster.h"

void USpawnSkillComponent::ActivateSkill()
{
    if (!IsCooldownOver())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cooldown not over yet!"));
        return;
    }

    StartCooldown();
    PlaySkillMontage();
    SpawnMonster();
}

void USpawnSkillComponent::SpawnMonster()
{
    if (!MonsterToSpawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Monster class not set!"));
        return;
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        FVector SpawnLocation = OwnerCharacter->GetActorLocation() + FVector(200.0f, 0.0f, 0.0f); // ��ȯ ��ġ�� ĳ���� �������� ����
        FRotator SpawnRotation = FRotator::ZeroRotator;

        for (int i = 0; i < NumberOfMonsters; i++)
        {
            // ��ȯ
            ABaseMonster* SpawnedMonster = GetWorld()->SpawnActor<ABaseMonster>(MonsterToSpawn, SpawnLocation, SpawnRotation);
            if (SpawnedMonster)
            {
                UE_LOG(LogTemp, Warning, TEXT("Monster spawned at: %s"), *SpawnLocation.ToString());
                FadeInMonster(SpawnedMonster);
            }
        }
    }
}