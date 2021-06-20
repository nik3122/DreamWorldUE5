#pragma once

#include "DreamWorld.h"
#include "Engine/AssetManager.h"
#include "DWAssetManager.generated.h"

/**
 * 资源管理器
 */
UCLASS()
class DREAMWORLD_API UDWAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UDWAssetManager() {}

public:
	/** 获取当前资源管理器实例 */
	static UDWAssetManager& Get();

public:
	virtual void StartInitialLoading() override;
};