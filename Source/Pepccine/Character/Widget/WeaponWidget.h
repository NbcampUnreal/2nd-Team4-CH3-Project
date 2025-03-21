#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PEPCCINE_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmo;
	UPROPERTY(meta = (BindWidget))
	UImage* FrontWeaponImage;
	UPROPERTY(meta = (BindWidget))
	UImage* BackWeaponImage;

	UFUNCTION(BlueprintCallable, Category = "UI|Weapon")
	void UpdateWeaponUI(UTexture2D* MainWeaponImage, UTexture2D* SubWeaponImage, const FString& WeaponName, const int32 Ammo, const int32 MaxAmmo, bool bIsMainWeapon);
};

