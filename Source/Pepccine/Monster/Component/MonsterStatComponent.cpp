#include "Monster/Component/MonsterStatComponent.h"
#include "Monster/AI/Controller/BossMonsterAIC.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Monster/Class/BaseMonster.h"

UMonsterStatComponent::UMonsterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;  // �ʱ� ü�� = �ִ� ü��
}

void UMonsterStatComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Monster Stat On!"));
}


void UMonsterStatComponent::DecreaseHealth(float Amount)
{
    float FinalDamage = FMath::Max(Amount - Defense, 1.0f);
    CurrentHealth = FMath::Max(CurrentHealth - FinalDamage, 0.0f);
    UE_LOG(LogTemp, Warning, TEXT("Monster HP: %f"), CurrentHealth);

    if (ABaseMonster* OwnerMonster = Cast<ABaseMonster>(GetOwner())) 
    {
        OwnerMonster->UpdateHealthBar(CurrentHealth, MaxHealth);
        OwnerMonster->HealthBarWidgetComp->SetHiddenInGame(false);

        // CheckHealth 호출
        CheckHealth(OwnerMonster);
    }
}

void UMonsterStatComponent::IncreaseHealth(float Amount)
{
    CurrentHealth = FMath::Min(CurrentHealth + Amount, MaxHealth);

    if (ABaseMonster* OwnerActor = Cast<ABaseMonster>(GetOwner())) 
    {
        OwnerActor->UpdateHealthBar(CurrentHealth, MaxHealth);
        OwnerActor->HealthBarWidgetComp->SetHiddenInGame(false);
    }
}

bool UMonsterStatComponent::IsDead() const
{
    return CurrentHealth <= 0.0f;
}

void UMonsterStatComponent::CheckHealth(ABaseMonster* OwnerMonster)
{
    if (!OwnerMonster) return;

    // BossMonsterAIC에 접근하여 CanBerserk 값을 설정
    ABossMonsterAIC* BossAIController = Cast<ABossMonsterAIC>(OwnerMonster->GetController());
    if (BossAIController)
    {
        float HealthPercentage = CurrentHealth / MaxHealth;
        if (HealthPercentage <= 0.4f)
        {
            // CanBerserk를 True로 설정
            BossAIController->SetCanBerserk(true);
            UE_LOG(LogTemp, Warning, TEXT("Boss is in Berserk Mode!"));
        }
    }
}