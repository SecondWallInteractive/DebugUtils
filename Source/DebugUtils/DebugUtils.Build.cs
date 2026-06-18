// Copyright (c) 2025–present Second Wall Interactive. All Rights Reserved.

using UnrealBuildTool;

public class DebugUtils : ModuleRules
{
	public DebugUtils(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine", // DrawDebugHelpers, FVisualLogger, UWorld, UPrimitiveComponent, FCollisionShape
		});
	}
}
