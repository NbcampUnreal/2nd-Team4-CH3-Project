﻿#pragma once

#include "CoreMinimal.h"
#include "Item/ItemSpawn/PepccineItemSpawnerSubSystem.h"
#include "Item/PepccineStatName.h"
#include "Item/Active/PepccineActiveItemData.h"
#include "Item/Active/PepccineActiveItemManager.h"
#include "Components/ActorComponent.h"
#include "Item/Weapon/PepccineWeaponItemData.h"
#include "Item/Weapon/PepccineWeaponItemComponent.h"
#include "Item/Passive/PepccinePassiveItemManager.h"
#include "Item/Weapon/PepccineWeaponItemManager.h"

#include "PepccineItemManagerComponent.generated.h"

class UPepccineActiveItemManager;
class UPepccinePassiveItemManager;
class UPepccineActiveItemData;
class UPepccinePassiveItemData;
class UPepccineItemDataAssetBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PEPCCINE_API UPepccineItemManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPepccineItemManagerComponent();

	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 데이터 로드
	bool LoadItemSaveData();
	// 데이터 세이브
	void SaveItemSaveData() const;

	// 아이템 획득
	bool PickUpItem(const UPepccineItemDataBase* DropItemData, bool bIsPlayPickUpSound = true, bool bIsShopItem = false,
	                int32 Price = 0, float MagazineAmmo = -1, float SpareAmmo = -1);
	// 무기 교체
	UFUNCTION(BlueprintCallable, Category = "Item|Weapon")
	void SwapWeapon(EPepccineWeaponItemType WeaponType) const;

	// 현재 장착 중인 무기 발사
	UFUNCTION(BlueprintCallable, Category = "Item|Weapon")
	void FireWeapon(float WeaponDamage) const;

	// 현재 장착 중인 무기 재장전
	UFUNCTION(BlueprintCallable, Category = "Item|Weapon")
	void ReloadWeapon() const;

	// 무기 스탯 합연산 및 곱연산 증가
	void IncreaseStatsOperations(TArray<FPepccineWeaponStatModifier> Modifiers);
	// 캐릭터 스탯 합연산 및 곱연산 증가
	void IncreaseStatsOperations(TArray<FPepccineCharacterStatModifier> Modifiers);

	// 무기 스탯 합연산 및 곱연산 감소
	void DecreaseStatsOperations(TArray<FPepccineWeaponStatModifier> Modifiers);
	// 캐릭터 스탯 합연산 및 곱연산 감소
	void DecreaseStatsOperations(TArray<FPepccineCharacterStatModifier> Modifiers);

	// 액티브 아이템 사용
	UFUNCTION(BlueprintCallable, Category = "Item|ActiveItem")
	void UseActiveItem() const;

	// 버프 제거
	void RemoveBuffEffect(const UPepccinePotionItemData* PotionItemData) const;

	// 코인 사용
	UFUNCTION(BlueprintCallable, Category = "Item|Resource")
	bool UseCoin(int32 Price);

	// getter
	// 발사 방향 가져오기
	FVector GetShootDirection() const;

	// 무기 컴포넌트 가져오기
	UFUNCTION(BlueprintPure, Category = "Item|Weapon")
	FORCEINLINE UPepccineWeaponItemComponent* GetWeaponItemComp() const
	{
		return WeaponItemManager->GetWeaponItemComp();
	};
	// 무기 데이터 가져오기
	UFUNCTION(BlueprintPure, Category = "Item|Weapon")
	FORCEINLINE UPepccineWeaponItemData* GetWeaponItemData(
		const EPepccineWeaponItemType WeaponItemType) const
	{
		return WeaponItemManager->GetWeaponItemData(WeaponItemType);
	}

	// 장착 중인 무기 데이터 가져오기
	UFUNCTION(BlueprintPure, Category = "Item|Weapon")
	FORCEINLINE UPepccineWeaponItemData* GetEquippedWeaponItemData() const
	{
		return WeaponItemManager->GetEquippedWeaponItemData();
	}

	// 현재 장착중인 무기가 메인 무기인지 확인
	FORCEINLINE bool IsMainWeaponEquipped() const
	{
		return GetEquippedWeaponItemData()->GetWeaponItemType() == EPepccineWeaponItemType::EPWIT_Main;
	}

	// 전체 패시브 데이터 가져오기
	UFUNCTION(BlueprintPure, Category = "Item|Passive")
	FORCEINLINE TMap<int32, UPepccinePassiveItemData*> GetPassiveItemDatas() const
	{
		return PassiveItemManager->GetPassiveItemDatas();
	}

	// 아이템 아이디로 패시브 데이터 가져오기
	UFUNCTION(BlueprintPure, Category = "Item|Passive")
	FORCEINLINE UPepccinePassiveItemData* GetPassiveItemById(const int32 ItemId) const
	{
		return PassiveItemManager->GetPassiveItemById(ItemId);
	}

	// 스탯 이름으로 무기 스탯 합연산 총합 가져오기
	FORCEINLINE float GetTotalSumByWeaponItemStatName(const EPepccineWeaponStatName WeaponItemStatName)
	{
		if (!GetEquippedWeaponItemData()) return 0;
		return TotalWeaponStatSum.FindOrAdd({GetEquippedWeaponItemData()->GetWeaponItemType(), WeaponItemStatName});
	}

	// 스탯 이름으로 무기 스탯 곱연산 총합 가져오기
	FORCEINLINE float GetTotalProductByWeaponItemStatName(const EPepccineWeaponStatName WeaponItemStatName)
	{
		if (!GetEquippedWeaponItemData()) return 0;
		return TotalWeaponStatProduct.FindOrAdd({GetEquippedWeaponItemData()->GetWeaponItemType(), WeaponItemStatName},
		                                        1.0f);
	}

	// 스탯 이름으로 캐릭터 스탯 합연산 총합 가져오기
	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE float GetTotalSumByCharacterStatName(const EPepccineCharacterStatName CharacterStatName)
	{
		return TotalCharacterStatSum.FindOrAdd(CharacterStatName);
	}

	// 스탯 이름으로 캐릭터 스탯 곱연산 총합 가져오기
	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE float GetTotalProductByCharacterStatName(const EPepccineCharacterStatName CharacterStatName)
	{
		return TotalCharacterStatProduct.FindOrAdd(CharacterStatName, 1.0f);
	}

	// 액티브 아이템 가져오기
	UFUNCTION(BlueprintPure, Category = "Item|Active")
	FORCEINLINE UPepccineActiveItemData* GetActiveItemData() const { return ActiveItemManager->GetActiveItemData(); }

	// 버프 적용 여부 확인
	UFUNCTION(BlueprintPure, Category = "Item|Active")
	FORCEINLINE bool IsAppliedBuff() const
	{
		return ActiveItemManager->IsAppliedBuff();
	}

	// 현재 남은 재사용 대기시간 가져오기
	UFUNCTION(BlueprintCallable, Category = "Item|Active")
	FORCEINLINE float GetActiveItemRemainingCooldown() const
	{
		return ActiveItemManager->GetActiveItemRemainingCooldown();
	}

	// 현재 재사용 대기 중 인지 확인
	UFUNCTION(BlueprintCallable, Category = "Item|Active")
	FORCEINLINE bool IsActiveItemCooldown() const { return ActiveItemManager->IsActiveItemCooldown(); }

	UFUNCTION(BlueprintCallable, Category = "Item|Resource")
	FORCEINLINE int32 GetCoinCount() const { return CoinCount; }

