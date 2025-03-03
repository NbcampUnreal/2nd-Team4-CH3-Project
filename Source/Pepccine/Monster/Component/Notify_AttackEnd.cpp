#include "Monster/Component/Notify_AttackEnd.h"
#include "Monster/Component/MonsterAttackComponent.h"
#include "Monster/Component/BaseSkillComponent.h"
#include "GameFramework/Actor.h"

void UNotify_AttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp)
    {
        AActor* Owner = MeshComp->GetOwner();
        if (Owner)
        {
            // 1. �Ϲ� ������ ��� MonsterAttackComponent ó��
            UMonsterAttackComponent* AttackComp = Owner->FindComponentByClass<UMonsterAttackComponent>();
            if (AttackComp)
            {
                UE_LOG(LogTemp, Log, TEXT("Monster Normal Attack!"));
                AttackComp->AttackTrace();
            }

            // 2. ��ų ������ ��� BaseSkillComponent ó��
            UBaseSkillComponent* SkillComp = Owner->FindComponentByClass<UBaseSkillComponent>();
            if (SkillComp)
            {
                UE_LOG(LogTemp, Log, TEXT("Monster SKill Attack!"));
                // ��ų�� ���� ���� �߰� �ʿ�
            }
        }
    }
}