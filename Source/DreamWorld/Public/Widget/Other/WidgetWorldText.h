// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Blueprint/UserWidget.h"
#include "WidgetWorldText.generated.h"

/**
 * 世界文本
 */
UCLASS()
class DREAMWORLD_API UWidgetWorldText : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	EWorldTextType WidgetWorldTextType;

public:
	UWidgetWorldText(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitWidgetWorldText(EWorldTextType InWidgetWorldTextType, const FString& InTextContent);
};
