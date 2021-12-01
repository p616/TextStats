#include "stdafx.h"
#include "CTextBlock.h"


// explicit instantiation of TallySubLevelSegments for T = std::wstring
// do-nothing => all valid levels handled by SubSegmentCount<std::wstring>
template<>
unsigned
CTextBlock<std::wstring>::TallySubLevelSegments(Levels level)
{
    // assert: this will never actually be called 
    return 0;
}
