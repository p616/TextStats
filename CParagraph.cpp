#include "stdafx.h"
#include "CParagraph.h"


CParagraph::CParagraph(wchar_t *input) : CTextBlock<std::wstring>(input)
{
    letters_[0] = 0xFFFF;
    for (int i = 1; i < 26; ++i)
    {
        letters_[i] = 0;
    }
}

CParagraph::~CParagraph()
{
}

// spaces and punctuation break up words in a paragraph
const wchar_t *
CParagraph::SEGMENT_SEPARATORS()
{
    return L" .,?!;:";
}

// lowest possible segment unit : wchar_t = level 1
// CParagraph segments : words = level 2
const Levels
CParagraph::SEGMENT_LEVEL()
{ 
    return Levels::WORD; 
}

// TODO: Hard-coded array of size 26 not extensible to other alphabets:
// Instead could use std::map<wchar,int> to store and count instances of 
// letters and std::iswalpha to determine which chars are letters...
unsigned
(&CParagraph::GetLetters())[26]
{
    if (!parsed)
    {
        Parse();
    }

    if (letters_[0] == 0xFFFF)
    {
        letters_[0] = 0;

        for (std::wstring & word : txtSegments)
        {
            for (wchar_t letter : word)
            {
                int index = ((char)std::towupper(letter) - 'A');
                if (index < 26 && index >= 0)
                {
                    letters_[index]++;
                }
                // else char is not a letter...
            }
        }
    }

    return letters_;
}

