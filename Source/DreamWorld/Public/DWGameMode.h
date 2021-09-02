// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WidgetPanelBase.h"
#include "GameFramework/GameModeBase.h"
#include "DWGameMode.generated.h"

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
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void PauseGame();
	
	UFUNCTION(BlueprintCallable)
	void UnPauseGame();
	
	UFUNCTION(BlueprintCallable)
	void BackMainMenu();
		
	UFUNCTION(BlueprintCallable)
	void QuitGame();
	
	//////////////////////////////////////////////////////////////////////////
	// InputMode
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EInputMode InputMode;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateInputMode();

	UFUNCTION(BlueprintCallable)
	void SetInputMode(EInputMode InInputMode);

	UFUNCTION(BlueprintPure)
	EInputMode GetInputMode() const { return InputMode; }

	//////////////////////////////////////////////////////////////////////////
	// WidgetPanels
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetPanelBase* TemporaryPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UWidgetPanelBase*> PermanentPanels;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName, class UWidgetPanelBase*> WidgetPanels;
			
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GenerateWidgets();

	UFUNCTION(BlueprintPure)
	bool IsExistWidgetPanel(FName InWidgetName) const;
		
	UFUNCTION(BlueprintCallable)
	UWidgetPanelBase* CreateWidgetPanel(TSubclassOf<UWidgetPanelBase> InWidgetClass, bool bShowPanel = false);

	UFUNCTION(BlueprintPure)
	UWidgetPanelBase* GetWidgetPanelByName(FName InWidgetName);

	template<class T>
	T* GetWidgetPanelByClass()
	{
		FName TmpName;
		if(UWidgetPanelBase* TmpObject = Cast<UWidgetPanelBase>(T::StaticClass()->GetDefaultObject()))
		{
			TmpName = TmpObject->GetWidgetName();
		}
		return Cast<T>(GetWidgetPanelByName(TmpName));
	}

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetWidgetPanelByClass", DeterminesOutputType = "InWidgetPanelClass"))
	UWidgetPanelBase* K2_GetWidgetPanelByClass(TSubclassOf<UWidgetPanelBase> InWidgetPanelClass);

	UFUNCTION(BlueprintCallable)
	bool RemoveWidgetPanelByName(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	bool ToggleWidgetPanelByName(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	bool ShowWidgetPanelByName(FName InWidgetName);

	UFUNCTION(BlueprintCallable)
	bool HideWidgetPanelByName(FName InWidgetName);
		
	UFUNCTION(BlueprintCallable)
	void ActiveTemporaryPanel(bool bActive);
			
	UFUNCTION(BlueprintCallable)
	void ActivePermanentPanels(bool bActive);
			
	UFUNCTION(BlueprintCallable)
	void ActiveWidgetPanels(bool bActive);

	UFUNCTION(BlueprintPure)
	UWidgetPanelBase* GetTemporaryPanel() const { return TemporaryPanel; }

	UFUNCTION(BlueprintCallable)
	void SetTemporaryPanel(UWidgetPanelBase* InTemporaryPanel);

	UFUNCTION(BlueprintPure)
	TArray<UWidgetPanelBase*> GetPermanentPanels() const { return PermanentPanels; }

	UFUNCTION(BlueprintPure)
	TArray<UWidgetPanelBase*> GetWidgetPanels() const;
};
