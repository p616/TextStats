// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"
#include "CPage.h"
#include "CParagraph.h"


// winMain
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Perform main dialog/window initialization
    CMainDialog mainDlg(hInstance);
    if (!mainDlg.InitInstance(nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEXTSTATS));
    
    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        CMainDialog                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////


//
//   FUNCTION: CMainDialog(HINSTANCE _hInst): constructor
//
CMainDialog::CMainDialog(HINSTANCE hInst) : hInst_(hInst)
{
    // Load string table format strings
    LoadStringW(hInst_, IDS_PARAGRAPH_COUNTS_FMT1, szParagraphCountsFmt1_, 64);
    LoadStringW(hInst_, IDS_PARAGRAPH_COUNTS_FMT2, szParagraphCountsFmt2_, 64);
    LoadStringW(hInst_, IDS_PARAGRAPH_COUNTS_FMT3, szParagraphCountsFmt3_, 64);
    LoadStringW(hInst_, IDS_LETTER_FREQ_TEXT_FMT1, szLettersFreqTextFmt1_, 64);
    LoadStringW(hInst_, IDS_LETTER_FREQ_TEXT_FMT2, szLettersFreqTextFmt2_, 64);
    LoadStringW(hInst_, IDS_LETTER_FREQ_FMT, szLettersFreqFmt_, 64);
}

//
//   FUNCTION: ~CMainDialog(): destructor
//
CMainDialog::~CMainDialog()
{
}

//
//   FUNCTION: CMainDialog::InitInstance(HINSTANCE, int)
//
//   PURPOSE: Creates and shows main (non-modal) dialog window and shows it
//
BOOL CMainDialog::InitInstance(int nCmdShow)
{
    // Create non-modal dialog for main application window
    HWND hWnd = CreateDialogParam(hInst_, MAKEINTRESOURCE(IDD_MAINDIALOG), 0, (DLGPROC)CMainDialog::DlgProc, (LPARAM)this);
    if (!hWnd)
    {
        return FALSE;
    }

    // Set dialog box icon
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst_, MAKEINTRESOURCE(IDI_TEXTSTATS)));
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInst_, MAKEINTRESOURCE(IDI_TEXTSTATS)));
    
    // Show the main dialog box window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: CMainDialog::DlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main dialog window.
//
//  WM_INITDIALOG - dialog initialization
//  WM_COMMAND    - process the dialog button presses
//  WM_DESTROY    - post a quit message and return
//
//
BOOL CALLBACK CMainDialog::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // thia pointer passed in on WM_INITDIALOG in lParam
    static CMainDialog *mainDlg = NULL;

    switch (message)
    {
    case WM_INITDIALOG:

        mainDlg = (CMainDialog *)lParam;
        return TRUE;

    case WM_COMMAND:

        if (LOWORD(wParam) == IDCANCEL)
        {
            PostQuitMessage(0);
        }
        else if (LOWORD(wParam) == IDC_BUTTON_PARSETEXT)
        {
            mainDlg->ParseInputAndDisplayStats(hWnd);
        }
        return TRUE;

    case WM_DESTROY:

        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

//
//  FUNCTION: CMainDialog::ParseInputAndDisplayStats(HWND)
//
//  PURPOSE: Parses input text and displays stats
//
BOOL CMainDialog::ParseInputAndDisplayStats(HWND hWnd)
{
    // TODO: dynamic allocation
    // Get input text
    wchar_t wszInput[TextBlock::MAX_TEXTLEN];
    GetDlgItemText(hWnd, IDC_EDIT_INPUTTEXT, wszInput, sizeof(wszInput)/sizeof(wchar_t));

    // Create page, parse text, and display page stats
    CPage pageBlock(wszInput);
    SetDlgItemText(hWnd, IDC_EDIT_TOTAL_PARAGRAPHS, (LPCWSTR)std::to_wstring(pageBlock.SegmentCount()).data());
    SetDlgItemText(hWnd, IDC_EDIT_TOTAL_WORDS, (LPCWSTR)std::to_wstring(pageBlock.SubSegmentCount(TextBlock::Levels::WORD)).data());
    SetDlgItemText(hWnd, IDC_EDIT_TOTAL_CHARS, (LPCWSTR)std::to_wstring(pageBlock.TotalChars()).data());

    std::wstring strParagraphWordCount;
    std::wstring strParagraphLetterCount;

    // Loop through all paragraphs on page and constuct paragraph stats display text
    unsigned currentParagraph = 0;
    for (CParagraph &paragraph : pageBlock.GetTextSegments())
    {
        currentParagraph++;

        // construct current paragraph's word and char count display text
        wchar_t szWords[128];
        unsigned wordCount = paragraph.SegmentCount();
        unsigned charCount = paragraph.TotalChars();
        swprintf_s(szWords, wordCount == 1 ? 
                                charCount == 1 ? szParagraphCountsFmt3_ : 
                                                 szParagraphCountsFmt2_ :
                                szParagraphCountsFmt1_,
                   currentParagraph, wordCount, charCount);

        // each paragraph's word and char count on new line
        if (!strParagraphWordCount.empty())
        {
            strParagraphWordCount.append(L" \r\n");
        }
        strParagraphWordCount.append(szWords);

        // construct letter count display text
        // skip any letters whose count = 0 (not in current paragraph)
        std::wstring strAllLetters;
        unsigned(&letters)[26] = paragraph.GetLetters();
        unsigned letterCount = 0;
        for (char letter = 'A'; letter <= 'Z'; ++letter)
        {
            unsigned letterFreq = letters[(char)letter - 'A'];
            if (letterFreq > 0)
            {
                wchar_t szLetters[1024];
                swprintf_s(szLetters, szLettersFreqFmt_, letter, letterFreq);

                if (!strAllLetters.empty())
                {
                    strAllLetters.append(L"  ");
                }
                strAllLetters.append(szLetters);
                
                letterCount++;
            }
        }

        wchar_t szLettersFreqText[256];
        swprintf_s(szLettersFreqText, letterCount == 1 ? 
                                            szLettersFreqTextFmt2_ : 
                                            szLettersFreqTextFmt1_, 
                   currentParagraph, strAllLetters.data());

        // each paragraph's letter count on new line
        if (!strParagraphLetterCount.empty())
        {
            strParagraphLetterCount.append(L"\r\n");
        }
        strParagraphLetterCount.append(szLettersFreqText);
    }

    // Display paragraph stats
    SetDlgItemText(hWnd, IDC_EDIT_PARAGRAPH_COUNTS, strParagraphWordCount.data());
    SetDlgItemText(hWnd, IDC_EDIT_PARAGRAPH_LETTERS, strParagraphLetterCount.data());

    return TRUE;
}

