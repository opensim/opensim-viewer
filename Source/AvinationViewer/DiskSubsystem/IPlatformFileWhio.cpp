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
#include "AvinationViewer.h"
#include "IPlatformFileWhio.h"
#include "FileManagerGeneric.h"
#include "IOBase.h"

FWhioPlatformFile::FWhioPlatformFile()
{
}

FWhioPlatformFile::~FWhioPlatformFile()
{
}

bool FWhioPlatformFile::ShouldBeUsed(IPlatformFile* Inner, const TCHAR* CmdLine) const
{
    // Does this ever get called?
	return true;
}

bool FWhioPlatformFile::Initialize(IPlatformFile* Inner, const TCHAR* CommandLineParam)
{
    LowerLevel = Inner;
    gamePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::GameDir(), TEXT("cache")));
	return true;
}

bool FWhioPlatformFile::Mount(const TCHAR* InWhioFilename, uint32 PakOrder, const TCHAR* InPath)
{
	return false;
}

bool FWhioPlatformFile::Unmount(const TCHAR* InWhioFilename)
{
	return false;
}

// BEGIN IPlatformFile Interface
bool FWhioPlatformFile::FileExists(const TCHAR* Filename)
{
    return LowerLevel->FileExists(Filename);
}

int64 FWhioPlatformFile::FileSize(const TCHAR* Filename)
{
    return LowerLevel->FileSize(Filename);
}

bool FWhioPlatformFile::DeleteFile(const TCHAR* Filename)
{
    return LowerLevel->DeleteFile(Filename);
}

bool FWhioPlatformFile::IsReadOnly(const TCHAR* Filename)
{
    return LowerLevel->IsReadOnly(Filename);
}

bool FWhioPlatformFile::MoveFile(const TCHAR* To, const TCHAR* From)
{
    return LowerLevel->MoveFile(To, From);
}

bool FWhioPlatformFile::SetReadOnly(const TCHAR* Filename, bool bNewReadOnlyValue)
{
    return LowerLevel->SetReadOnly(Filename, bNewReadOnlyValue);
}

FDateTime FWhioPlatformFile::GetTimeStamp(const TCHAR* Filename)
{
    return LowerLevel->GetTimeStamp(Filename);
}

void FWhioPlatformFile::SetTimeStamp(const TCHAR* Filename, FDateTime DateTime)
{
    LowerLevel->SetTimeStamp(Filename, DateTime);
}

FDateTime FWhioPlatformFile::GetAccessTimeStamp(const TCHAR* Filename)
{
	return FDateTime();
}

FString FWhioPlatformFile::GetFilenameOnDisk(const TCHAR* Filename)
{
    return LowerLevel->GetFilenameOnDisk(Filename);
}

IFileHandle* FWhioPlatformFile::OpenRead(const TCHAR* Filename, bool bAllowWrite)
{
    //if (FPaths::ConvertRelativePathToFull(FString(Filename)).StartsWith(gamePath))
    //UE_LOG(LogTemp, Warning, TEXT("OpenRead %s"), *FPaths::ConvertRelativePathToFull(FString(Filename)));
    return LowerLevel->OpenRead(Filename, bAllowWrite);

}

IFileHandle* FWhioPlatformFile::OpenWrite(const TCHAR* Filename, bool bAppend, bool bAllowRead)
{
    return LowerLevel->OpenWrite(Filename, bAppend, bAllowRead);
}

bool FWhioPlatformFile::DirectoryExists(const TCHAR* Directory)
{
    return LowerLevel->DirectoryExists(Directory);
}

bool FWhioPlatformFile::CreateDirectory(const TCHAR* Directory)
{
    return LowerLevel->CreateDirectory(Directory);
}

bool FWhioPlatformFile::DeleteDirectory(const TCHAR* Directory)
{
    return LowerLevel->DeleteDirectory(Directory);
}

bool FWhioPlatformFile::IterateDirectory(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor)
{
    return LowerLevel->IterateDirectory(Directory, Visitor);
}

bool FWhioPlatformFile::IterateDirectoryRecursively(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor)
{
    return LowerLevel->IterateDirectoryRecursively(Directory, Visitor);
}

bool FWhioPlatformFile::DeleteDirectoryRecursively(const TCHAR* Directory)
{
    return LowerLevel->DeleteDirectoryRecursively(Directory);
}

bool FWhioPlatformFile::CreateDirectoryTree(const TCHAR* Directory)
{
    return LowerLevel->CreateDirectoryTree(Directory);
}

bool FWhioPlatformFile::CopyFile(const TCHAR* To, const TCHAR* From)
{
    return LowerLevel->CopyFile(To, From);
}

FString FWhioPlatformFile::ConvertToAbsolutePathForExternalAppForRead(const TCHAR* Filename)
{
    return LowerLevel->ConvertToAbsolutePathForExternalAppForRead(Filename);
}

FString FWhioPlatformFile::ConvertToAbsolutePathForExternalAppForWrite(const TCHAR* Filename)
{
    return LowerLevel->ConvertToAbsolutePathForExternalAppForWrite(Filename);
}

// END IPlatformFile Interface

// BEGIN Console commands
#if !UE_BUILD_SHIPPING
void FWhioPlatformFile::HandleWhioListCommand(const TCHAR* Cmd, FOutputDevice& Ar)
{
}

void FWhioPlatformFile::HandleMountCommand(const TCHAR* Cmd, FOutputDevice& Ar)
{
}

void FWhioPlatformFile::HandleUnmountCommand(const TCHAR* Cmd, FOutputDevice& Ar)
{
}

FFileStatData FWhioPlatformFile::GetStatData(const TCHAR* FilenameOrDirectory)
{
    return LowerLevel->GetStatData(FilenameOrDirectory);
}

bool FWhioPlatformFile::IterateDirectoryStat(const TCHAR* Directory, FDirectoryStatVisitor& Visitor)
{
    return LowerLevel->IterateDirectoryStat(Directory, Visitor);
}

#endif
// END Console commands


