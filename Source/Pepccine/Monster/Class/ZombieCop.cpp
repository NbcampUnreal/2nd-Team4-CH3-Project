#include "ZombieCop.h"
#include "Monster/AI/Controller/NormalMonsterAIC.h"
#include "Monster/Component/MonsterAttackComponent.h"

AZombieCop::AZombieCop()
{
	AIControllerClass = ANormalMonsterAIC::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	MonsterClass = EMonsterClass::Normal;
	MonsterType = EMonsterType::LongRange;
}