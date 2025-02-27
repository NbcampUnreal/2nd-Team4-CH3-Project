#pragma once

#include "CoreMinimal.h"
#include "Monster/Class/BaseMonster.h"
#include "BossMonster.generated.h"

class UBerserkModeComponent;
class ABossMonsterAIC;

UCLASS()
class PEPCCINE_API ABossMonster : public ABaseMonster
{
	GENERATED_BODY()

public:
	ABossMonster();

protected:
	// ����ȭ ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Berserk")
	UBerserkModeComponent* BerserkModeComponent;
};