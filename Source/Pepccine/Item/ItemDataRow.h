#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase ���ǰ� ����ִ� ���
#include "ItemDataRow.generated.h"

USTRUCT(BlueprintType)
struct FItemDataRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackMultiplier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RangeMultiplier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireRateMultiplier;
};
