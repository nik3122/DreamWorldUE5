// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/GameModeBase.h"
#include "DWGameMode.generated.h"

class UWidgetPanelBase;
/**
 * 游戏模式基类
 */
UCLASS()
class ADWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADWGameMode();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EInputMode InputMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetPanelBase* TemporaryPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UWidgetPanelBase*> PermanentPanels;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName, UWidgetPanelBase*> WidgetPanels;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void PauseGame();
	
	UFUNCTION(BlueprintCallable)
	void ContinueGame();
	
	UFUNCTION(BlueprintCallable)
	void BackMainMenu();
		
	UFUNCTION(BlueprintCallable)
	void ExitGame();
				
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GenerateWidgets();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsExistWidgetPanel(FName InWidgetName);
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWidgetPanelBase* GetWidgetPanel(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	UWidgetPanelBase* CreateWidgetPanel(TSubclassOf<UWidgetPanelBase> InWidgetClass, bool bShowPanel = false);

	UFUNCTION(BlueprintCallable)
	bool RemoveWidgetPanel(UWidgetPanelBase* InWidgetPanel);

	bool RemoveWidgetPanel(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	bool ToggleWidgetPanel(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	bool ShowWidgetPanel(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	bool HideWidgetPanel(FName InWidgetName);
		
	UFUNCTION(BlueprintCallable)
	void ActiveTemporaryPanel(bool bActive);
			
	UFUNCTION(BlueprintCallable)
	void ActivePermanentPanels(bool bActive);
			
	UFUNCTION(BlueprintCallable)
	void ActiveWidgetPanels(bool bActive);

	UFUNCTION(BlueprintCallable)
	void UpdateInputMode();

	UFUNCTION(BlueprintCallable)
	void SetInputMode(EInputMode InInputMode);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EInputMode GetInputMode() const { return InputMode; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWidgetPanelBase* GetTemporaryPanel() const { return TemporaryPanel; }

	UFUNCTION(BlueprintCallable)
	void SetTemporaryPanel(UWidgetPanelBase* InTemporaryPanel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UWidgetPanelBase*> GetPermanentPanels() const { return PermanentPanels; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FName, UWidgetPanelBase*> GetWidgetPanels() const { return WidgetPanels; }
};
