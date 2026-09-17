#include "MainForm.h"

#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>

#include <cwctype>
#include <sstream>
#include <string>

using namespace ezui;

namespace {
    struct StepResult {
        bool ok;
        UIString title;
        UIString detail;

        StepResult(bool success, const UIString& itemTitle, const UIString& itemDetail)
            : ok(success), title(itemTitle), detail(itemDetail)
        {
        }
    };

    std::wstring TrimWide(std::wstring text)
    {
        while (!text.empty() && std::iswspace(text.front())) {
            text.erase(text.begin());
        }
        while (!text.empty() && std::iswspace(text.back())) {
            text.pop_back();
        }
        return text;
    }

    std::wstring RemoveQuotes(std::wstring text)
    {
        text = TrimWide(text);
        if (text.size() >= 2 && text.front() == L'"' && text.back() == L'"') {
            return text.substr(1, text.size() - 2);
        }
        return text;
    }

    std::wstring NormalizeSlashes(std::wstring text)
    {
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == L'/') {
                text[i] = L'\\';
            }
        }
        return text;
    }

    std::wstring TrimTrailingBackslash(std::wstring text)
    {
        while (text.size() > 3 && (text.back() == L'\\' || text.back() == L'/')) {
            text.pop_back();
        }
        return text;
    }

    bool IsDriveLetter(wchar_t ch)
    {
        return (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z');
    }

    std::wstring NormalizeRedirectRoot(const UIString& input)
    {
        std::wstring text = NormalizeSlashes(RemoveQuotes(input.unicode()));
        if (text.empty()) {
            return L"";
        }

        if (text.size() == 1 && IsDriveLetter(text[0])) {
            wchar_t drive = (wchar_t)std::towupper(text[0]);
            return std::wstring(1, drive) + L":\\";
        }

        if (text.size() >= 2 && IsDriveLetter(text[0]) && text[1] == L':') {
            wchar_t drive = (wchar_t)std::towupper(text[0]);
            if (text.size() == 2) {
                return std::wstring(1, drive) + L":\\";
            }
            text[0] = drive;
            if (text[2] != L'\\') {
                return L"";
            }
            return TrimTrailingBackslash(text);
        }

        return L"";
    }

    std::wstring GetDriveRoot(const std::wstring& path)
    {
        if (path.size() >= 3 && IsDriveLetter(path[0]) && path[1] == L':' && path[2] == L'\\') {
            return path.substr(0, 3);
        }
        return L"";
    }

    UIString LastErrorText(DWORD errorCode = ::GetLastError())
    {
        LPWSTR buffer = NULL;
        DWORD length = ::FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            0,
            (LPWSTR)&buffer,
            0,
            NULL);

        std::wstring message;
        if (length > 0 && buffer) {
            message.assign(buffer, length);
            while (!message.empty() && (message.back() == L'\r' || message.back() == L'\n' || std::iswspace(message.back()))) {
                message.pop_back();
            }
            ::LocalFree(buffer);
        }

        if (message.empty()) {
            message = L"错误码 " + std::to_wstring(errorCode);
        }
        return UIString(message);
    }

    bool DirectoryExists(const std::wstring& path)
    {
        DWORD attr = ::GetFileAttributesW(path.c_str());
        return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool IsReparseDirectory(const std::wstring& path)
    {
        DWORD attr = ::GetFileAttributesW(path.c_str());
        return attr != INVALID_FILE_ATTRIBUTES
            && (attr & FILE_ATTRIBUTE_DIRECTORY)
            && (attr & FILE_ATTRIBUTE_REPARSE_POINT);
    }

    bool CreateDirectories(const std::wstring& path, UIString* error)
    {
        if (path.empty() || DirectoryExists(path)) {
            return true;
        }

        int ret = ::SHCreateDirectoryExW(NULL, path.c_str(), NULL);
        if (ret == ERROR_SUCCESS || ret == ERROR_ALREADY_EXISTS || DirectoryExists(path)) {
            return true;
        }

        if (error) {
            *error = UIString(L"创建目录失败: ") + UIString(path) + UIString(L" - ") + LastErrorText((DWORD)ret);
        }
        return false;
    }

    std::wstring ParentPath(const std::wstring& path)
    {
        size_t pos = path.find_last_of(L"\\/");
        if (pos == std::wstring::npos) {
            return L"";
        }
        if (pos == 2 && path.size() >= 3 && path[1] == L':') {
            return path.substr(0, 3);
        }
        return path.substr(0, pos);
    }

    std::wstring JoinPath(const std::wstring& left, const std::wstring& right)
    {
        if (left.empty()) {
            return right;
        }
        if (right.empty()) {
            return left;
        }
        if (left.back() == L'\\' || left.back() == L'/') {
            return left + right;
        }
        return left + L"\\" + right;
    }

    std::wstring NormalizeForCompare(std::wstring path)
    {
        path = NormalizeSlashes(path);
        if (path.rfind(L"\\\\?\\", 0) == 0) {
            path = path.substr(4);
        }
        path = TrimTrailingBackslash(path);
        for (size_t i = 0; i < path.size(); ++i) {
            path[i] = (wchar_t)std::towlower(path[i]);
        }
        return path;
    }

    bool CopyDirectoryRecursive(const std::wstring& source, const std::wstring& target, UIString* error)
    {
        if (!DirectoryExists(source)) {
            return CreateDirectories(target, error);
        }

        if (!CreateDirectories(target, error)) {
            return false;
        }

        WIN32_FIND_DATAW findData = {};
        std::wstring pattern = JoinPath(source, L"*");
        HANDLE findHandle = ::FindFirstFileW(pattern.c_str(), &findData);
        if (findHandle == INVALID_HANDLE_VALUE) {
            if (error) {
                *error = UIString(L"读取目录失败: ") + UIString(source) + UIString(L" - ") + LastErrorText();
            }
            return false;
        }

        bool ok = true;
        do {
            const wchar_t* name = findData.cFileName;
            if (wcscmp(name, L".") == 0 || wcscmp(name, L"..") == 0) {
                continue;
            }

            std::wstring sourceItem = JoinPath(source, name);
            std::wstring targetItem = JoinPath(target, name);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ok = CopyDirectoryRecursive(sourceItem, targetItem, error);
            }
            else {
                if (!::CopyFileW(sourceItem.c_str(), targetItem.c_str(), FALSE)) {
                    if (error) {
                        *error = UIString(L"复制文件失败: ") + UIString(sourceItem) + UIString(L" - ") + LastErrorText();
                    }
                    ok = false;
                }
            }
        } while (ok && ::FindNextFileW(findHandle, &findData));

        ::FindClose(findHandle);
        return ok;
    }

    bool DeleteDirectoryRecursive(const std::wstring& path, UIString* error)
    {
        if (!DirectoryExists(path)) {
            return true;
        }

        if (IsReparseDirectory(path)) {
            if (::RemoveDirectoryW(path.c_str())) {
                return true;
            }
            if (error) {
                *error = UIString(L"删除目录链接失败: ") + UIString(path) + UIString(L" - ") + LastErrorText();
            }
            return false;
        }

        WIN32_FIND_DATAW findData = {};
        std::wstring pattern = JoinPath(path, L"*");
        HANDLE findHandle = ::FindFirstFileW(pattern.c_str(), &findData);
        if (findHandle == INVALID_HANDLE_VALUE) {
            if (error) {
                *error = UIString(L"读取目录失败: ") + UIString(path) + UIString(L" - ") + LastErrorText();
            }
            return false;
        }

        bool ok = true;
        do {
            const wchar_t* name = findData.cFileName;
            if (wcscmp(name, L".") == 0 || wcscmp(name, L"..") == 0) {
                continue;
            }

            std::wstring item = JoinPath(path, name);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ok = DeleteDirectoryRecursive(item, error);
            }
            else {
                ::SetFileAttributesW(item.c_str(), findData.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
                if (!::DeleteFileW(item.c_str())) {
                    if (error) {
                        *error = UIString(L"删除文件失败: ") + UIString(item) + UIString(L" - ") + LastErrorText();
                    }
                    ok = false;
                }
            }
        } while (ok && ::FindNextFileW(findHandle, &findData));

        ::FindClose(findHandle);
        if (!ok) {
            return false;
        }

        if (::RemoveDirectoryW(path.c_str())) {
            return true;
        }

        if (error) {
            *error = UIString(L"删除目录失败: ") + UIString(path) + UIString(L" - ") + LastErrorText();
        }
        return false;
    }

    bool IsDirectoryRedirected(const std::wstring& directory, std::wstring* finalPath)
    {
        HANDLE file = ::CreateFileW(
            directory.c_str(),
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);

        if (file == INVALID_HANDLE_VALUE) {
            return false;
        }

        std::vector<wchar_t> buffer(32768);
        DWORD length = ::GetFinalPathNameByHandleW(file, buffer.data(), (DWORD)buffer.size(), FILE_NAME_NORMALIZED);
        ::CloseHandle(file);

        if (length == 0 || length >= buffer.size()) {
            return false;
        }

        std::wstring realPath(buffer.data(), length);
        if (realPath.rfind(L"\\\\?\\", 0) == 0) {
            realPath = realPath.substr(4);
        }
        if (finalPath) {
            *finalPath = realPath;
        }
        return NormalizeForCompare(realPath) != NormalizeForCompare(directory);
    }

    std::wstring QuoteArg(const std::wstring& text)
    {
        std::wstring quoted = L"\"";
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == L'"') {
                quoted += L'\\';
            }
            quoted += text[i];
        }
        quoted += L"\"";
        return quoted;
    }

    bool RunProcessWait(const std::wstring& file, const std::wstring& args, bool elevate, DWORD* exitCode, UIString* error)
    {
        SHELLEXECUTEINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        info.lpVerb = elevate ? L"runas" : NULL;
        info.lpFile = file.c_str();
        info.lpParameters = args.empty() ? NULL : args.c_str();
        info.nShow = SW_HIDE;

        if (!::ShellExecuteExW(&info)) {
            DWORD code = ::GetLastError();
            if (error) {
                if (code == ERROR_CANCELLED) {
                    *error = L"操作已取消，或没有授予管理员权限。";
                }
                else {
                    *error = LastErrorText(code);
                }
            }
            return false;
        }

        if (info.hProcess) {
            ::WaitForSingleObject(info.hProcess, INFINITE);
            DWORD code = 0;
            if (::GetExitCodeProcess(info.hProcess, &code)) {
                if (exitCode) {
                    *exitCode = code;
                }
            }
            ::CloseHandle(info.hProcess);
            return code == 0;
        }

        if (exitCode) {
            *exitCode = 0;
        }
        return true;
    }

    bool SetRegistryDword(HKEY root, const wchar_t* keyPath, const wchar_t* valueName, DWORD value, UIString* error)
    {
        HKEY key = NULL;
        LONG ret = ::RegCreateKeyExW(root, keyPath, 0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL);
        if (ret != ERROR_SUCCESS) {
            if (error) {
                *error = LastErrorText((DWORD)ret);
            }
            return false;
        }

        ret = ::RegSetValueExW(key, valueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        ::RegCloseKey(key);

        if (ret != ERROR_SUCCESS) {
            if (error) {
                *error = LastErrorText((DWORD)ret);
            }
            return false;
        }
        return true;
    }

    bool SetRegistryDefaultString(HKEY root, const wchar_t* keyPath, const std::wstring& value, UIString* error)
    {
        HKEY key = NULL;
        LONG ret = ::RegCreateKeyExW(root, keyPath, 0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL);
        if (ret != ERROR_SUCCESS) {
            if (error) {
                *error = LastErrorText((DWORD)ret);
            }
            return false;
        }

        ret = ::RegSetValueExW(
            key,
            NULL,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(value.c_str()),
            (DWORD)((value.size() + 1) * sizeof(wchar_t)));
        ::RegCloseKey(key);

        if (ret != ERROR_SUCCESS) {
            if (error) {
                *error = LastErrorText((DWORD)ret);
            }
            return false;
        }
        return true;
    }

    void NotifyExplorerSettingsChanged()
    {
        ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
        DWORD_PTR result = 0;
        ::SendMessageTimeoutW(
            HWND_BROADCAST,
            WM_SETTINGCHANGE,
            0,
            reinterpret_cast<LPARAM>(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"),
            SMTO_ABORTIFHUNG,
            1500,
            &result);
    }

    std::wstring GetEnvString(const wchar_t* name)
    {
        DWORD count = ::GetEnvironmentVariableW(name, NULL, 0);
        if (count == 0) {
            return L"";
        }

        std::wstring value(count, L'\0');
        DWORD written = ::GetEnvironmentVariableW(name, &value[0], count);
        if (written == 0) {
            return L"";
        }
        value.resize(written);
        return value;
    }

    std::wstring BuildMirrorTarget(const std::wstring& redirectRoot, const std::wstring& source)
    {
        if (source.size() >= 3 && IsDriveLetter(source[0]) && source[1] == L':' && source[2] == L'\\') {
            return JoinPath(redirectRoot, source.substr(3));
        }
        return JoinPath(redirectRoot, source);
    }

    bool CreateJunction(const std::wstring& linkPath, const std::wstring& targetPath, UIString* error)
    {
        DWORD exitCode = 1;
        std::wstring args = L"/c mklink /J " + QuoteArg(linkPath) + L" " + QuoteArg(targetPath);
        if (!RunProcessWait(L"cmd.exe", args, false, &exitCode, error)) {
            if (error && error->empty()) {
                *error = UIString(L"mklink 失败，退出码 ") + ToString((int)exitCode);
            }
            return false;
        }
        return true;
    }

    bool RedirectOneDirectory(const std::wstring& source, const std::wstring& target, UIString* detail)
    {
        if (NormalizeForCompare(source) == NormalizeForCompare(target)) {
            if (detail) {
                *detail = L"源目录和目标目录相同。";
            }
            return false;
        }

        std::wstring existingTarget;
        if (IsDirectoryRedirected(source, &existingTarget)) {
            if (NormalizeForCompare(existingTarget) == NormalizeForCompare(target)) {
                return true;
            }
            if (IsReparseDirectory(source)) {
                if (!::RemoveDirectoryW(source.c_str())) {
                    if (detail) {
                        *detail = UIString(L"移除旧目录链接失败: ") + LastErrorText();
                    }
                    return false;
                }
            }
            else {
                if (detail) {
                    *detail = UIString(L"目录已重定向到其他位置: ") + UIString(existingTarget);
                }
                return false;
            }
        }

        UIString error;
        if (!CreateDirectories(ParentPath(target), &error)) {
            if (detail) {
                *detail = error;
            }
            return false;
        }

        if (!DirectoryExists(source) && !CreateDirectories(source, &error)) {
            if (detail) {
                *detail = error;
            }
            return false;
        }

        if (!CopyDirectoryRecursive(source, target, &error)) {
            if (detail) {
                *detail = error;
            }
            return false;
        }

        if (!DeleteDirectoryRecursive(source, &error)) {
            if (detail) {
                *detail = error;
            }
            return false;
        }

        if (!CreateJunction(source, target, &error)) {
            UIString restoreError;
            CreateDirectories(source, &restoreError);
            CopyDirectoryRecursive(target, source, &restoreError);
            if (detail) {
                *detail = error.empty() ? restoreError : error;
            }
            return false;
        }

        return true;
    }

    StepResult OptimizeGameBar()
    {
        UIString error;
        bool ok = SetRegistryDword(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\GameDVR",
            L"AppCaptureEnabled",
            0,
            &error);

        if (ok) {
            ok = SetRegistryDword(
                HKEY_CURRENT_USER,
                L"System\\GameConfigStore",
                L"GameDVR_Enabled",
                0,
                &error);
        }

        return StepResult(ok, L"关闭 Game Bar", ok ? L"GameDVR 捕获开关已关闭。" : error);
    }

    StepResult ShowThisPc()
    {
        UIString error;
        bool ok = SetRegistryDword(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel",
            L"{20D04FE0-3AEA-1069-A2D8-08002B30309D}",
            0,
            &error);

        if (ok) {
            NotifyExplorerSettingsChanged();
        }

        return StepResult(ok, L"显示我的电脑", ok ? L"桌面图标设置已刷新。" : error);
    }

    StepResult DisableHibernate()
    {
        UIString error;
        DWORD exitCode = 1;
        bool ok = RunProcessWait(L"powercfg.exe", L"-h off", true, &exitCode, &error);
        if (!ok && error.empty()) {
            error = UIString(L"powercfg 失败，退出码 ") + ToString((int)exitCode);
        }
        return StepResult(ok, L"关闭系统休眠", ok ? L"已执行 powercfg -h off。" : error);
    }

    StepResult EnableClassicContextMenu()
    {
        UIString error;
        bool ok = SetRegistryDefaultString(
            HKEY_CURRENT_USER,
            L"Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}\\InprocServer32",
            L"",
            &error);

        if (ok) {
            NotifyExplorerSettingsChanged();
        }

        return StepResult(ok, L"展开 Win11 右键菜单", ok ? L"已写入经典右键菜单注册表项，重启资源管理器后完全生效。" : error);
    }

    StepResult RedirectCache(const UIString& input)
    {
        std::wstring redirectRoot = NormalizeRedirectRoot(input);
        if (redirectRoot.empty()) {
            return StepResult(false, L"重定向 C 盘缓存", L"请输入目标盘符或路径，例如 D 或 D:\\Cache。");
        }

        std::wstring driveRoot = GetDriveRoot(redirectRoot);
        if (driveRoot.empty()) {
            return StepResult(false, L"重定向 C 盘缓存", L"目标路径必须使用盘符开头。");
        }

        if (driveRoot[0] == L'C' || driveRoot[0] == L'c') {
            return StepResult(false, L"重定向 C 盘缓存", L"目标不能是 C 盘。");
        }

        UINT driveType = ::GetDriveTypeW(driveRoot.c_str());
        if (driveType == DRIVE_NO_ROOT_DIR || driveType == DRIVE_UNKNOWN) {
            return StepResult(false, L"重定向 C 盘缓存", UIString(L"目标盘不存在: ") + UIString(driveRoot));
        }

        UIString error;
        if (!CreateDirectories(redirectRoot, &error)) {
            return StepResult(false, L"重定向 C 盘缓存", error);
        }

        std::wstring profile = TrimTrailingBackslash(NormalizeSlashes(GetEnvString(L"USERPROFILE")));
        if (profile.empty()) {
            return StepResult(false, L"重定向 C 盘缓存", L"无法读取 USERPROFILE。");
        }

        std::vector<std::wstring> sources;
        sources.push_back(JoinPath(profile, L"Documents"));
        sources.push_back(JoinPath(profile, L"Downloads"));
        sources.push_back(JoinPath(profile, L"Desktop"));
        sources.push_back(JoinPath(profile, L"Music"));
        sources.push_back(JoinPath(profile, L"Videos"));
        sources.push_back(JoinPath(profile, L"Pictures"));
        sources.push_back(JoinPath(profile, L"Saved Games"));
        sources.push_back(JoinPath(profile, L"source"));
        sources.push_back(JoinPath(profile, L"AppData\\Local\\Temp"));

        int successCount = 0;
        UIString firstError;
        for (size_t i = 0; i < sources.size(); ++i) {
            std::wstring target = BuildMirrorTarget(redirectRoot, sources[i]);
            UIString detail;
            if (RedirectOneDirectory(sources[i], target, &detail)) {
                ++successCount;
            }
            else if (firstError.empty()) {
                firstError = detail.empty()
                    ? UIString(L"处理失败: ") + UIString(sources[i])
                    : detail;
            }
        }

        if (successCount == (int)sources.size()) {
        return StepResult(true, L"重定向 C 盘缓存", UIString(L"已重定向 ") + ToString(successCount) + UIString(L" 个用户目录。"));
        }

        return StepResult(
            false,
            L"重定向 C 盘缓存",
            UIString(L"完成 ") + ToString(successCount) + UIString("/") + ToString((int)sources.size()) + UIString(L" 个目录。 ") + firstError);
    }

    StepResult ExecuteOptimization(MainForm::OptimizationId id, const UIString& redirectTarget)
    {
        switch (id) {
        case MainForm::OptimizationId::GameBar:
            return OptimizeGameBar();
        case MainForm::OptimizationId::ThisPc:
            return ShowThisPc();
        case MainForm::OptimizationId::Hibernate:
            return DisableHibernate();
        case MainForm::OptimizationId::RedirectCache:
            return RedirectCache(redirectTarget);
        case MainForm::OptimizationId::ClassicContextMenu:
            return EnableClassicContextMenu();
        default:
            return StepResult(false, L"未知项目", L"未识别的优化项。");
        }
    }
}

MainForm::MainForm()
    : BorderlessWindow(),
    applyButton_(NULL),
    redirectInput_(NULL),
    logBox_(NULL),
    summaryLabel_(NULL),
    statusPill_(NULL),
    worker_(),
    busy_(false)
{
    items_.push_back({ OptimizationId::GameBar, "ckGameBar", L"关闭 Game Bar" });
    items_.push_back({ OptimizationId::ThisPc, "ckThisPc", L"显示我的电脑" });
    items_.push_back({ OptimizationId::Hibernate, "ckHibernate", L"关闭系统休眠" });
    items_.push_back({ OptimizationId::RedirectCache, "ckRedirect", L"重定向 C 盘缓存" });
    items_.push_back({ OptimizationId::ClassicContextMenu, "ckContextMenu", L"展开 Win11 右键菜单" });

    SetTitle(L"System Optimizer");
    SetResizable(false);
    EnableAlphaBlending();
    LoadXml("res/mainForm.htm");
    SetFixedSize(620, 640);

    BindControls();
    if (auto* check = FindControl<CheckBox>("ckGameBar")) {
        check->SetCheck(true);
    }
    if (auto* check = FindControl<CheckBox>("ckThisPc")) {
        check->SetCheck(true);
    }
    if (auto* check = FindControl<CheckBox>("ckHibernate")) {
        check->SetCheck(true);
    }
    UpdateRedirectInputState();
    UpdateSelectionSummary();
}

MainForm::~MainForm() = default;

void MainForm::BindControls()
{
    applyButton_ = FindControl<Button>("btnApply");
    redirectInput_ = FindControl<TextBox>("editRedirectTarget");
    logBox_ = FindControl<TextBox>("resultLog");
    summaryLabel_ = FindControl<Label>("summaryText");
    statusPill_ = FindControl<Label>("statusPill");

    if (applyButton_) {
        applyButton_->AddEventHandler(Event::MouseClick, [this](Control*, EventArgs* args) {
            if (args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
                ApplySelected();
            }
            });
    }

    if (auto* redirectCheck = FindControl<CheckBox>("ckRedirect")) {
        redirectCheck->CheckedChanged = [this](CheckBox*, bool) {
            UpdateRedirectInputState();
            UpdateSelectionSummary();
            };
    }

    for (const auto& item : items_) {
        if (item.checkBoxName == "ckRedirect") {
            continue;
        }
        if (auto* check = FindControl<CheckBox>(item.checkBoxName)) {
            check->CheckedChanged = [this](CheckBox*, bool) {
                UpdateSelectionSummary();
            };
        }
    }
}

void MainForm::UpdateRedirectInputState()
{
    bool enabled = IsChecked("ckRedirect") && !busy_;
    if (redirectInput_) {
        redirectInput_->SetEnabled(enabled);
        redirectInput_->Invalidate();
    }
}

void MainForm::UpdateSelectionSummary()
{
    size_t count = GetSelectedItems().size();
    SetSummaryText(UIString(ToString((int)count)) + UIString(L" 个项目待执行"));
}

void MainForm::ApplySelected()
{
    if (busy_) {
        return;
    }

    std::vector<OptimizationId> selected = GetSelectedItems();
    if (selected.empty()) {
        SetLog(L"请先勾选至少一个优化项。");
        UpdateSelectionSummary();
        return;
    }

    UIString redirectTarget;
    if (redirectInput_) {
        redirectTarget = redirectInput_->GetText();
    }

    SetBusy(true);
    SetLog(L"正在应用选中的优化项...");
    SetSummaryText(UIString(L"准备执行 ") + ToString((int)selected.size()) + UIString(L" 个项目"));

    auto weakThis = GetWeakPtr<MainForm>();
    worker_.reset(new Thread([weakThis, selected, redirectTarget]() {
        std::vector<StepResult> results;
        for (size_t i = 0; i < selected.size(); ++i) {
            results.push_back(ExecuteOptimization(selected[i], redirectTarget));
        }

        BeginInvoke([weakThis, results]() {
            if (!weakThis.IsAlive()) {
                return;
            }

            int okCount = 0;
            UIString log;
            for (size_t i = 0; i < results.size(); ++i) {
                const StepResult& item = results[i];
                if (item.ok) {
                    ++okCount;
                }
                log += item.ok ? UIString("[OK] ") : UIString("[FAIL] ");
                log += item.title + UIString(" - ") + item.detail;
                if (i + 1 < results.size()) {
                    log += UIString("\r\n");
                }
            }

            weakThis->SetLog(log);
            weakThis->SetSummaryText(UIString(L"完成 ") + ToString(okCount) + UIString("/") + ToString((int)results.size()) + UIString(L" 个项目"));
            weakThis->SetBusy(false);
            weakThis->UpdateSelectionSummary();
            });
        }));
}

void MainForm::SetBusy(bool busy)
{
    busy_ = busy;

    if (applyButton_) {
        applyButton_->SetEnabled(!busy);
        applyButton_->SetText(busy ? L"正在应用" : L"应用优化");
        applyButton_->Invalidate();
    }

    if (statusPill_) {
        statusPill_->SetText(busy ? L"RUNNING" : L"READY");
        statusPill_->Invalidate();
    }

    UpdateRedirectInputState();
}

void MainForm::SetLog(const UIString& text)
{
    if (logBox_) {
        logBox_->SetText(text);
        logBox_->Invalidate();
    }
}

void MainForm::SetSummaryText(const UIString& text)
{
    if (summaryLabel_) {
        summaryLabel_->SetText(text);
        summaryLabel_->Invalidate();
    }
}

bool MainForm::IsChecked(const UIString& checkBoxName) const
{
    CheckBox* checkBox = const_cast<MainForm*>(this)->FindControl<CheckBox>(checkBoxName);
    return checkBox && checkBox->GetCheck();
}

std::vector<MainForm::OptimizationId> MainForm::GetSelectedItems() const
{
    std::vector<OptimizationId> selected;
    for (size_t i = 0; i < items_.size(); ++i) {
        if (IsChecked(items_[i].checkBoxName)) {
            selected.push_back(items_[i].id);
        }
    }
    return selected;
}

void MainForm::OnClose(bool& allowClose)
{
    allowClose = true;
    Application::Exit(0);
}
