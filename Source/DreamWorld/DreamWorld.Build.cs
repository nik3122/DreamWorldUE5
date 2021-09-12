// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

/// <summary>
/// ��ģ�����C#��
/// </summary>
public class DreamWorld : ModuleRules
{
	public DreamWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		// ����Ԥ����ͷ�ļ�
		PrivatePCHHeaderFile = "DreamWorld.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// ��ӹ�������ģ��
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		// ���˽������ģ��
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"UMG",
				"AIModule",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"ProceduralMeshComponent",
				"WHFramework",
				"AchievementSystem"
			}
		);
	}
}
