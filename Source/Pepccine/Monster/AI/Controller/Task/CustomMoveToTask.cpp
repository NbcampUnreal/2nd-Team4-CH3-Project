#include "Monster/AI/Controller/Task/CustomMoveToTask.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

UCustomMoveToTask::UCustomMoveToTask()
{
    NodeName = TEXT("Custom Move To (With Acceleration)");
    bNotifyTick = true;  // Tick �Լ� Ȱ��ȭ
}

EBTNodeResult::Type UCustomMoveToTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    AICharacter = Cast<ACharacter>(AIController->GetPawn());
    if (!AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // Blackboard���� Target�� ������ (Object �Ǵ� Actor)
    UObject* TargetObject = BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey());
    TargetActor = Cast<AActor>(TargetObject);

    if (!TargetActor)
    {
        return EBTNodeResult::Failed;
    }

    // �׺���̼� ��� ã��
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationPath* NavPath = NavSystem->FindPathToActorSynchronously(GetWorld(), AICharacter->GetActorLocation(), TargetActor);

    if (!NavPath || NavPath->PathPoints.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    // ��� ����
    PathPoints = NavPath->PathPoints;
    CurrentPathIndex = 0;

    // ���ӵ� ����
    AICharacter->GetCharacterMovement()->MaxAcceleration = 2048.0f;
    AICharacter->GetCharacterMovement()->bRequestedMoveUseAcceleration = true;

    return EBTNodeResult::InProgress;
}

void UCustomMoveToTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!AICharacter || !TargetActor || PathPoints.Num() == 0)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // ���� ��ǥ ����
    FVector CurrentTarget = PathPoints[CurrentPathIndex];

    // ���� ��ġ���� ��ǥ ���������� �Ÿ� ���
    float Distance = FVector::Dist(AICharacter->GetActorLocation(), CurrentTarget);

    // ���������� ���� ��η� �̵�
    if (Distance < 100.0f)
    {
        CurrentPathIndex++;

        // ������ ���� ���� �� ���� ó��
        if (CurrentPathIndex >= PathPoints.Num())
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }

        CurrentTarget = PathPoints[CurrentPathIndex];
    }

    // ��ǥ �������� �̵� (���ӵ� �ݿ�)
    FVector Direction = (CurrentTarget - AICharacter->GetActorLocation()).GetSafeNormal();
    FVector NewVelocity = Direction * AICharacter->GetCharacterMovement()->MaxWalkSpeed;

    AICharacter->GetCharacterMovement()->Velocity = FMath::VInterpTo(AICharacter->GetCharacterMovement()->Velocity, NewVelocity, DeltaSeconds, 2.0f);
}
