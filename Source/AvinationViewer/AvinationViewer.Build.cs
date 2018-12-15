/*
 * Copyright (c) Contributors, http://opensimulator.org/
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSimulator Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
using UnrealBuildTool;
using System;
using System.IO;

public class AvinationViewer : ModuleRules
{
	public AvinationViewer(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "HTTP", "XmlParser", "RHI", "RenderCore", "UMG", "Slate", "SlateCore", "Json" });

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
		var thisPath = ModuleDirectory;
		var depsPath = Path.Combine(thisPath,"..","..","Deps");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			var windeps = Path.Combine(depsPath,"Windows");
			PublicIncludePaths.Add(windeps);
			PublicAdditionalLibraries.Add(Path.Combine(windeps,"zlibwapi.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(windeps,"openjp2.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(windeps,"whio_amalgamation.lib"));
			//Target.bForceEnableExceptions = true;
//			PrivateIncludePathModuleNames.Add("TextureCompressor");
//			PrivateIncludePaths.AddRange(new string[] {
//				"Developer/Windows/WindowsTargetPlatform/Private"});
		}
		else if ( Target.Platform == UnrealTargetPlatform.Linux )
		{
			var linuxdeps = Path.Combine(depsPath,"Linux");
			PublicIncludePaths.Add(linuxdeps);
			PublicAdditionalLibraries.Add("/lib/libz");
			PublicAdditionalLibraries.Add("/usr/lib64/libopenjp2.so");
			PublicAdditionalLibraries.Add(Path.Combine(linuxdeps,"libwhio_amalgamation.a"));
			PublicIncludePaths.Add("/usr/include");
			PublicIncludePaths.Add("/usr/include/openjpeg-2.1");
			
		} 
		else if ( Target.Platform == UnrealTargetPlatform.Mac )
		{
			var osxdeps = Path.Combine(depsPath,"Mac");
			PublicIncludePaths.Add(osxdeps);
			PublicAdditionalLibraries.Add(Path.Combine(osxdeps,"libwhio_amalgamation.a"));
			PublicAdditionalLibraries.Add("/usr/lib/libz.dylib");
			PublicAdditionalLibraries.Add("/usr/local/lib/libopenjp2.dylib");
			PublicIncludePaths.Add("/usr/local/include/openjpeg-2.1");
		} // additional platforms
	}
}
