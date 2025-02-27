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
	ELeft UMETA(DisplayName = "Left"),
	ERight UMETA(DisplayName = "Right"),
	EUp UMETA(DisplayName = "Up"),
	EDown UMETA(DisplayName = "Down")
};

UCLASS()
class PEPCCINE_API ABaseDoor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* DoorStaticMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerVolume;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SpawnPosition;
	UPROPERTY(EditAnywhere, Category = "Components")
	EDoorDirection Direction;

	bool bIsLocked;

public:
	ABaseDoor();
	FRoomData* GetDirectionRoom();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnStarted();
	UFUNCTION()
	void OnCleared();

	void LockDoor();
	void OpenDoor();

public:
	FORCEINLINE FVector GetSpawnPosition() const { return SpawnPosition->GetComponentLocation(); }
	FORCEINLINE FRotator GetSpawnRotation() const{ return GetActorRotation(); }
};
