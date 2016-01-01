// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class AvinationViewer : ModuleRules
{
	public AvinationViewer(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "HTTP", "XmlParser" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }

        PublicAdditionalLibraries.Add("/usr/lib/libz.dylib");
        PublicAdditionalLibraries.Add("/usr/local/lib/libopenjp2.dylib");
        PublicIncludePaths.Add("/usr/local/include/openjpeg-2.1");
	}
}
