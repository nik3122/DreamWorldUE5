// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DreamWorldEditorTarget : TargetRules
{
	public DreamWorldEditorTarget(TargetInfo InTarget) : base(InTarget)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("DreamWorld");
	}
}
