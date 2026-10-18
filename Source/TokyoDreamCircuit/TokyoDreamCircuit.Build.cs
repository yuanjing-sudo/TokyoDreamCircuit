// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TokyoDreamCircuit : ModuleRules
{
	public TokyoDreamCircuit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Kept deliberately lean: only what the code in this module actually
		// references. Every extra dependency slows the first compile a lot.
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"PhysicsCore",
			"ProceduralMeshComponent"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RenderCore",
			"RHI"
		});

		// Uncomment when you start using these. They are enabled as plugins in
		// TokyoDreamCircuit.uproject already.
		// PrivateDependencyModuleNames.AddRange(new string[] { "ChaosVehicles", "Chaos", "Niagara", "PCG" });
	}
}
