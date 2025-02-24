#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDetected, AActor*, DetectedActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEPCCINE_API URadorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URadorComponent();

  // Debug ��
  UPROPERTY(EditAnywhere, Category = "Debug")
  bool bShowDetectionRadius = false;

  UPROPERTY(EditAnywhere, Category = "Debug")
  bool bShowFieldOfView = false; // FOV

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
  UPROPERTY(EditAnywhere, Category = "Radar")
  float DetectionRadius = 1200.0f; // ���� �ݰ�

  UPROPERTY(EditAnywhere, Category = "Radar")
  float FieldOfView = 120.0f; // �þ߰� (FOV)

  UPROPERTY(EditAnywhere, Category = "Radar")
  TSubclassOf<AActor> DetectionClass; // ������ ���� Ÿ�� (��: ACharacter)

  UPROPERTY(BlueprintAssignable, Category = "Radar")
  FOnActorDetected OnActorDetected; // �������Ʈ �̺�Ʈ

private:
  bool IsInFieldOfView(AActor* TargetActor) const;
  void DetectActors();

  void DrawDebugLine();
};
