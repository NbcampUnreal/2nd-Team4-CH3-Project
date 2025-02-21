// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/PepccineMontagesComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UPepccineMontagesComponent::UPepccineMontagesComponent()
{
}

void UPepccineMontagesComponent::BeginPlay()
{
    Owner = Cast<ACharacter>(GetOwner());
    if (Owner)
    {
        TObjectPtr<class USkeletalMeshComponent> Mesh = Cast<USkeletalMeshComponent>(Owner->GetMesh());
        if (Mesh)
        {
            AnimInstance = Mesh->GetAnimInstance();
        }
    }
}

void UPepccineMontagesComponent::PlayFireMontage()
{
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
    }
}

void UPepccineMontagesComponent::PlayReloadMontage()
{
    if (AnimInstance && ReloadMontage)
    {
        AnimInstance->Montage_Play(ReloadMontage);
    }
}
