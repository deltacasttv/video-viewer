#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "colorbar.hpp"

using namespace Deltacast;

static uint64_t expected_size(int w, int h, ColorBar::PixelFormat pf)
{
    switch (pf)
    {
    case ColorBar::PixelFormat::ycbcr_422_8:
        return static_cast<uint64_t>(w) * h * 2;
    case ColorBar::PixelFormat::ycbcr_422_10_le_msb:
        return static_cast<uint64_t>(w) * h * 8 / 3;
    case ColorBar::PixelFormat::ycbcr_422_10_be:
        return static_cast<uint64_t>(w) * h * 5 / 2;
    case ColorBar::PixelFormat::ycbcr_444_8:
        return static_cast<uint64_t>(w) * h * 3;
    case ColorBar::PixelFormat::rgb_444_8:
        return static_cast<uint64_t>(w) * h * 3;
    case ColorBar::PixelFormat::bgr_444_8_le_msb:
        return static_cast<uint64_t>(w) * h * 4;
    case ColorBar::PixelFormat::bgr_444_8:
        return static_cast<uint64_t>(w) * h * 3;
    default:
        return 0;
    }
}

TEST_CASE("ColorBar datasize matches expected", "[ColorBar]")
{
    const int width = 12;
    const int height = 8;
    const ColorBar::PixelFormat formats[] = {
        ColorBar::PixelFormat::ycbcr_422_8,
        ColorBar::PixelFormat::ycbcr_422_10_le_msb,
        ColorBar::PixelFormat::ycbcr_422_10_be,
        ColorBar::PixelFormat::ycbcr_444_8,
        ColorBar::PixelFormat::rgb_444_8,
        ColorBar::PixelFormat::bgr_444_8_le_msb,
        ColorBar::PixelFormat::bgr_444_8
    };

    for (auto pf : formats)
    {
        ColorBar bar(width, height, pf);
        REQUIRE(bar.get_datasize() == expected_size(width, height, pf));
    }
}
