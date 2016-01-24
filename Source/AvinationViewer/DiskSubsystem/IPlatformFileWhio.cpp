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
	return true;
}

bool FWhioPlatformFile::Initialize(IPlatformFile* Inner, const TCHAR* CommandLineParam)
{
	return false;
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
	return false;
}

int64 FWhioPlatformFile::FileSize(const TCHAR* Filename)
{
	return 0;
}

bool FWhioPlatformFile::DeleteFile(const TCHAR* Filename)
{
	return false;
}

bool FWhioPlatformFile::IsReadOnly(const TCHAR* Filename)
{
	return false;
}

bool FWhioPlatformFile::MoveFile(const TCHAR* To, const TCHAR* From)
{
	return false;
}

bool FWhioPlatformFile::SetReadOnly(const TCHAR* Filename, bool bNewReadOnlyValue)
{
	return false;
}

FDateTime FWhioPlatformFile::GetTimeStamp(const TCHAR* Filename)
{
	return FDateTime();
}

void FWhioPlatformFile::SetTimeStamp(const TCHAR* Filename, FDateTime DateTime)
{
}

FDateTime FWhioPlatformFile::GetAccessTimeStamp(const TCHAR* Filename)
{
	return FDateTime();
}

FString FWhioPlatformFile::GetFilenameOnDisk(const TCHAR* Filename)
{
	return FString();
}

IFileHandle* FWhioPlatformFile::OpenRead(const TCHAR* Filename, bool bAllowWrite)
{
	return 0;
}

IFileHandle* FWhioPlatformFile::OpenWrite(const TCHAR* Filename, bool bAppend, bool bAllowRead)
{
	return 0;
}

bool FWhioPlatformFile::DirectoryExists(const TCHAR* Directory)
{
	return false;
}

bool FWhioPlatformFile::CreateDirectory(const TCHAR* Directory)
{
	return false;
}

bool FWhioPlatformFile::DeleteDirectory(const TCHAR* Directory)
{
	return false;
}

bool FWhioPlatformFile::IterateDirectory(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor)
{
	return false;
}

bool FWhioPlatformFile::IterateDirectoryRecursively(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor)
{
	return false;
}

bool FWhioPlatformFile::DeleteDirectoryRecursively(const TCHAR* Directory)
{
	return false;
}

bool FWhioPlatformFile::CreateDirectoryTree(const TCHAR* Directory)
{
	return false;
}

bool FWhioPlatformFile::CopyFile(const TCHAR* To, const TCHAR* From)
{
	return false;
}

FString FWhioPlatformFile::ConvertToAbsolutePathForExternalAppForRead(const TCHAR* Filename)
{
	return FString();
}

FString FWhioPlatformFile::ConvertToAbsolutePathForExternalAppForWrite(const TCHAR* Filename)
{
	return FString();
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

#endif
// END Console commands


