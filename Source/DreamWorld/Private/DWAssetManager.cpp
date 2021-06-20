#include "DWAssetManager.h"
#include "AbilitySystemGlobals.h"

UDWAssetManager& UDWAssetManager::Get()
{
	// 获取引擎默认的资源管理器
	UDWAssetManager* Manager = Cast<UDWAssetManager>(GEngine->AssetManager);
	if (Manager)
	{
		return *Manager;
	}
	else
	{
		UDWHelper::Debug("请将DefaultEngine.ini里的AssetManager设置为DWAssetManager！");
		return *NewObject<UDWAssetManager>();
	}
}

void UDWAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
