#include "path.h"
#include "std_string.h"
#include <Windows.h>

#include <CommDlg.h>
#include <io.h>
#include <shlobj_core.h>

const char DRIVE_DELIMITER = ':';
const char * const DIR_DOUBLEDELIM = "\\\\";
const char DIRECTORY_DELIMITER = '\\';
const char DIRECTORY_DELIMITER2 = '/';

const char EXTENSION_DELIMITER = '.';
void * Path::m_hInst = nullptr;

Path::Path()
{
}

Path::Path(const Path & path)
{
    m_path = path.m_path;
}

Path::Path(const Path & path, const char * fileName)
{
    SetDriveDirectory(path.m_path.c_str());
    SetNameExtension(fileName);
}

Path::Path(const char * path)
{
    m_path = path ? path : "";
    CleanPath(m_path);
}

Path::Path(const char * path, const char * fileName)
{
    SetDriveDirectory(path);
    SetNameExtension(fileName);
}

Path::Path(const std::string & path)
{
    m_path = path;
    CleanPath(m_path);
}

Path::Path(const std::string & path, const char * fileName)
{
    SetDriveDirectory(path.c_str());
    SetNameExtension(fileName);
}

Path::Path(const std::string & path, const std::string & fileName)
{
    SetDriveDirectory(path.c_str());
    SetNameExtension(fileName.c_str());
}

Path::Path(DIR_CURRENT_DIRECTORY /*sdt*/, const char * nameExten)
{
    SetToCurrentDirectory();
    if (nameExten)
    {
        SetNameExtension(nameExten);
    }
}

Path::Path(DIR_MODULE_DIRECTORY /*sdt*/, const char * nameExten)
{
    SetToModuleDirectory();
    SetNameExtension(nameExten ? nameExten : "");
}

Path::~Path()
{
    CloseFindHandle();
}

Path & Path::operator=(const Path & path)
{
    if (this != &path)
    {
        m_path = path.m_path;
    }
    return *this;
}

Path::operator const char *() const
{
    return (const char *)m_path.c_str();
}

Path::operator const std::string &()
{
    return m_path;
}

void Path::GetDriveDirectory(std::string & driveDirectory) const
{
    std::string drive, directory;
    GetComponents(&drive, &directory);
    driveDirectory = drive;
    if (!drive.empty())
    {
        driveDirectory += DRIVE_DELIMITER;
    }
    driveDirectory += directory;
}

std::string Path::GetDriveDirectory(void) const
{
    std::string driveDirectory;
    GetDriveDirectory(driveDirectory);
    return driveDirectory;
}

void Path::GetDirectory(std::string & directory) const
{
    GetComponents(nullptr, &directory);
}

std::string Path::GetDirectory(void) const
{
    std::string directory;
    GetDirectory(directory);
    return directory;
}

void Path::GetNameExtension(std::string & nameExtension) const
{
    std::string name, extension;
    GetComponents(nullptr, nullptr, &name, &extension);
    nameExtension = name;
    if (!extension.empty())
    {
        nameExtension += EXTENSION_DELIMITER;
        nameExtension += extension;
    }
}

std::string Path::GetNameExtension(void) const
{
    std::string nameExtension;
    GetNameExtension(nameExtension);
    return nameExtension;
}

void Path::GetExtension(std::string & extension) const
{
    GetComponents(nullptr, nullptr, nullptr, &extension);
}

std::string Path::GetExtension(void) const
{
    std::string extension;
    GetExtension(extension);
    return extension;
}

