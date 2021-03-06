#pragma once

class CMainDialog
{
public:

    CMainDialog(HINSTANCE hInst);
    ~CMainDialog();

    BOOL InitInstance(int nCmdShow);

private:

    HINSTANCE hInst_;

    // Stat display format strings (loaded from string table)
    wchar_t szParagraphCountsFmt1_[64];
    wchar_t szParagraphCountsFmt2_[64];
    wchar_t szParagraphCountsFmt3_[64];
    wchar_t szLettersFreqTextFmt1_[64];
    wchar_t szLettersFreqTextFmt2_[64];
    wchar_t szLettersFreqFmt_[64];

    BOOL ParseInputAndDisplayStats(HWND hWnd);

    static BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
