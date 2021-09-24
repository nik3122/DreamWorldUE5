// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WidgetInventorySlot.h"

#include "Border.h"
#include "Widget/Inventory/WidgetInventory.h"
#include "DragDropOperation.h"
#include "Image.h"
#include "TextBlock.h"
#include "WidgetBlueprintLibrary.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Kismet/KismetTextLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

UWidgetInventorySlot::UWidgetInventorySlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OwnerSlot = nullptr;
}

void UWidgetInventorySlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(ImgMask)
	{
		MaskMatInst = ImgMask->GetDynamicMaterial();
		MaskMatInst->SetVectorParameterValue(FName("Color"), FLinearColor(0.f, 0.f, 0.f, 0.3f));
		MaskMatInst->SetScalarParameterValue(FName("Progress"), 0.5f);
	}
}

bool UWidgetInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	auto payloadSlot = Cast<UWidgetInventorySlot>(InOperation->Payload);
	if (payloadSlot && payloadSlot != this && !payloadSlot->IsEmpty())
	{
		FItem& tmpItem = payloadSlot->GetItem();
		if(OwnerSlot->CheckSlot(tmpItem))
		{
			if (OwnerSlot->Contains(tmpItem))
			{
				OwnerSlot->AddItem(tmpItem);
				payloadSlot->OwnerSlot->Refresh();
			}
			else
			{
				OwnerSlot->Replace(payloadSlot->OwnerSlot);
			}
		}
	}
	return true;
}

void UWidgetInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	SetBorderColor(FLinearColor(1.0f, 0.843f, 0.0f, 1.0f));
}

void UWidgetInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	SetBorderColor(FLinearColor(0.0f, 1.0f, 1.0f, 0.7f));
}

void UWidgetInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	SetBorderColor(FLinearColor(1.0f, 0.843f, 0.0f, 1.0f));
}

void UWidgetInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SetBorderColor(FLinearColor(0.0f, 1.0f, 1.0f, 0.7f));
}

FReply UWidgetInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FName KeyName = InMouseEvent.GetEffectingButton().GetFName();
	if(KeyName.IsEqual(FName("RightMouseButton")))
	{
		if(InMouseEvent.IsLeftShiftDown())
		{
			MoveItem(-1);
		}
		else
		{
			UseItem(1);
		}
	}
	else if(KeyName.IsEqual(FName("MiddleMouseButton")))
	{
		if(InMouseEvent.IsLeftShiftDown())
		{
			DiscardItem(-1);
		}
		else
		{
			DiscardItem(1);
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UWidgetInventorySlot::InitSlot(UInventorySlot* InOwnerSlot)
{
	OwnerSlot = InOwnerSlot;
	if(OwnerSlot)
	{
		OwnerSlot->SetUISlot(this);
	}
}

void UWidgetInventorySlot::SplitItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->SplitItem(InCount);
	}
}

void UWidgetInventorySlot::MoveItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->MoveItem(InCount);
	}
}

void UWidgetInventorySlot::UseItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->UseItem(InCount);
	}
}

void UWidgetInventorySlot::DiscardItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->DiscardItem(InCount);
	}
}

void UWidgetInventorySlot::Refresh()
{
	if(!IsEmpty())
	{
		ImgIcon->SetVisibility(ESlateVisibility::Visible);
		ImgIcon->SetBrushFromTexture(GetItem().GetData().Icon);
		if(TxtCount)
		{
			if(GetItem().Count > 1)
			{
				TxtCount->SetVisibility(ESlateVisibility::Visible);
				TxtCount->SetText(FText::FromString(FString::FromInt(GetItem().Count)));
			}
			else
			{
				TxtCount->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		ImgIcon->SetVisibility(ESlateVisibility::Hidden);
		if(TxtCount)
		{
			TxtCount->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWidgetInventorySlot::RefreshCooldown()
{
	if(OwnerSlot)
	{
		const FDWCooldownInfo CooldownInfo = OwnerSlot->GetCooldownInfo();
		if(CooldownInfo.bCooldowning)
		{
			ImgMask->SetVisibility(ESlateVisibility::Visible);
			TxtCooldown->SetVisibility(ESlateVisibility::Visible);
			TxtCooldown->SetText(UKismetTextLibrary::Conv_FloatToText(CooldownInfo.RemainTime, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 1));
			MaskMatInst->SetScalarParameterValue(FName("Progress"), 1.f - CooldownInfo.RemainTime / CooldownInfo.TotalTime);
		}
		else
		{
			ImgMask->SetVisibility(ESlateVisibility::Hidden);
			TxtCooldown->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWidgetInventorySlot::SetBorderColor(FLinearColor InColor)
{
	Border->SetBrushColor(InColor);
}

bool UWidgetInventorySlot::IsEmpty() const
{
	if (OwnerSlot) return OwnerSlot->IsEmpty();
	return true;
}

FItem& UWidgetInventorySlot::GetItem() const
{
	if(OwnerSlot) return OwnerSlot->GetItem();
	return FItem::Empty;
}
