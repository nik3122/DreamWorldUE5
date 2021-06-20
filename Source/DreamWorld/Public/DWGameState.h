// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/GameStateBase.h"
#include "DWGameState.generated.h"

/**
 * 游戏状态基类
 */
UCLASS()
class ADWGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADWGameState();

protected:
	// 当前状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGameState CurrentState;

protected:
	virtual void BeginPlay() override;

public:
	/** 设置当前状态 */
	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EGameState InGameState);

	/** 获取当前状态 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EGameState GetCurrentState() const { return CurrentState; }
};