private:
	// 무기 아이템 매니저
	UPROPERTY(VisibleInstanceOnly, Category = "Item|Manager")
	UPepccineWeaponItemManager* WeaponItemManager;

	// 패시브 아이템 매니저
	UPROPERTY(VisibleInstanceOnly, Category = "Item|Manager")
	UPepccinePassiveItemManager* PassiveItemManager;

	// 무기 스탯 합연산 총합
	TMap<TPair<EPepccineWeaponItemType, EPepccineWeaponStatName>, float> TotalWeaponStatSum;
	// 무기 스탯 곱연산 총합
	TMap<TPair<EPepccineWeaponItemType, EPepccineWeaponStatName>, float> TotalWeaponStatProduct;

	// 캐릭터 스탯 합연산 총합
	TMap<EPepccineCharacterStatName, float> TotalCharacterStatSum;
	// 캐릭터 스탯 곱연산 총합
	TMap<EPepccineCharacterStatName, float> TotalCharacterStatProduct;

	// 액티브 아이템 매니저
	UPROPERTY(VisibleInstanceOnly, Category = "Item|Manager")
	UPepccineActiveItemManager* ActiveItemManager;
	
	// 코인
	UPROPERTY(EditDefaultsOnly, Category = "Item|Resource", meta = (DisplayName = "코인 수"))
	int32 CoinCount = 100;
};
