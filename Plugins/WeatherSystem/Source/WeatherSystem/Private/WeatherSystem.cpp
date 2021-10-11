// Copyright Epic Games, Inc. All Rights Reserved.
// publisher name:Frey
// 2021 of publication
#include "WeatherSystem.h"

#define LOCTEXT_NAMESPACE "FWeatherSystemModule"

void FWeatherSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FWeatherSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWeatherSystemModule, WeatherSystem)