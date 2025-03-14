﻿#pragma once

#include "CoreMinimal.h"
#include "Item/PepccineItemDataBase.h"
#include "PepccineResourceItemData.generated.h"

UENUM(BlueprintType)
enum class EPepccineResourceItemType : uint8
{
	EPRIT_AmmoBox UMETA(DisplayName = "탄약통"),
	EPRIT_Coin UMETA(DisplayName = "코인"),
	EPRIT_HealingPotion UMETA(DisplayName = "체력 포션")
};

UCLASS()
class PEPCCINE_API UPepccineResourceItemData : public UPepccineItemDataBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Item|Resource")
	FORCEINLINE EPepccineResourceItemType GetResourceItemType() const { return ResourceItemType; }
	
	UFUNCTION(BlueprintPure, Category = "Item|Resource")
	FORCEINLINE int32 GetResourceAmount() const { return ResourceAmount; }
	
	UFUNCTION(BlueprintCallable, Category = "Item|Resource")
	FORCEINLINE void SetResourceAmount(const int32 InResourceCount) { ResourceAmount = InResourceCount; }

protected:
	// 자원 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info|Resource", meta = (DisplayName = "자원 타입"))
	EPepccineResourceItemType ResourceItemType;

	// 자원 양
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info|Resource", meta = (DisplayName = "자원 양"))
	int32 ResourceAmount = 1;
};
