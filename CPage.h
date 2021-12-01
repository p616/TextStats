#pragma once

#include "CTextBlock.h"

class CParagraph;

class CPage : public CTextBlock<CParagraph>
{
public:
    CPage(wchar_t *input);
    ~CPage();

protected:
    const wchar_t * SEGMENT_SEPARATORS() override;
    const Levels SEGMENT_LEVEL() override;
};

