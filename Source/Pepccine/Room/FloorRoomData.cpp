﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorRoomData.h"

#include "RoomManager.h"

UFloorRoomData::UFloorRoomData()
{
	MapSize = 6;
	EndPointCount = 4;
}

TSoftObjectPtr<UWorld> UFloorRoomData::GetRandomRoomLevel(const ERoomType RoomType)
{
	switch (RoomType)
	{
		case ERoomType::EStart: return StartRoomLevels[FMath::RandRange(0, StartRoomLevels.Num() - 1)];
		case ERoomType::EBoss: return BossRoomLevels[FMath::RandRange(0, BossRoomLevels.Num() - 1)];
		case ERoomType::EItem: return ItemRoomLevels[FMath::RandRange(0, ItemRoomLevels.Num() - 1)];
		case ERoomType::EShop: return ShopRoomLevels[FMath::RandRange(0, ShopRoomLevels.Num() - 1)];
		case ERoomType::EDefault:
		case ERoomType::ENone: return nullptr;
	}
	return nullptr;
}