void Path::GetComponents(std::string * drive, std::string * directory, std::string * name, std::string * extension) const
{
    char driveBuff[_MAX_DRIVE + 1] = {0}, dirBuff[_MAX_DIR + 1] = {0}, nameBuff[_MAX_FNAME + 1] = {0}, extBuff[_MAX_EXT + 1] = {0};

    const char * basePath = m_path.c_str();
    const char * driveDir = strrchr(basePath, DRIVE_DELIMITER);
    if (driveDir != nullptr)
    {
        size_t len = sizeof(dirBuff) < (driveDir - basePath) ? sizeof(driveBuff) : driveDir - basePath;
        strncpy(driveBuff, basePath, len);
        basePath += len + 1;
    }

    const char * last = strrchr(basePath, DIRECTORY_DELIMITER);
    if (last != nullptr)
    {
        size_t len = sizeof(dirBuff) < (last - basePath) ? sizeof(dirBuff) : last - basePath;
        if (len > 0)
        {
            strncpy(dirBuff, basePath, len);
        }
        else
        {
            dirBuff[0] = DIRECTORY_DELIMITER;
            dirBuff[1] = '\0';
        }
        strncpy(nameBuff, last + 1, sizeof(nameBuff));
    }
    else
    {
        strncpy(dirBuff, basePath, sizeof(dirBuff));
    }
    char * ext = strrchr(nameBuff, '.');
    if (ext != nullptr)
    {
        strncpy(extBuff, ext + 1, sizeof(extBuff));
        *ext = '\0';
    }

    if (drive)
    {
        *drive = driveBuff;
    }
    if (directory)
    {
        *directory = dirBuff;
    }
    if (name)
    {
        *name = nameBuff;
    }
    if (extension)
    {
        *extension = extBuff;
    }
}

bool Path::IsRelative() const
{
    if (m_path.length() > 1 && m_path[1] == DRIVE_DELIMITER)
    {
        return false;
    }
    if (m_path.length() > 2 && m_path[0] == DIRECTORY_DELIMITER && m_path[1] == DIRECTORY_DELIMITER)
    {
        return false;
    }
    return true;
}

void Path::SetDriveDirectory(const char * driveDirectory)
{
    std::string driveDirectoryStr = driveDirectory;
    if (driveDirectoryStr.length() > 0)
    {
        EnsureTrailingBackslash(driveDirectoryStr);
        CleanPath(driveDirectoryStr);
    }

    std::string name, extension;
    GetComponents(nullptr, nullptr, &name, &extension);
    SetComponents(nullptr, driveDirectoryStr.c_str(), name.c_str(), extension.c_str());
}

void Path::SetDirectory(const char * directory, bool ensureAbsolute)
{
    std::string newDirectory = directory;
    if (ensureAbsolute)
    {
        EnsureLeadingBackslash(newDirectory);
    }
    if (newDirectory.length() > 0)
    {
        EnsureTrailingBackslash(newDirectory);
    }
    std::string drive, name, extension;
    GetComponents(&drive, nullptr, &name, &extension);
    SetComponents(drive.c_str(), newDirectory.c_str(), name.c_str(), extension.c_str());
}

void Path::SetNameExtension(const char * nameExtension)
{
    std::string directory, drive;
    GetComponents(&drive, &directory);
    SetComponents(drive.c_str(), directory.c_str(), nameExtension, nullptr);
}

void Path::SetComponents(const char * drive, const char * directory, const char * name, const char * extension)
{
    char fullname[MAX_PATH];

    memset(fullname, 0, sizeof(fullname));
    if (directory == nullptr || strlen(directory) == 0)
    {
        static char emptyDir[] = {DIRECTORY_DELIMITER, '\0'};
        directory = emptyDir;
    }

    _makepath(fullname, drive, directory, name, extension);
    m_path = fullname;
}

void Path::AppendDirectory(const char * subDirectory)
{
    std::string newSubDirectory = subDirectory;
    if (newSubDirectory.empty())
    {
        return;
    }

    StripLeadingBackslash(newSubDirectory);
    EnsureTrailingBackslash(newSubDirectory);

    std::string drive, directory, name, extension;
    GetComponents(&drive, &directory, &name, &extension);
    EnsureTrailingBackslash(directory);
    directory += newSubDirectory;
    SetComponents(drive.c_str(), directory.c_str(), name.c_str(), extension.c_str());
}

