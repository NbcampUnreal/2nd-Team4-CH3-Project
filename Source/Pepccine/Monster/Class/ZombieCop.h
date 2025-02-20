#pragma once

#include "CoreMinimal.h"
#include "BaseMonster.h"
#include "ZombieCop.generated.h"

class UMonsterAttackComponent;  // ���� ����

UCLASS()
class PEPCCINE_API AZombieCop : public ABaseMonster
{
	GENERATED_BODY()
	
public: 
	AZombieCop();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMonsterAttackComponent* AttackComponent;
};
