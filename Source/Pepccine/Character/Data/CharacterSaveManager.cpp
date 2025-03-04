#include "CharacterSaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStatSaveGame.h"
#include "Character/Data/StatModifier.h"

void UCharacterSaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Warning, TEXT("Character Save Manager Initialized"));
	bIsFirstTimeLoaded = true;

	SavePlayerStats(FPlayerStats());
}

bool UCharacterSaveManager::LoadPlayerStats(FPlayerStats& OutStats)
{
	FString SaveSlotName = TEXT("PlayerStatsSlot");

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		UPlayerStatSaveGame* LoadGameInstance = Cast<UPlayerStatSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

		if (LoadGameInstance)
		{
			OutStats = LoadGameInstance->SavedStats;

			UE_LOG(LogTemp, Log, TEXT("플레이어 스탯이 성공적으로 로드되었습니다!"));
			return true;
		}

		UE_LOG(LogTemp, Warning, TEXT("세이브 데이터 로드 실패!"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("세이브 데이터 없음. 기본 값 사용!"));
	return false;
}

void UCharacterSaveManager::SavePlayerStats(const FPlayerStats& InStats)
{
	FString SaveSlotName = TEXT("PlayerStatsSlot");

	UPlayerStatSaveGame* SaveGameInstance = Cast<UPlayerStatSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UPlayerStatSaveGame::StaticClass()));

	if (!SaveGameInstance) return;

	SaveGameInstance->SavedStats = InStats;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
	UE_LOG(LogTemp, Log, TEXT("플레이어 스탯이 저장되었습니다."));
}
