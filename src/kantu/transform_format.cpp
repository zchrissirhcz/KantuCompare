#include "kantu/transform_format.hpp"
#include "kantu/log.hpp"
#include <fmt/core.h>

using namespace Shadow;

void kantu::chw_to_hwc(const cv::Mat& src, cv::Mat& dst)
{
    if (src.depth() != CV_8U)
    {
        LOG(ERROR) << "error: src's depth() should be CV_8U\n";
        return;
    }
    if (src.channels() != 3)
    {
        LOG(ERROR) << "error: currently only support 3 channel\n";
        return;
    }
    dst.create(src.size(), src.type());

    int width = src.cols;
    int height = src.rows;
    const int channels = src.channels();
    uint8_t* rgb = dst.data;

    const int chw_pitch = width;

    const uint8_t* chw_plane0 = src.data;
    const uint8_t* chw_plane1 = src.data + width * height;
    const uint8_t* chw_plane2 = src.data + width * height * 2;

    const int dstPitch = width * channels;
    for (int i = 0; i < height; i++)
    {
        const uint8_t* pY = chw_plane0 + i * chw_pitch;
        const uint8_t* pU = chw_plane1 + i * chw_pitch;
        const uint8_t* pV = chw_plane2 + i * chw_pitch;

        uint8_t* prgb = rgb + i * dstPitch;
        for (int j = 0; j < width; j++)
        {
            uint8_t y, u, v;
            y = *pY++;
            u = *pU++;
            v = *pV++;
            *prgb++ = y;
            *prgb++ = u;
            *prgb++ = v;
        }
    }

    cv::cvtColor(dst, dst, cv::COLOR_BGR2RGB);
}

void kantu::my_chw_to_hwc(cv::InputArray src, cv::OutputArray dst) {
    const auto& src_size = src.getMat().size;
    const int src_c = src_size[0];
    const int src_h = src_size[1];
    const int src_w = src_size[2];

    auto c_hw = src.getMat().reshape(0, {src_c, src_h * src_w});

    dst.create(src_h, src_w, CV_MAKETYPE(src.depth(), src_c));
    cv::Mat dst_1d = dst.getMat().reshape(src_c, {src_h, src_w});

    cv::transpose(c_hw, dst_1d);
}


// behave same as opencv(exclude carotene)
static void RGBfromYUV_BT601_u8(uint8_t& R, uint8_t& G, uint8_t& B, uint8_t Y, uint8_t U, uint8_t V)
{
    // double dR, dG, dB;
    // double dY = Y;
    // double dU = U;
    // double dV = V;
    // RGBfromYUV_BT601(dR, dG, dB, dY, dU, dV);

    // R = dR;
    // G = dG;
    // B = dB;

    static const int ITUR_BT_601_CY = 1220542;
    static const int ITUR_BT_601_CUB = 2116026;
    static const int ITUR_BT_601_CUG = -409993;
    static const int ITUR_BT_601_CVG = -852492;
    static const int ITUR_BT_601_CVR = 1673527;
    static const int ITUR_BT_601_SHIFT = 20;


    #define CLIP_TO_UCHAR(x) (uint8_t)((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

    int u = U - 128;///
    int v = V - 128;///

    int shift = 1 << (ITUR_BT_601_SHIFT - 1);
    int ruv = shift + ITUR_BT_601_CVR * v;
    int guv = shift + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
    int buv = shift + ITUR_BT_601_CUB * u;


    int y00 = std::max(0, Y-16) * ITUR_BT_601_CY;
    R = CLIP_TO_UCHAR( (y00 + ruv) >> ITUR_BT_601_SHIFT );
    G = CLIP_TO_UCHAR( (y00 + guv) >> ITUR_BT_601_SHIFT );
    B = CLIP_TO_UCHAR( (y00 + buv) >> ITUR_BT_601_SHIFT );

    #undef CLIP_TO_UCHAR
}


void kantu::i444_to_rgb(uint8_t* i444, uint8_t* rgb, int height, int width)
{
    uint8_t* src = i444;
    uint8_t* dst = rgb;
    const int w = width;
    const int h = height;

    LOG(INFO) << fmt::format("!! calling {:s}\n", __FUNCTION__);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            uint8_t y = src[0 * w * h + i * w + j];
            uint8_t u = src[1 * w * h + i * w + j];
            uint8_t v = src[2 * w * h + i * w + j];
            uint8_t r, g, b;
            RGBfromYUV_BT601_u8(r, g, b, y, u, v);
            dst[i * w * 3 + j * 3 + 0] = r;
            dst[i * w * 3 + j * 3 + 1] = g;
            dst[i * w * 3 + j * 3 + 2] = b;
        }
    }
}
