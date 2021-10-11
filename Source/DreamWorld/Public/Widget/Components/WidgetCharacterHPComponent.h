// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "WidgetComponent.h"
#include "WidgetCharacterHPComponent.generated.h"

class ADWCharacter;

/**
 * 角色血条组件
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class DREAMWORLD_API UWidgetCharacterHPComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UWidgetCharacterHPComponent();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void RefreshVisibility() const;
};
