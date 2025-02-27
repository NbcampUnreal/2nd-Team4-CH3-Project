#include "Monster/AI/Controller/Task/MonsterAttackTask.h"
#include "Monster/Component/MonsterAttackComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UMonsterAttackTask::UMonsterAttackTask()
{
    NodeName = TEXT("Monster Attack");
}

EBTNodeResult::Type UMonsterAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // BlackBoard���� Ÿ���� ��������
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

    // Ÿ���� �����ϰ�, ������ AttackComponent�� �ִ� ���
    if (TargetActor)
    {
        // OwnerComp�� �����ڰ� AIController���� Ȯ��
        AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
        if (AIController)
        {
            APawn* OwnerPawn = AIController->GetPawn();  // AIController���� Pawn�� ��������.
            if (OwnerPawn)
            {
                UMonsterAttackComponent* AttackComp = OwnerPawn->FindComponentByClass<UMonsterAttackComponent>();
                if (AttackComp)
                {
                    // ���� ����
                    AttackComp->PerformAttack();
                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}
