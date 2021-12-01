#include "stdafx.h"
#include "CPage.h"
#include "CParagraph.h"


CPage::CPage(wchar_t *input) : CTextBlock<CParagraph>(input)
{
}

CPage::~CPage() 
{
}

// carriage return / line feed / new line characters break up paragraphs on a page
const wchar_t *
CPage::SEGMENT_SEPARATORS()
{
    return L"\r\n";
}

// lowest possible segment unit : wchar_t = level 1
// CParagraph segments : words = level 2
// CPage segments : paragraphs = level 3
const Levels
CPage::SEGMENT_LEVEL()
{
    return Levels::PARAGRAPH;
}