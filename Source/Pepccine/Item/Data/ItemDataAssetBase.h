#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAssetBase.generated.h"

USTRUCT(BlueprintType)
struct FItemDataInfo 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info");
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (MultiLine = "true"));
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info");
	int32 SellingPrice;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	TObjectPtr<UStaticMesh> SpawnMesh;
};

USTRUCT(BlueprintType)
struct FWeaponItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon");
	FItemDataInfo Info;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMesh> EquippedMesh;

	// TODO[���] : ���� ���� �߰�
};

USTRUCT(BlueprintType)
struct FPassiveItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passive");
	FItemDataInfo Info;

	// TODO[���] : ��� �߰�
};


UCLASS()
class PEPCCINE_API UItemDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWeaponItemData> Weapons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPassiveItemData> Passives;

	// TODO[���] : UseableItemData �߰�

};
