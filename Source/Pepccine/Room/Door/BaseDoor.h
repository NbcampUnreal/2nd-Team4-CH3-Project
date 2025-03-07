// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseDoor.generated.h"

struct FRoomData;
class UBoxComponent;

UENUM()
enum class EDoorDirection : uint8
{
	EUp UMETA(DisplayName = "Up"),
	EDown UMETA(DisplayName = "Down"),
	ELeft UMETA(DisplayName = "Left"),
	ERight UMETA(DisplayName = "Right"),
	ENone UMETA(DisplayName = "None"),
};

UCLASS()
class PEPCCINE_API ABaseDoor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RootScene;
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* DoorStaticMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* TriggerVolume;
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* SpawnPosition;
	UPROPERTY(VisibleInstanceOnly, Category = "Components")
	EDoorDirection Direction;

	bool bIsLocked;

public:
	ABaseDoor();
	FRoomData* GetDirectionRoom();

protected:
	UFUNCTION()
	virtual void OnStarted();
	UFUNCTION()
	virtual void OnCleared();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LockDoor();
	virtual void LockDoor_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OpenDoor();
	virtual void OpenDoor_Implementation();

	virtual void BeginPlay() override;
	virtual void EnterDoor();

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	FORCEINLINE FVector GetSpawnPosition() const { return SpawnPosition->GetComponentLocation(); }
	FORCEINLINE FRotator GetSpawnRotation() const { return GetActorRotation(); }
	FORCEINLINE EDoorDirection GetDirection() const { return Direction; }
	FORCEINLINE void SetDirection(const EDoorDirection DoorDirection) { Direction = DoorDirection; }
};
