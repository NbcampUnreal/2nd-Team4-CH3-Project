#pragma once

#include "CoreMinimal.h"
#include "Monster/Component/BaseSkillComponent.h"
#include "StealthModeSkillComponent.generated.h"

UCLASS(Blueprintable)
class PEPCCINE_API UStealthModeSkillComponent : public UBaseSkillComponent
{
	GENERATED_BODY()

public:
	UStealthModeSkillComponent();
	virtual void ActivateSkill() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Stealth")
	float StealthTime = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Stealth")
	float SpeedMultiplier = 1.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill|Stealth")
	bool bIsStealthMode = false;

	// �������Ʈ���� ��Ƽ���� Dissolve �� ���� ó��
	UFUNCTION(BlueprintImplementableEvent, Category = "Skill|Stealth")
	void OnStealthActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill|Stealth")
	void OnStealthDeactivated();

	FTimerHandle StealthTimerHandle;

	virtual void BeginPlay() override;
	void BoostSpeed(float Amount);
	void ResetSpeed();
};
