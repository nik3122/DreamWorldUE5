// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

/// <summary>
/// 主模块规则C#类
/// </summary>
public class DreamWorld : ModuleRules
{
	public DreamWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		// 设置预编译头文件
		PrivatePCHHeaderFile = "DreamWorld.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 添加公有依赖模块
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		// 添加私有依赖模块
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"UMG",
				"AIModule",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"ProceduralMeshComponent"
			}
		);
	}
}
