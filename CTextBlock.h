#pragma once

namespace TextBlock
{
    // max edit box input text len: 32K
    static const unsigned MAX_TEXTLEN = 32767;

    enum class Levels
    {
        CHAR = 1,
        WORD,
        PARAGRAPH,
        PAGE
    };

    // CTextBlock designed with goal of easy extensibility...
    // Current types: CTextBlock (chars), CParagraph (words), CPage (paragraphs)
    // Possible extensions: CChapter (pages), CBook (chapters), CSeries (books)

    // Abstract Base Template Class
    template <typename T>
    class CTextBlock
    {
    public:
        CTextBlock(wchar_t *input);
        virtual ~CTextBlock();

        // at lowest level of all text block hierarchies is characters
        unsigned TotalChars() { return totalChars; };

        // at highest level in this text block's hierarchy is "segments" (typename T)
        unsigned SegmentCount();
        std::vector<T> & GetTextSegments();

        // returns segment count on either the highest (this block's level) or the lowest (characters)
        // for levels in between highest and lowest, the function delegates to TallySubLevelSegments
        unsigned SubSegmentCount(Levels level);

    protected:

        // TODO: dynamic allocation
        // input text to parse into segments (typename T) 
        wchar_t text[TextBlock::MAX_TEXTLEN];

        // total chars in input text block
        unsigned totalChars;

        // list and count of segments (typename T) in this text block
        unsigned segmentCount;
        std::vector<T> txtSegments;

        // method to parse input text into segments and flag to track parse state
        bool parsed;
        void Parse();

        // returns segment count for level between highest (this block's level) and lowest (characters)
        // ex: for CPage, calling with level = 2 returns word count (count of each paragraph's segments = words)
        unsigned TallySubLevelSegments(Levels level);

        // derived class must specify its segment separators 
        virtual const wchar_t * SEGMENT_SEPARATORS() = 0;

        // derived class must (define and) specify its level
        virtual const Levels SEGMENT_LEVEL() = 0;
    };

    // base class constructor
    template <typename T>
    CTextBlock<T>::CTextBlock(wchar_t *input) : segmentCount(0), parsed(false)
    {
        // TODO: dynamic allocation
        wcscpy_s(text, TextBlock::MAX_TEXTLEN, input);

        totalChars = static_cast<unsigned>(wcslen(text));
    }

    // destructor
    template<typename T>
    CTextBlock<T>::~CTextBlock()
    {
    }

    // property "get"
    template <typename T>
    unsigned
    CTextBlock<T>::SegmentCount()
    {
        // make sure this text block has been parsed
        if (!parsed)
        {
            Parse();
        }
        return segmentCount;
    }

    // property "get"
    template <typename T>
    std::vector<T> &
    CTextBlock<T>::GetTextSegments()
    {
        // make sure this text block has been parsed
        if (!parsed)
        {
            Parse();
        }
        return txtSegments;
    }

    // utility to get count of subsegments (segments on lower levels)
    // ex: called from CPage to get word count
    template<typename T>
    unsigned
    CTextBlock<T>::SubSegmentCount(Levels level)
    {
        // error: can't request levels above this block's level
        if (level > SEGMENT_LEVEL())
        {
            return 0;
        }

        // special case : requesting segment count at level = CHAR (1) is same as asking for TotalChars
        if (level == Levels::CHAR)
        {
            return TotalChars();
        }

        // special case : requesting segment count at this block's level is same as asking for SegmentCount 
        if (level == SEGMENT_LEVEL())
        {
            return SegmentCount();
        }

        // assert: level < segmentLevel && level > 1
        // return count of segments on requested sub-level
        return TallySubLevelSegments(level);
    }

    // Primary template member definition
    template<typename T>
    unsigned
    CTextBlock<T>::TallySubLevelSegments(Levels level)
    {
        // assert: level < segmentLevel && level > 1 (CHAR)
        // for each level less than SegmentCount and greater than totalChars, count the segments in each segment
        // ex: Page (level = 3) called with level = WORD (2) : count words
        // future ex: Chapter (level = 4) called with level = WORD (2) : count words
        //              - segments = pages (level = 3)
        //              - for each segment (page), get its segments (paragraphs) : call CPage.SubSegmentCount(WORD)
        //              - for each segment per page (paragraph), get its segments (words) : call CParagraph.SubSegmentCount(WORD) = SegmentCount()
        unsigned subSegmentCount = 0;
        for (T& segment : txtSegments)
        {
            subSegmentCount += segment.SubSegmentCount(level);
        }
        return subSegmentCount;
    }

    // Partial template specialization for T = std::wstring
    // Avoid Linker Errors: Suppresses implicit instantiation
    // The actual (do-nothing) function is explicitly instantiated in CTextBlock.cpp
    extern template unsigned CTextBlock<std::wstring>::TallySubLevelSegments(Levels level);

    // Parse text
    template <typename T>
    void CTextBlock<T>::Parse()
    {
        wchar_t *token = nullptr;
        wchar_t *next_token = nullptr;

        token = wcstok_s(text, SEGMENT_SEPARATORS(), &next_token);
        while (token != NULL)
        {
            segmentCount++;
            // creates CTextBlock<T>(token)
            txtSegments.push_back(token);
            token = wcstok_s(nullptr, SEGMENT_SEPARATORS(), &next_token);
        }

        parsed = true;
    }
}

using namespace TextBlock;