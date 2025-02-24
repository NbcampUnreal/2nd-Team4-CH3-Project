#include "Character/Player/RadorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "PepCharacter.h"
#include "DrawDebugHelpers.h" // ����׿� ���

URadorComponent::URadorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URadorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void URadorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (AActor* Owner = GetOwner())
  {
    DetectActors();
    DrawDebugLine();
  }
}

void URadorComponent::DrawDebugLine()
{
  if (!bShowDetectionRadius && !bShowFieldOfView) return;

  FVector Location = GetOwner()->GetActorLocation();
  FVector Forward = GetOwner()->GetActorForwardVector();

  // �����ݰ�
  if (bShowDetectionRadius)
  {
    DrawDebugSphere(GetWorld(), Location, DetectionRadius, 32, FColor::Green, false, 0.1f, 0, 2.0f);
  }

  // �þ߰�
  if (bShowFieldOfView)
  {
    float HalfFOV = FMath::DegreesToRadians(FieldOfView * 0.5f);
    DrawDebugCone(GetWorld(), Location, Forward, DetectionRadius, HalfFOV, HalfFOV, 30, FColor::Red, false, 0.1f, 0, 2.0f);
  }
}

void URadorComponent::DetectActors()
{
  TArray<AActor*> DetectedActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), DetectionClass, DetectedActors);

  // Tag �������̳� ����

  for (AActor* Actor : DetectedActors)
  {
    if (!Actor || Actor == GetOwner()) continue;

    // ���� �ݰ� ���� �ִ��� Ȯ��
    float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
    if (Distance > DetectionRadius) continue;

    // �þ߰� ���� �ִ��� Ȯ��
    if (IsInFieldOfView(Actor))
    {
      OnActorDetected.Broadcast(Actor); // ���� �̺�Ʈ ȣ��
    }
  }
}

// ����(Dot Product): �� ���Ͱ� Ư�� ���� �ȿ� �ִ��� �Ǵ��� �� ��� (FOV üũ)
// ����(Cross Product) : ���� ���� ���� ���踦 Ȯ���� �� ���
bool URadorComponent::IsInFieldOfView(AActor* TargetActor) const
{
  if (!TargetActor) return false;

  FVector MyLocation = GetOwner()->GetActorLocation();
  FVector TargetLocation = TargetActor->GetActorLocation();

  // �� ���� �ٶ󺸴� ����
  FVector ForwardVector = GetOwner()->GetActorForwardVector().GetSafeNormal();

  // ��ǥ ���� ����
  FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();

  // ����(dot product) ���
  float DotProduct = FVector::DotProduct(ForwardVector, ToTarget);

  // �þ߰� �ڻ��� ��
  float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(FieldOfView * 0.5f));

  return DotProduct >= CosHalfFOV;
}