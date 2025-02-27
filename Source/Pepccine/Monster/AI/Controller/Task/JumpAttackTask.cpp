#include "Monster/AI/Controller/Task/JumpAttackTask.h"
#include "Monster/Component/JumpAttackComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UJumpAttackTask::UJumpAttackTask()
{
    NodeName = TEXT("Jump Attack");
}

EBTNodeResult::Type UJumpAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // BlackBoard���� Ÿ���� ��������
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

    // Ÿ���� �����ϰ�, ���� ���� ������Ʈ�� ���� ���
    if (TargetActor)
    {
        // OwnerComp�� �����ڰ� AIController���� Ȯ��
        AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
        if (AIController)
        {
            APawn* OwnerPawn = AIController->GetPawn();  // AIController���� Pawn�� ��������.
            if (OwnerPawn)
            {
                UJumpAttackComponent* JumpAttackComp = OwnerPawn->FindComponentByClass<UJumpAttackComponent>();
                if (JumpAttackComp)
                {
                    // ���� ���� ����
                    JumpAttackComp->ActivateSkill();
                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}