bool Path::FileDelete(bool evenIfReadOnly) const
{
    std::wstring filePath = stdstr(m_path).ToUTF16();
    uint32_t attributes = ::GetFileAttributes(filePath.c_str());
    if (attributes == (uint32_t)-1)
    {
        return false;
    }

    if (((attributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY) && !evenIfReadOnly)
    {
        return false;
    }

    SetFileAttributes(filePath.c_str(), FILE_ATTRIBUTE_NORMAL);
    return DeleteFile(filePath.c_str()) != 0;
}

bool Path::FileExists() const
{
    WIN32_FIND_DATA FindData;
    HANDLE hFindFile = FindFirstFile(stdstr(m_path).ToUTF16().c_str(), &FindData);
    bool bSuccess = (hFindFile != INVALID_HANDLE_VALUE);

    if (hFindFile != nullptr) // Make sure we close the search
    {
        FindClose(hFindFile);
    }

    return bSuccess;
}

bool Path::FileSelect(void * hwndOwner, const char * initialDir, const char * fileFilter, bool fileMustExist)
{
    size_t filterLen = 0;
    while (fileFilter[filterLen] != '\0' || fileFilter[filterLen + 1] != '\0')
    {
        filterLen++;
    }
    filterLen += 2;

    std::vector<wchar_t> fileFilterW(filterLen);
    MultiByteToWideChar(CP_UTF8, 0, fileFilter, (int)filterLen, fileFilterW.data(), static_cast<int>(filterLen));

    Path currentDir(CURRENT_DIRECTORY);
    std::wstring initialDirW = stdstr(initialDir).ToUTF16();

    OPENFILENAME openfilename = {};
    std::vector<wchar_t> fileName(32768);

    openfilename.lStructSize = sizeof(openfilename);
    openfilename.hwndOwner = (HWND)hwndOwner;
    openfilename.lpstrFilter = fileFilterW.data();
    openfilename.lpstrFile = fileName.data();
    openfilename.lpstrInitialDir = initialDirW.c_str();
    openfilename.nMaxFile = (DWORD)fileName.size();
    openfilename.Flags = OFN_HIDEREADONLY | (fileMustExist ? OFN_FILEMUSTEXIST : 0);

    bool res = GetOpenFileName(&openfilename) != 0;
    if (Path(CURRENT_DIRECTORY) != currentDir)
    {
        currentDir.DirectoryChange();
    }
    if (!res)
    {
        return false;
    }
    m_path = stdstr().FromUTF16(fileName.data());
    CleanPath(m_path);
    return true;
}

Path & Path::BrowseForDirectory(void * parentWindow, const char * title)
{
    *this = Path();
    const HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    IFileDialog* dlg = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&dlg));
    if (SUCCEEDED(hr))
    {
        DWORD options = 0;
        if (SUCCEEDED(dlg->GetOptions(&options)))
        {
            options |= FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR;
            dlg->SetOptions(options);
        }

        if (title && *title)
        {
            dlg->SetTitle(stdstr_f(title).ToUTF16().c_str());
        }
        hr = dlg->Show((HWND)parentWindow);
        if (SUCCEEDED(hr))
        {
            IShellItem* result = nullptr;
            if (SUCCEEDED(dlg->GetResult(&result)) && result)
            {
                PWSTR psz = nullptr;
                if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &psz)) && psz)
                {
                    *this = Path(stdstr().FromUTF16(psz).c_str(), "");
                    CoTaskMemFree(psz);
                }
                result->Release();
            }
        }
        dlg->Release();
    }

    if (SUCCEEDED(hrCo))
    {
        CoUninitialize();
    }
    return *this;
}

bool Path::IsDirectory() const
{
    std::string fileName;
    GetNameExtension(fileName);
    return fileName.empty();
}

bool Path::DirectoryCreate(bool createIntermediates)
{
    if (DirectoryExists())
    {
        return true;
    }

    std::string path;
    GetDriveDirectory(path);
    StripTrailingBackslash(path);
    bool bSuccess = ::CreateDirectory(stdstr(path).ToUTF16().c_str(), nullptr) != 0;
    if (!bSuccess && createIntermediates)
    {
        std::string::size_type delimiter = path.rfind(DIRECTORY_DELIMITER);
        if (delimiter == std::string::npos)
        {
            return false;
        }

        path.resize(delimiter + 1);
        return Path(path).DirectoryCreate() ? DirectoryCreate(false) : false;
    }
    return bSuccess;
}

bool Path::DirectoryChange() const
{
    std::string driveDirectory;
    GetDriveDirectory(driveDirectory);

    return SetCurrentDirectory(stdstr(driveDirectory).ToUTF16().c_str()) != 0;
}

