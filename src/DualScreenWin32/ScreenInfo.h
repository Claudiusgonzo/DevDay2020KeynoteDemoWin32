#pragma once
#include <vector>
#include <tuple>

namespace dual_screen
{
    inline int RectWidth(const RECT& rect) { return rect.right - rect.left; }
    inline int RectHeight(const RECT& rect) { return rect.bottom - rect.top; }

    // Is the 'rect' argument logically before (left of / above) the 'comparedTo' argument?
    inline bool operator<(const RECT& left, const RECT& right)
    {
        return std::tie(left.top, left.left) < std::tie(right.top, right.left);
    }

    // Kind of split between different regions.
    enum class SplitKind
    {
        Unknown,
        None,
        Vertical,
        Horizontal
    };

    // ScreenInfo is a helper class that provides an abstraction over
    // the content rects API.
    struct ScreenInfo
    {
        class Snapshot;

        ScreenInfo();

        SplitKind GetSplitKind() const;
        RECT GetClientRect() const;
        RECT GetWindowRect() const;
        unsigned int GetRectCount() const;
        RECT GetRect(unsigned int index) const;
        int GetIndexForRect(LPRECT rect) const;
        int GetWidestIndex() const;
        int GetTallestIndex() const;

        void SetMinRectSize(int minSize);
        int GetMinRectSize() const;

        int GetBestIndexForHorizontalContent() const;

        // Returns true if layout has materially changed.
        bool Update(HWND hWnd) noexcept;

        Snapshot GetSnapshot() const;
        bool HasConfigurationChanged(const Snapshot& other) const;

        void EmulateScreens(int screens, SplitKind splitKind);
        const bool IsEmulating() const;

        static bool AreMultipleScreensPresent();

        class Snapshot
        {
            friend struct ScreenInfo;

            Snapshot(const std::vector<RECT>& rects, const RECT& clientRect);
            bool IsSameAs(const std::vector<RECT>& other_rects, const RECT& other_clientRect) const;
            bool IsSameAs(const Snapshot& other) const;

            RECT m_clientRect{ 0 };
            std::vector<RECT> m_contentRects{};
        };

    private:

        SplitKind m_splitKind{ SplitKind::None };
        RECT m_clientRect{ 0 };
        RECT m_windowRect{ 0 };
        std::vector<RECT> m_contentRects;

        // Default to "less than 200px is useless for layout" - can be overridden.
        int m_minSizeForRect{ 200 };

        int m_emulatedScreenCount{ -1 };
        bool ComputeEmulatedScreens(const Snapshot& snapshot);
    };
}
