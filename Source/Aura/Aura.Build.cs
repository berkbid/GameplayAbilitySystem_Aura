// Copyright Berkeley Bidwell

using UnrealBuildTool;

public class Aura : ModuleRules
{
	public Aura(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject",
			"Engine", 
			"InputCore", 
			"GameplayAbilities",
			"ModularGameplay",
			"ModularGameplayActors"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UMG",
			"EnhancedInput",
			"GameplayTags",
			"GameplayTasks",
			"NavigationSystem",
			"Niagara",
			"CommonUI",
			"CommonUser",
			"CommonGame",
			"AIModule"
		});

	}
}
