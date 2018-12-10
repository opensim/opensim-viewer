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
#pragma once

extern "C"
{
#include "whio_amalgamation.h"
}

//AVINATIONVIEWER_API DECLARE_LOG_CATEGORY_EXTERN(LogWhio, Log, All);

/**
 * Platform file wrapper to be able to use whio files.
 **/
class AVINATIONVIEWER_API FWhioPlatformFile : public IPlatformFile
{
	/** Wrapped file */
	IPlatformFile* LowerLevel;
    FString gamePath;
    
public:

	static const TCHAR* GetTypeName()
	{
		return TEXT("WhioFile");
	}

	/**
	 * Constructor.
	 * 
	 * @param InLowerLevel Wrapper platform file.
	 */
	FWhioPlatformFile();

	/**
	 * Destructor.
	 */
	virtual ~FWhioPlatformFile();

	virtual bool ShouldBeUsed(IPlatformFile* Inner, const TCHAR* CmdLine) const override;
	virtual bool Initialize(IPlatformFile* Inner, const TCHAR* CommandLineParam) override;

	virtual IPlatformFile* GetLowerLevel() override
	{
		return LowerLevel;
	}

	virtual const TCHAR* GetName() const override
	{
		return FWhioPlatformFile::GetTypeName();
	}

	bool Mount(const TCHAR* InWhioFilename, uint32 PakOrder, const TCHAR* InPath = NULL);
	bool Unmount(const TCHAR* InWhioFilename);

	// BEGIN IPlatformFile Interface
	virtual bool FileExists(const TCHAR* Filename) override;
	virtual int64 FileSize(const TCHAR* Filename) override;
	virtual bool DeleteFile(const TCHAR* Filename) override;
	virtual bool IsReadOnly(const TCHAR* Filename) override;
	virtual bool MoveFile(const TCHAR* To, const TCHAR* From) override;
	virtual bool SetReadOnly(const TCHAR* Filename, bool bNewReadOnlyValue) override;
	virtual FDateTime GetTimeStamp(const TCHAR* Filename) override;
	virtual void SetTimeStamp(const TCHAR* Filename, FDateTime DateTime) override;
	virtual FDateTime GetAccessTimeStamp(const TCHAR* Filename) override;
	virtual FString GetFilenameOnDisk(const TCHAR* Filename) override;
	virtual IFileHandle* OpenRead(const TCHAR* Filename, bool bAllowWrite = false) override;
	virtual IFileHandle* OpenWrite(const TCHAR* Filename, bool bAppend = false, bool bAllowRead = false) override;
	virtual bool DirectoryExists(const TCHAR* Directory) override;
	virtual bool CreateDirectory(const TCHAR* Directory) override;
	virtual bool DeleteDirectory(const TCHAR* Directory) override;
	virtual bool IterateDirectory(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor) override;
	virtual bool IterateDirectoryRecursively(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor) override;
	virtual bool DeleteDirectoryRecursively(const TCHAR* Directory) override;
	virtual bool CreateDirectoryTree(const TCHAR* Directory) override;
	virtual bool CopyFile(const TCHAR* To, const TCHAR* From) override;
	FString ConvertToAbsolutePathForExternalAppForRead(const TCHAR* Filename) override;
	FString ConvertToAbsolutePathForExternalAppForWrite(const TCHAR* Filename) override;
    virtual FFileStatData GetStatData(const TCHAR* FilenameOrDirectory) override;
    virtual bool IterateDirectoryStat(const TCHAR* Directory, FDirectoryStatVisitor& Visitor) override;
	// END IPlatformFile Interface

	// BEGIN Console commands
#if !UE_BUILD_SHIPPING
	void HandleWhioListCommand(const TCHAR* Cmd, FOutputDevice& Ar);
	void HandleMountCommand(const TCHAR* Cmd, FOutputDevice& Ar);
	void HandleUnmountCommand(const TCHAR* Cmd, FOutputDevice& Ar);
#endif
	// END Console commands
};


