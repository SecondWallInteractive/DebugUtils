// Copyright (c) 2025–present Second Wall Interactive. All Rights Reserved.

using UnrealBuildTool;

public class SwiDebugOutput : ModuleRules
{
	public SwiDebugOutput(ReadOnlyTargetRules Target) : base(Target)
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
