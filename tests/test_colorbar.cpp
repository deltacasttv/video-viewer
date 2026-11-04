#define CATCH_CONFIG_MAIN
#include "colorbar.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include <cstring>
#include <vector>

using namespace Deltacast;

static uint64_t expected_size(int w, int h, ColorBar::PixelFormat pf)
{
    switch (pf)
    {
    case ColorBar::PixelFormat::ycbcr_422_8:
        return static_cast<uint64_t>(w) * h * 2;
    case ColorBar::PixelFormat::ycbcr_422_10_le_msb:
        // every line starts on a new group of 6 pixels (16 bytes)
        return (static_cast<uint64_t>(w) + 5) / 6 * 16 * h;
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
    const int                   width = 12;
    const int                   height = 8;
    const ColorBar::PixelFormat formats[] = {
        ColorBar::PixelFormat::ycbcr_422_8,     ColorBar::PixelFormat::ycbcr_422_10_le_msb,
        ColorBar::PixelFormat::ycbcr_422_10_be, ColorBar::PixelFormat::ycbcr_444_8,
        ColorBar::PixelFormat::rgb_444_8,       ColorBar::PixelFormat::bgr_444_8_le_msb,
        ColorBar::PixelFormat::bgr_444_8
    };

    for (auto pf : formats)
    {
        ColorBar bar(width, height, pf);
        REQUIRE(bar.get_datasize() == expected_size(width, height, pf));
    }
}

TEST_CASE("ColorBar ycbcr_422_10_le_msb starts every line on a new 6-pixel group", "[ColorBar]")
{
    struct Size
    {
        int width;
        int height;
    };
    // 1280, 2048 and 4096 are not multiples of 6 pixels (issue #24)
    const Size sizes[] = { { 720, 576 },   { 1280, 720 },  { 1920, 1080 },
                           { 2048, 1536 }, { 3840, 2160 }, { 4096, 2160 } };

    for (auto size : sizes)
    {
        CAPTURE(size.width, size.height);
        const uint64_t line_stride = (static_cast<uint64_t>(size.width) + 5) / 6 * 16;
        ColorBar       bar(size.width, size.height, ColorBar::PixelFormat::ycbcr_422_10_le_msb);
        REQUIRE(bar.get_datasize() == line_stride * size.height);
        if (size.width % 48 == 0)
            REQUIRE(bar.get_datasize() == static_cast<uint64_t>(size.width) * size.height * 8 / 3);

        // vertical bars: every line is identical, no line slides
        const uint8_t* data = bar.get_data();
        REQUIRE(data != nullptr);
        bool lines_identical = true;
        for (int y = 1; y < size.height && lines_identical; y++)
            lines_identical = std::memcmp(data, data + y * line_stride, line_stride) == 0;
        REQUIRE(lines_identical);
    }
}

TEST_CASE("ColorBar ycbcr_422_10_le_msb moving line stays in its line", "[ColorBar]")
{
    const int            width = 1280;
    const int            height = 720;
    const uint64_t       line_stride = (static_cast<uint64_t>(width) + 5) / 6 * 16;
    const int            frame_count = 721;  // line 1
    ColorBar             bar(width, height, ColorBar::PixelFormat::ycbcr_422_10_le_msb);
    std::vector<uint8_t> data(bar.get_data(), bar.get_data() + bar.get_datasize());

    bar.draw_moving_line(data.data(), frame_count);

    const uint8_t* ref = bar.get_data();
    REQUIRE(std::memcmp(data.data(), ref, line_stride) == 0);
    REQUIRE(std::memcmp(data.data() + line_stride, ref + line_stride, line_stride) != 0);
    REQUIRE(std::memcmp(data.data() + 2 * line_stride, ref + 2 * line_stride,
                        bar.get_datasize() - 2 * line_stride) == 0);

    // the moving line is uniform: all its groups are equal
    bool groups_equal = true;
    for (uint64_t offset = 16; offset < line_stride && groups_equal; offset += 16)
        groups_equal = std::memcmp(data.data() + line_stride, data.data() + line_stride + offset,
                                   16) == 0;
    REQUIRE(groups_equal);
}
