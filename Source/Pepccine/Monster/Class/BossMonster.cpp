#include "Monster/Class/BossMonster.h"
#include "Monster/Component/BerserkModeComponent.h"
#include "Monster/AI/Controller/BossMonsterAIC.h"

ABossMonster::ABossMonster()
{
	// ����ȭ ��� ������Ʈ �ʱ�ȭ
	BerserkModeComponent = CreateDefaultSubobject<UBerserkModeComponent>(TEXT("BerserkModeComponent"));
}
