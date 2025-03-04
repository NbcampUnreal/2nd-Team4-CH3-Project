#pragma once

#include "CoreMinimal.h"
#include "PlayerStats.h"
#include "StatModifier.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CharacterSaveManager.generated.h"

UCLASS()
class PEPCCINE_API UCharacterSaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool LoadPlayerStats(FPlayerStats& OutStats);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SavePlayerStats(const FPlayerStats& InStats);

	bool IsFirstTimeLoaded() const { return bIsFirstTimeLoaded; }

	FORCEINLINE_DEBUGGABLE bool GetIsFirstTimeLoaded() const { return bIsFirstTimeLoaded; }
	FORCEINLINE_DEBUGGABLE void SetIsFirstTimeLoaded(const bool b) { bIsFirstTimeLoaded = b; }
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	bool bIsFirstTimeLoaded = false;
};
