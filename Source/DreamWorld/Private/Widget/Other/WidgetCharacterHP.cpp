// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Other/WidgetCharacterHP.h"

UWidgetCharacterHP::UWidgetCharacterHP(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OwnerCharacter = nullptr;
	Visibility = ESlateVisibility::Hidden;
}
