// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "WidgetComponent.h"
#include "WidgetWorldTextComponent.generated.h"

/**
 * 世界文本组件
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class DREAMWORLD_API UWidgetWorldTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UWidgetWorldTextComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void RefreshVisibility();
};