bool Path::DirectoryExists() const
{
    Path path(m_path.c_str());

    std::string directory;
    path.DirectoryUp(&directory);
    path.SetNameExtension(directory.c_str());

    WIN32_FIND_DATA FindData;
    HANDLE findFile = FindFirstFile(stdstr((const char *)path).ToUTF16().c_str(), &FindData);
    bool res = (findFile != INVALID_HANDLE_VALUE);
    if (findFile != INVALID_HANDLE_VALUE)
    {
        FindClose(findFile);
    }
    return res;
}

Path & Path::DirectoryNormalize(Path BaseDir)
{
    stdstr directory = BaseDir.GetDriveDirectory();
    bool changed = false;
    if (IsRelative())
    {
        EnsureTrailingBackslash(directory);
        directory += GetDirectory();
        changed = true;
    }
    strvector parts = directory.Tokenize(DIRECTORY_DELIMITER);
    strvector normalizesParts;
    for (strvector::const_iterator itr = parts.begin(); itr != parts.end(); itr++)
    {
        if (*itr == ".")
        {
            changed = true;
        }
        else if (*itr == "..")
        {
            normalizesParts.pop_back();
            changed = true;
        }
        else
        {
            normalizesParts.push_back(*itr);
        }
    }
    if (changed)
    {
        directory.clear();
        for (strvector::const_iterator itr = normalizesParts.begin(); itr != normalizesParts.end(); itr++)
        {
            directory += *itr + DIRECTORY_DELIMITER;
        }
        SetDriveDirectory(directory.c_str());
    }
    return *this;
}

void Path::DirectoryUp(std::string * lastDir)
{
    std::string directory;
    GetDirectory(directory);
    StripTrailingBackslash(directory);
    if (directory.empty())
    {
        return;
    }
    std::string::size_type delimiter = directory.rfind(DIRECTORY_DELIMITER);
    if (lastDir != nullptr)
    {
        *lastDir = directory.substr(delimiter);
        StripLeadingBackslash(*lastDir);
    }

    if (delimiter != std::string::npos)
    {
        directory = directory.substr(0, delimiter);
    }
    SetDirectory(directory.c_str());
}

bool Path::FindFirst(uint32_t attributes)
{
    CloseFindHandle();

    m_findAttributes = attributes;
    bool bWantSubdirectory = (FIND_ATTRIBUTE_SUBDIR & attributes) != 0;

    WIN32_FIND_DATA findData;
    m_findHandle = FindFirstFile(stdstr(m_path).ToUTF16().c_str(), &findData);
    bool foundFile = (m_findHandle != INVALID_HANDLE_VALUE);

    if (m_findHandle == INVALID_HANDLE_VALUE)
    {
        m_findHandle = nullptr;
    }

    while (foundFile)
    {
        if (AttributesMatch(m_findAttributes, findData.dwFileAttributes) &&
            (!bWantSubdirectory || (findData.cFileName[0] != '.')))
        {
            if ((FIND_ATTRIBUTE_SUBDIR & findData.dwFileAttributes) != 0)
            {
                StripTrailingBackslash(m_path);
            }
            SetNameExtension(stdstr().FromUTF16(findData.cFileName).c_str());
            if ((FIND_ATTRIBUTE_SUBDIR & findData.dwFileAttributes) != 0)
            {
                EnsureTrailingBackslash(m_path);
            }
            return true;
        }
        foundFile = FindNextFile(m_findHandle, &findData);
    }
    return false;
}

bool Path::FindNext()
{
    if (m_findHandle == nullptr)
    {
        return false;
    }

    WIN32_FIND_DATA FindData;
    while (FindNextFile(m_findHandle, &FindData) != false)
    {
        if (AttributesMatch(m_findAttributes, FindData.dwFileAttributes))
        {
            if ((_A_SUBDIR & FindData.dwFileAttributes) == _A_SUBDIR)
            {
                if (IsDirectory())
                {
                    DirectoryUp();
                }
                else
                {
                    SetNameExtension("");
                }
                AppendDirectory(stdstr().FromUTF16(FindData.cFileName).c_str());
            }
            else
            {
                if (IsDirectory())
                {
                    DirectoryUp();
                }
                SetNameExtension(stdstr().FromUTF16(FindData.cFileName).c_str());
            }
            if ((_A_SUBDIR & FindData.dwFileAttributes) == _A_SUBDIR)
            {
                EnsureTrailingBackslash(m_path);
            }
            return true;
        }
    }
    return false;
}

