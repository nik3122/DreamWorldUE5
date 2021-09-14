/*
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "DWGameMode.generated.h"

/**
 * 游戏模式基类
 #1#
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
};
*/
