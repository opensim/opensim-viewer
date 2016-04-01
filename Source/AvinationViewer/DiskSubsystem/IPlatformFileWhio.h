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


