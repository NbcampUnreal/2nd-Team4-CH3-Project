#pragma once

#include "CoreMinimal.h"
#include "ItemStat.generated.h"

/** ������ ���� �̸� */
UENUM(BlueprintType)
enum class EItemStatName : uint8
{
	AttackMultiplier UMETA(DisplayName = "���ݷ� ����"),
	RangeMultiplier UMETA(DisplayName = "��Ÿ� ����"),
	FireRateMultiplier UMETA(DisplayName = "���� ����"),
	ZoomMultiplier UMETA(DisplayName = "Ȯ�� ����"),
	MagazineCapacity UMETA(DisplayName = "źâ"),
	BulletSpeed UMETA(DisplayName = "ź��"),
	ReloadSpeed UMETA(DisplayName = "������ �ӵ�"),
	ProjectileCount UMETA(DisplayName = "����ü ����"),
	BulletSpread UMETA(DisplayName = "ź ����"),
	Recoil UMETA(DisplayName = "�ݵ�"),
	Weight UMETA(DisplayName = "����")
};

/** ������ ���� */
USTRUCT(BlueprintType)
struct FItemStat
{
	GENERATED_BODY()

public:
	TMap<EItemStatName, float> Stats;
};