void Path::SetToCurrentDirectory()
{
    DWORD required = ::GetCurrentDirectory(0, nullptr);
    if (required == 0)
    {
        m_path = "";
        return;
    }
    std::vector<wchar_t> path(required);
    DWORD result = ::GetCurrentDirectory(required, path.data());
    if (result == 0 || result > required)
    {
        m_path = "";
        return;
    }
    SetDriveDirectory(stdstr().FromUTF16(path.data()).c_str());
}

void Path::SetToModuleDirectory()
{
    DWORD bufferSize = MAX_PATH;
    std::vector<wchar_t> buffPath(bufferSize);

    while (true)
    {
        DWORD result = GetModuleFileNameW((HINSTANCE)m_hInst, buffPath.data(), bufferSize);

        if (result == 0)
        {
            m_path = "";
            return;
        }

        if (result < bufferSize)
        {
            m_path = stdstr().FromUTF16(buffPath.data());
            SetNameExtension("");
            return;
        }

        bufferSize *= 2;
        buffPath.resize(bufferSize);
    }
}

bool Path::AttributesMatch(uint32_t targetAttributes, uint32_t fileAttributes)
{
    if (targetAttributes == FIND_ATTRIBUTE_ALLFILES)
    {
        return true;
    }
    if (targetAttributes == FIND_ATTRIBUTE_FILES)
    {
        return ((FIND_ATTRIBUTE_SUBDIR & fileAttributes) == 0);
    }
    return (((targetAttributes & fileAttributes) != 0) && ((FIND_ATTRIBUTE_SUBDIR & targetAttributes) == (FIND_ATTRIBUTE_SUBDIR & fileAttributes)));
}

void Path::CloseFindHandle()
{
    if (m_findHandle != nullptr)
    {
        FindClose(m_findHandle);
        m_findHandle = nullptr;
    }
}

void Path::CleanPath(std::string & path) const
{
    std::string::size_type pos = path.find(DIRECTORY_DELIMITER2);
    while (pos != std::string::npos)
    {
        path.replace(pos, 1, &DIRECTORY_DELIMITER);
        pos = path.find(DIRECTORY_DELIMITER2, pos + 1);
    }

    bool appendEnd = !_strnicmp(path.c_str(), DIR_DOUBLEDELIM, 2);
    pos = path.find(DIR_DOUBLEDELIM);
    while (pos != std::string::npos)
    {
        path.replace(pos, 2, &DIRECTORY_DELIMITER);
        pos = path.find(DIR_DOUBLEDELIM, pos + 1);
    }
    if (appendEnd)
    {
        path.insert(0, stdstr_f("%c", DIRECTORY_DELIMITER).c_str());
    }
}

void Path::EnsureLeadingBackslash(std::string & directory) const
{
    if (directory.empty() || (directory[0] != DIRECTORY_DELIMITER))
    {
        directory = stdstr_f("%c%s", DIRECTORY_DELIMITER, directory.c_str());
    }
}

void Path::EnsureTrailingBackslash(std::string & directory) const
{
    std::string::size_type length = directory.length();

    if (directory.empty() || (directory[length - 1] != DIRECTORY_DELIMITER))
    {
        directory += DIRECTORY_DELIMITER;
    }
}

void Path::StripLeadingBackslash(std::string & directory) const
{
    if (directory.length() <= 1)
    {
        return;
    }

    if (directory[0] == DIRECTORY_DELIMITER)
    {
        directory = directory.substr(1);
    }
}

void Path::StripTrailingBackslash(std::string & directory) const
{
    for (;;)
    {
        std::string::size_type length = directory.length();
        if (length <= 1)
        {
            break;
        }

        if (directory[length - 1] == DIRECTORY_DELIMITER || directory[length - 1] == DIRECTORY_DELIMITER2)
        {
            directory.resize(length - 1);
            continue;
        }
        break;
    }
}
