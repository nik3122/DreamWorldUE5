// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "Gameplay/WHGameState.h"
#include "DWGameState.generated.h"

/**
 * ��Ϸ״̬����
 */
UCLASS()
class DREAMWORLD_API ADWGameState : public AWHGameState
{
	GENERATED_BODY()

public:
	ADWGameState();

protected:
	// ��ǰ״̬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGameState CurrentState;

protected:
	virtual void BeginPlay() override;

public:
	/** ���õ�ǰ״̬ */
	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EGameState InGameState);

	/** ��ȡ��ǰ״̬ */
	UFUNCTION(BlueprintPure)
	EGameState GetCurrentState() const { return CurrentState; }
};
