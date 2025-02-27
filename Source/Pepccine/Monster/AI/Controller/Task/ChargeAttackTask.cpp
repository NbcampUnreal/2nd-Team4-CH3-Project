#include "Monster/AI/Controller/Task/ChargeAttackTask.h"
#include "Monster/Component/ChargeAttackComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UChargeAttackTask::UChargeAttackTask()
{
    NodeName = TEXT("Charge Attack");
}

EBTNodeResult::Type UChargeAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // BlackBoard���� Ÿ���� ��������
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

    // Ÿ���� �����ϰ�, ChargeAttackComponent�� ���� ���
    if (TargetActor)
    {
        // OwnerComp�� �����ڰ� AIController���� Ȯ��
        AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
        if (AIController)
        {
            APawn* OwnerPawn = AIController->GetPawn();  // AIController���� Pawn�� ��������.
            if (OwnerPawn)
            {
                UChargeAttackComponent* ChargeAttackComp = OwnerPawn->FindComponentByClass<UChargeAttackComponent>();
                if (ChargeAttackComp)
                {
                    // ���� ���� ����
                    ChargeAttackComp->ActivateSkill();
                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}
