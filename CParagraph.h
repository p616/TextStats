#pragma once

#include "CTextBlock.h"

class CParagraph : public CTextBlock<std::wstring>
{
public:
    CParagraph(wchar_t *input);
    ~CParagraph();

    unsigned(&GetLetters())[26];

protected:
    const wchar_t * SEGMENT_SEPARATORS() override;
    const Levels SEGMENT_LEVEL() override;

private:
    // TODO: change to map<char,int> to handle other alphabets
    unsigned letters_[26];
};



