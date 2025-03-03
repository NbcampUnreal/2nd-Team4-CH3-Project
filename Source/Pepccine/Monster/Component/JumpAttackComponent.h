#pragma once

#include "CoreMinimal.h"
#include "Monster/Component/BaseSkillComponent.h"
#include "JumpAttackComponent.generated.h"

UCLASS()
class PEPCCINE_API UJumpAttackComponent : public UBaseSkillComponent
{
	GENERATED_BODY()

public:
	virtual void ActivateSkill() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float JumpPower = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float JumpHeight = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float AttackRadius = 10.0f;
};
