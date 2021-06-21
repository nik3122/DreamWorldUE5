// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Blueprint/UserWidget.h"
#include "WidgetPanelBase.generated.h"

/**
 * UI面板基类
 */
UCLASS()
class DREAMWORLD_API UWidgetPanelBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidgetPanelBase(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FName WidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EInputMode InputMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EWidgetPanelType WidgetType;

public:
	UFUNCTION(BlueprintCallable)
	void SetActive(bool bActive);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShowPanel();
		
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HidePanel();

	UFUNCTION(BlueprintCallable)
	void TogglePanel();
	
	UFUNCTION(BlueprintCallable)
	virtual void RefreshPanel();
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "RefreshPanel"))
	void K2_RefreshPanel();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActive();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual AActor* GetOwnerActor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetWidgetName() const { return WidgetName; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EInputMode GetInputMode() const { return InputMode; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EWidgetPanelType GetWidgetType() const { return WidgetType; }
};
