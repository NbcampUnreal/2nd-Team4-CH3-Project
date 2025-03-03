#pragma once

#include "CoreMinimal.h"
#include "Monster/Component/BaseSkillComponent.h"
#include "GameFramework/Actor.h"
#include "ChargeAttackComponent.generated.h"

UCLASS()
class PEPCCINE_API UChargeAttackComponent : public UBaseSkillComponent
{
    GENERATED_BODY()

public:
    UChargeAttackComponent();

    virtual void ActivateSkill() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    UAnimMontage* ChargeSkillEndMontage;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY(EditAnywhere, Category = "Skill")
    float ChargeSpeed = 1000.0f;  // ���� �ӵ�
    UPROPERTY(EditAnywhere, Category = "Skill")
    float ChargeDuration = 3.0f;  // ���� ���� �ð�

    
    void StartCharge();
    void StopCharge();
    void PlayEndAnimation();
    void OnHit(AActor* HitActor);

    FTimerHandle ChargeTimerHandle;

    bool bIsCharging;  // Charging ���� ����
    float ChargeStartTime;  // Charge ���� �ð� ����
};
