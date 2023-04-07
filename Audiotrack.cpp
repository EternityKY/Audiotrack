#include <Windows.h>
#include <string>
#include <stdio.h>
#include <tchar.h>

int main()
{
    LANGID langID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    HINSTANCE hInstance = LoadLibraryEx(TEXT("kernel32.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE);
    HMODULE hModule = GetModuleHandle(NULL);

    TCHAR szFfmpeg[MAX_PATH];
    if (SearchPath(NULL, TEXT("ffmpeg.exe"), NULL, MAX_PATH, szFfmpeg, NULL) == 0)
    {
        MessageBox(NULL, TEXT("Could not find ffmpeg executable!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return 1;
    }

    // REMOVE AUDIO
    HKEY hKey;
    RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("SystemFileAssociations\\.mp4\\shell\\Remove Audio"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    TCHAR szDefault[MAX_PATH] = TEXT("Remove Audio");
    RegSetValueEx(hKey, NULL, 0, REG_SZ, (const BYTE*)szDefault, (lstrlen(szDefault) + 1) * sizeof(TCHAR));

    HKEY commandKey;
    RegCreateKeyEx(hKey, TEXT("command"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &commandKey, NULL);
    TCHAR szCommand[MAX_PATH];
    _stprintf_s(szCommand, MAX_PATH, TEXT("\"%s\" -i \"%%1\" -an -vcodec copy \"%%1_noaudio.mp4\""), szFfmpeg);
    RegSetValueEx(commandKey, NULL, 0, REG_SZ, (const BYTE*)szCommand, (lstrlen(szCommand) + 1) * sizeof(TCHAR));

    RegCloseKey(commandKey);

    // SEPARATE AUDIO
    RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("SystemFileAssociations\\.mp4\\shell\\Separate Audio"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    TCHAR szDefaultSA[MAX_PATH] = TEXT("Separate Audio");
    RegSetValueEx(hKey, NULL, 0, REG_SZ, (const BYTE*)szDefaultSA, (lstrlen(szDefaultSA) + 1) * sizeof(TCHAR));

    HKEY commandKeySA;
    RegCreateKeyEx(hKey, TEXT("command"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &commandKeySA, NULL);
    TCHAR szCommandSA[MAX_PATH];
    _stprintf_s(szCommandSA, MAX_PATH, TEXT("\"%s\" -i \"%%1\" -vn -acodec libmp3lame \"%%1_audio.mp3\""), szFfmpeg);
    RegSetValueEx(commandKeySA, NULL, 0, REG_SZ, (const BYTE*)szCommandSA, (lstrlen(szCommandSA) + 1) * sizeof(TCHAR));

    RegCloseKey(commandKeySA);
    RegCloseKey(hKey);

    return 0;
}
