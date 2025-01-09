// Copyright Berkeley Bidwell

using UnrealBuildTool;
using System.Collections.Generic;
using Microsoft.Extensions.Logging;

public class AuraTarget : TargetRules
{
	public AuraTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		
		ExtraModuleNames.AddRange( new string[] { "Aura" } );
		
		AuraTarget.ApplySharedAuraTargetSettings(this);
	}
	
	private static bool bHasWarnedAboutShared = false;

	internal static void ApplySharedAuraTargetSettings(TargetRules Target)
	{
		ILogger Logger = Target.Logger;
		
		Target.DefaultBuildSettings = BuildSettingsVersion.Latest;
		Target.IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		bool bIsTest = Target.Configuration == UnrealTargetConfiguration.Test;
		bool bIsShipping = Target.Configuration == UnrealTargetConfiguration.Shipping;
		bool bIsDedicatedServer = Target.Type == TargetType.Server;

		if (Target.BuildEnvironment == TargetBuildEnvironment.Unique)
		{
			Target.ShadowVariableWarningLevel = WarningLevel.Error;

			Target.bUseLoggingInShipping = true;

			if (bIsShipping && !bIsDedicatedServer)
			{
				// Make sure that we validate certificates for HTTPS traffic
				Target.bDisableUnverifiedCertificates = true;

				// Uncomment these lines to lock down the command line processing
				// This will only allow the specified command line arguments to be parsed
				//Target.GlobalDefinitions.Add("UE_COMMAND_LINE_USES_ALLOW_LIST=1");
				//Target.GlobalDefinitions.Add("UE_OVERRIDE_COMMAND_LINE_ALLOW_LIST=\"-space -separated -list -of -commands\"");

				// Uncomment this line to filter out sensitive command line arguments that you
				// don't want to go into the log file (e.g., if you were uploading logs)
				//Target.GlobalDefinitions.Add("FILTER_COMMANDLINE_LOGGING=\"-some_connection_id -some_other_arg\"");
			}

			if (bIsShipping || bIsTest)
			{
				// Disable reading generated/non-ufs ini files
				Target.bAllowGeneratedIniWhenCooked = false;
				Target.bAllowNonUFSIniWhenCooked = false;
			}

			if (Target.Type != TargetType.Editor)
			{
				// We don't use the path tracer at runtime, only for beauty shots, and this DLL is quite large
				Target.DisablePlugins.Add("OpenImageDenoise");

				// Reduce memory use in AssetRegistry always-loaded data, but add more cputime expensive queries
				Target.GlobalDefinitions.Add("UE_ASSETREGISTRY_INDIRECT_ASSETDATA_POINTERS=1");
			}

			//LyraGameTarget.ConfigureGameFeaturePlugins(Target);
		}
		else
		{
			// !!!!!!!!!!!! WARNING !!!!!!!!!!!!!
			// Any changes in here must not affect PCH generation, or the target
			// needs to be set to TargetBuildEnvironment.Unique

			// This only works in editor or Unique build environments
			if (Target.Type == TargetType.Editor)
			{
				//LyraGameTarget.ConfigureGameFeaturePlugins(Target);
			}
			else
			{
				// Shared monolithic builds cannot enable/disable plugins or change any options because it tries to re-use the installed engine binaries
				if (!bHasWarnedAboutShared)
				{
					bHasWarnedAboutShared = true;
					Logger.LogWarning("AuraGameEOS and dynamic target options are disabled when packaging from an installed version of the engine");
				}
			}
		}

	}
}
