// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/CharacterInventory.h"

#include "WidgetInventoryBar.h"
#include "WidgetModuleBPLibrary.h"
#include "Character/DWCharacter.h"

UCharacterInventory::UCharacterInventory()
{
	
}

void UCharacterInventory::Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos)
{
	Super::Initialize(InOwner, InSplitInfos);
}

void UCharacterInventory::LoadData(FInventoryData InInventoryData, AActor* InOwner)
{
	Super::LoadData(InInventoryData, InOwner);
}

FInventoryData UCharacterInventory::ToData(bool bSaved)
{
	return Super::ToData();
}

void UCharacterInventory::DiscardAll()
{
	Super::DiscardAll();
}

void UCharacterInventory::ClearAll()
{
	Super::ClearAll();
}

TArray<UInventorySlot*> UCharacterInventory::GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int32 InStartIndex, int32 InEndIndex)
{
	if (InStartIndex == -1 && HasSplitSlotInfo(ESplitSlotType::Shortcut) && GetOwnerCharacter() && GetOwnerCharacter()->GetNature() == ECharacterNature::Player)
	{
		InStartIndex = GetSplitSlotInfo(ESplitSlotType::Shortcut).StartIndex + UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->GetSelectedSlotIndex();
	}
	return Super::GetValidatedList(InActionType, InItem, InStartIndex, InEndIndex);
}

ADWCharacter* UCharacterInventory::GetOwnerCharacter() const
{
	return Cast<ADWCharacter>(OwnerActor);
}
