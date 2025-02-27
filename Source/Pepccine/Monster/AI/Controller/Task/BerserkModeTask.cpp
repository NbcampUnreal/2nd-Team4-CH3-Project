#include "Monster/AI/Controller/Task/BerserkModeTask.h"
#include "Monster/Component/BerserkModeComponent.h"
#include "AIController.h"

UBerserkModeTask::UBerserkModeTask()
{
    NodeName = TEXT("Berserk Mode Change");
}

EBTNodeResult::Type UBerserkModeTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // OwnerComp�� �����ڰ� AIController���� Ȯ��
    AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
    if (AIController)
    {
        APawn* OwnerPawn = AIController->GetPawn();  // AIController���� Pawn�� ��������.
        if (OwnerPawn)
        {
            UBerserkModeComponent* BerserkModeComp = OwnerPawn->FindComponentByClass<UBerserkModeComponent>();
            if (BerserkModeComp)
            {
                // ����ũ ��� ����
                BerserkModeComp->EnterBerserkMode();
                return EBTNodeResult::Succeeded;
            }
        }
    }
    return EBTNodeResult::Failed;
}