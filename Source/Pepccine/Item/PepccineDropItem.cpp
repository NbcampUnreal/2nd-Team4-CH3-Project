﻿#include "PepccineDropItem.h"

#include "PepccineItemDataBase.h"
#include "PepccineItemManagerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Passive/PepccinePassiveItemData.h"
#include "Resource/PepccineResourceItemData.h"
#include "Weapon/PepccineWeaponItemData.h"

APepccineDropItem::APepccineDropItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// 오버랩 컴포넌트 초기화
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->InitSphereRadius(50.f);
	// init전 콜리전 비활성화
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 스태틱 메시 컴포넌트 초기화
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(GetRootComponent());
	StaticMeshComp->SetSimulatePhysics(false);

	// 상호작용 위젯 생성
	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(GetRootComponent());
}

void APepccineDropItem::BeginPlay()
{
	Super::BeginPlay();

	StartZLocation = StaticMeshComp->GetRelativeLocation().Z;
}

void APepccineDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 스태틱 메쉬 상하 움직임
	CurrentTime += DeltaTime;
	CurrentTime = FMath::Fmod(CurrentTime, 360.0f);

	const float NewZLocation = StartZLocation + 20.0f * FMath::Sin(CurrentTime * 0.5f * 2.0f * PI);

	FVector NewLocation = StaticMeshComp->GetRelativeLocation();
	NewLocation.Z = NewZLocation;
	StaticMeshComp->SetRelativeLocation(NewLocation);
	
	// 스태틱 메쉬 회전
	StaticMeshComp->AddLocalRotation(FRotator(0, DeltaTime * 45.0f, 0));
}

void APepccineDropItem::InitializeDropItem(const UPepccineItemDataBase* InDropItemData)
{
	// 데이터 복사 저장
	DropItemData = DuplicateObject<UPepccineItemDataBase>(InDropItemData, this);

	if (DropItemData)
	{
		// 스폰 메시 설정
		StaticMeshComp->SetStaticMesh(DropItemData->GetMeshToSpawn());

		// 패시브일 경우 메시 추가 설정
		if (DropItemData->IsA(UPepccinePassiveItemData::StaticClass()))
		{
			StaticMeshComp->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));

			UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(0), this);
			DynMaterial->SetTextureParameterValue(FName("ItemIcon"), DropItemData->IconTexture);

			StaticMeshComp->SetMaterial(0, DynMaterial);
		}

		// 콜리전 활성화
		SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void APepccineDropItem::ShowInteractWidget(const bool bShow) const
{
	InteractWidgetComp->SetHiddenInGame(!bShow);
}

void APepccineDropItem::PickUpItem(UPepccineItemManagerComponent* ItemManagerComponent)
{
	if (ItemManagerComponent)
	{
		bool IsDestroy = true;

		if (!DropItemData)
		{
			UE_LOG(LogTemp,Warning, TEXT("아이템 데이터가 없습니다!"));
			return;
		}
	
		UPepccineWeaponItemData* BeforeWeapon = nullptr;

		// 무기 아이템
		if (const UPepccineWeaponItemData* DropWeapon = Cast<UPepccineWeaponItemData>(DropItemData))
		{
			// 이전 무기 가져오기
			BeforeWeapon = ItemManagerComponent->GetWeaponItemData(DropWeapon->GetWeaponItemType());
		}
		// 자원 아이템
		else if (UPepccineResourceItemData* ResourceItemData = Cast<UPepccineResourceItemData>(DropItemData))
		{
			int32 RemainingCount = 0;
			
			if (ResourceItemData->GetResourceItemType() == EPepccineResourceItemType::EPRIT_AmmoBox)
			{
				const UPepccineWeaponItemData* MainWeaponData = ItemManagerComponent->GetWeaponItemData(EPepccineWeaponItemType::EPWIT_Main);
				if (!MainWeaponData)
				{
					UE_LOG(LogTemp, Warning, TEXT("주 무기가 없습니다."));
					return;
				}
				RemainingCount = (MainWeaponData->GetWeaponItemStats().SpareAmmo + ResourceItemData->GetResourceCount()) - 999;
			}
			// else if (ResourceItemData->GetResourceItemType() == EPepccineResourceItemType::EPRIT_CardKey)
			// {
			// 	RemainingCount = 99 - (ItemManagerComponent->GetCardKeyCount() + ResourceItemData->GetResourceCount());
			// }
			else if (ResourceItemData->GetResourceItemType() == EPepccineResourceItemType::EPRIT_Coin)
			{
				RemainingCount = (ItemManagerComponent->GetCoinCount() + ResourceItemData->GetResourceCount()) - 99;
			}

			if (RemainingCount > 0)
			{
				IsDestroy = false;
				ResourceItemData->SetResourceCount(ResourceItemData->GetResourceCount() - RemainingCount);
			}
		}

		// 아이템 획득
		if (!ItemManagerComponent->PickUpItem(DropItemData))
		{
			return;
		}

		// 이전 무기가 있을 경우
		if (BeforeWeapon)
		{
			IsDestroy = false;
			DropItemData = BeforeWeapon;
			StaticMeshComp->SetStaticMesh(BeforeWeapon->GetMeshToSpawn());
		}

		if (IsDestroy)
		{
			// 획득 후 엑터 제거
			Destroy();
		}
	}
}
