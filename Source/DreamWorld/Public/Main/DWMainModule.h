// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MainModule.h"

#include "DWMainModule.generated.h"

/**
 * 游戏模式基类
 */
UCLASS()
class ADWMainModule : public AMainModule
{
	GENERATED_BODY()

public:
	ADWMainModule();

public:
	virtual void InitializeModules_Implementation() override;
};
