// Copyright Epic Games, Inc. All Rights Reserved.
// publisher name:Frey
// 2021 of publication
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FWeatherSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
