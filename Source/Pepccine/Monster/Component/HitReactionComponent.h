#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitReactionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PEPCCINE_API UHitReactionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHitReactionComponent();

    void HandleHitReaction(float DamageAmount);
protected:
    virtual void BeginPlay() override;

    /** �ִ� �ǰ� ī��Ʈ */
    UPROPERTY(EditAnywhere, Category = "Hit Reaction")
    int32 MaxHitCount = 3;  // ���÷� 3��

    /** ����(Stagger) �ð� */
    UPROPERTY(EditAnywhere, Category = "Hit Reaction")
    float StaggerTime = 0.5f;

    /** �˹�(Knockback) ���� */
    UPROPERTY(EditAnywhere, Category = "Hit Reaction")
    float KnockbackStrength = 500.0f;

    /** �ǰ� �ִϸ��̼� */
    UPROPERTY(EditAnywhere, Category = "Hit Reaction")
    UAnimMontage* HitReactionMontage;

private:
    /** �ǰ� ī��Ʈ */
    int32 HitCount = 0;
};
