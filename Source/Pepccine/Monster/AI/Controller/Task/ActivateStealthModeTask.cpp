#include "Monster/AI/Controller/Task/ActivateStealthModeTask.h"
#include "Monster/Component/StealthModeSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UActivateStealthModeTask::UActivateStealthModeTask()
{
    NodeName = TEXT("Activate Stealth Mode");
}

EBTNodeResult::Type UActivateStealthModeTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // BlackBoard���� Ÿ���� ��������
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

    // Ÿ���� �����ϰ�, StealthMode ������Ʈ�� ���� ���
    if (TargetActor)
    {
        // OwnerComp�� �����ڰ� AIController���� Ȯ��
        AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
        if (AIController)
        {
            APawn* OwnerPawn = AIController->GetPawn();  // AIController���� Pawn�� ��������.
            if (OwnerPawn)
            {
                UStealthModeSkillComponent* StealthModeComp = OwnerPawn->FindComponentByClass<UStealthModeSkillComponent>();
                if (StealthModeComp)
                {
                    // ���� ��� ����
                    StealthModeComp->ActivateSkill();
                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}
