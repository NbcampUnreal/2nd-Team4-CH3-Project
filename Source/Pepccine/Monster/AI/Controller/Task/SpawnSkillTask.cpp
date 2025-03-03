#include "Monster/AI/Controller/Task/SpawnSkillTask.h"
#include "Monster/Component/SpawnSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USpawnSkillTask::USpawnSkillTask()
{
    NodeName = TEXT("Spawn Skill");
}

EBTNodeResult::Type USpawnSkillTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // BlackBoard���� Ÿ���� ��������
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

    // Ÿ���� �����ϰ�, ���� ��ų ������Ʈ�� ���� ���
    if (TargetActor)
    {
        // OwnerComp�� �����ڰ� AIController���� Ȯ��
        AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
        if (AIController)
        {
            APawn* OwnerPawn = AIController->GetPawn();  // AIController���� Pawn�� ��������.
            if (OwnerPawn)
            {
                USpawnSkillComponent* SpawnSkillComp = OwnerPawn->FindComponentByClass<USpawnSkillComponent>();
                if (SpawnSkillComp)
                {
                    // ���� ���� ����
                    SpawnSkillComp->ActivateSkill();
                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}