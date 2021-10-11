// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "WidgetComponent.h"
#include "WidgetVitalityHPComponent.generated.h"

class AVitalityObject;

/**
 * 生命体血条组件
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class DREAMWORLD_API UWidgetVitalityHPComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UWidgetVitalityHPComponent();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	AVitalityObject* OwnerVitality;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void RefreshVisibility() const;
};
