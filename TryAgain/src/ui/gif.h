//
// gif.h
// by Charlie Tangora
//
// A simple C++ header-only library for creating animated GIFs.
// To use, define GIF_IMPLEMENTATION in one of your C++ files.
//
#pragma once
#ifndef gif_h
#define gif_h

#include <vector>
#include <cstdint>
#include <string>
#include <cstdio> // For FILE

struct GifWriter
{
    FILE* f;
    uint8_t* oldImage;
    bool firstFrame;
};

// Creates a GIF file.
bool GifBegin(GifWriter& writer, const char* filename, uint32_t width, uint32_t height, uint32_t delay, int32_t bitDepth = 8, bool dither = false);

// Writes out a new frame to a GIF file.
bool GifWriteFrame(GifWriter& writer, const uint8_t* image, uint32_t width, uint32_t height, uint32_t delay, int bitDepth = 8, bool dither = false);

// Closes a GIF file and frees memory.
bool GifEnd(GifWriter& writer);

#endif // gif_h

#ifdef GIF_IMPLEMENTATION

#include <vector>
#include <cstring> // For memcpy

// Based on jo_gif.cpp https://github.com/joncampbell123/jo_gif
// This is a simple implementation of an LZW-based GIF encoder.

struct GifPalette
{
    int bitDepth;
    uint8_t r[256], g[256], b[256];
};

void GifGetClosestPaletteColor(GifPalette* pPal, int r, int g, int b, int& bestInd, int& bestDiff, int treeRoot)
{
    if (treeRoot > (1 << pPal->bitDepth) - 1) return;

    int ri = pPal->r[treeRoot];
    int gi = pPal->g[treeRoot];
    int bi = pPal->b[treeRoot];
    int diff = (r - ri) * (r - ri) + (g - gi) * (g - gi) + (b - bi) * (b - bi);

    if (diff < bestDiff)
    {
        bestInd = treeRoot;
        bestDiff = diff;
    }

    int comps[3] = { r, g, b };
    int palComps[3] = { ri, gi, bi };
    int sortedComps[3];
    int sortedPalComps[3];

    for (int i = 0; i < 3; ++i)
    {
        int maxV = -1, maxI = -1;
        for (int j = 0; j < 3; ++j)
        {
            if (comps[j] > maxV)
            {
                maxV = comps[j];
                maxI = j;
            }
        }
        sortedComps[i] = maxI;
        comps[maxI] = -1;

        maxV = -1;
        maxI = -1;
        for (int j = 0; j < 3; ++j)
        {
            if (palComps[j] > maxV)
            {
                maxV = palComps[j];
                maxI = j;
            }
        }
        sortedPalComps[i] = maxI;
        palComps[maxI] = -1;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (sortedComps[i] != sortedPalComps[i])
        {
            GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2);
            GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2 + 1);
            return;
        }
    }
}

void GifSwapPixels(uint8_t* image, int pix1, int pix2)
{
    uint8_t r = image[pix1 * 4];
    uint8_t g = image[pix1 * 4 + 1];
    uint8_t b = image[pix1 * 4 + 2];
    image[pix1 * 4] = image[pix2 * 4];
    image[pix1 * 4 + 1] = image[pix2 * 4 + 1];
    image[pix1 * 4 + 2] = image[pix2 * 4 + 2];
    image[pix2 * 4] = r;
    image[pix2 * 4 + 1] = g;
    image[pix2 * 4 + 2] = b;
}

int GifPartition(uint8_t* image, const int left, const int right, const int elt, int pivotIndex)
{
    float pivotValue = image[pivotIndex * 4 + elt];
    GifSwapPixels(image, pivotIndex, right - 1);
    int storeIndex = left;
    for (int ii = left; ii < right - 1; ++ii)
    {
        if (image[ii * 4 + elt] < pivotValue)
        {
            GifSwapPixels(image, ii, storeIndex);
            ++storeIndex;
        }
    }
    GifSwapPixels(image, storeIndex, right - 1);
    return storeIndex;
}

void GifMedianCut(GifPalette* pPal, uint8_t* image, const int left, const int right, int palIndex)
{
    if (left >= right) return;
    if (palIndex > (1 << pPal->bitDepth) - 1) return;

    int minR = 255, maxR = 0;
    int minG = 255, maxG = 0;
    int minB = 255, maxB = 0;
    for (int ii = left; ii < right; ++ii)
    {
        int r = image[ii * 4];
        int g = image[ii * 4 + 1];
        int b = image[ii * 4 + 2];

        if (r < minR) minR = r;
        if (r > maxR) maxR = r;
        if (g < minG) minG = g;
        if (g > maxG) maxG = g;
        if (b < minB) minB = b;
        if (b > maxB) maxB = b;
    }

    int rRange = maxR - minR;
    int gRange = maxG - minG;
    int bRange = maxB - minB;

    int maxRange = rRange;
    int splitElt = 0;
    if (gRange > maxRange) { maxRange = gRange; splitElt = 1; }
    if (bRange > maxRange) { maxRange = bRange; splitElt = 2; }

    int medianIndex = (left + right) / 2;
    GifPartition(image, left, right, splitElt, medianIndex);

    long long r_sum = 0, g_sum = 0, b_sum = 0;
    for (int ii = left; ii < right; ++ii)
    {
        r_sum += image[ii * 4];
        g_sum += image[ii * 4 + 1];
        b_sum += image[ii * 4 + 2];
    }

    if (right - left > 0) {
        pPal->r[palIndex] = r_sum / (right - left);
        pPal->g[palIndex] = g_sum / (right - left);
        pPal->b[palIndex] = b_sum / (right - left);
    }

    GifMedianCut(pPal, image, left, medianIndex, palIndex * 2);
    GifMedianCut(pPal, image, medianIndex + 1, right, palIndex * 2 + 1);
}

void GifThresholdImage(const uint8_t* lastFrame, const uint8_t* nextFrame, uint8_t* outFrame, uint32_t width, uint32_t height, uint32_t threshold)
{
    for (uint32_t ii = 0; ii < width * height; ++ii)
    {
        int r_diff = lastFrame[ii * 4] - nextFrame[ii * 4];
        int g_diff = lastFrame[ii * 4 + 1] - nextFrame[ii * 4 + 1];
        int b_diff = lastFrame[ii * 4 + 2] - nextFrame[ii * 4 + 2];
        if (r_diff * r_diff + g_diff * g_diff + b_diff * b_diff > threshold * threshold)
        {
            outFrame[ii * 4] = nextFrame[ii * 4];
            outFrame[ii * 4 + 1] = nextFrame[ii * 4 + 1];
            outFrame[ii * 4 + 2] = nextFrame[ii * 4 + 2];
            outFrame[ii * 4 + 3] = 255;
        }
        else
        {
            outFrame[ii * 4] = 0;
            outFrame[ii * 4 + 1] = 0;
            outFrame[ii * 4 + 2] = 0;
            outFrame[ii * 4 + 3] = 0;
        }
    }
}

struct GifLzwNode
{
    uint16_t m_next[256];
};

void fput_word(FILE* f, uint16_t w)
{
    fputc(w & 0xff, f);
    fputc((w >> 8) & 0xff, f);
}

void fput_bits(FILE* f, uint32_t bits, uint32_t n_bits)
{
    static uint32_t bit_buffer = 0;
    static uint32_t bit_count = 0;
    bit_buffer |= bits << bit_count;
    bit_count += n_bits;
    while (bit_count >= 8)
    {
        fputc(bit_buffer & 0xff, f);
        bit_buffer >>= 8;
        bit_count -= 8;
    }
}

void GifWriteLzwImage(FILE* f, uint8_t* image, uint32_t left, uint32_t top, uint32_t width, uint32_t height, uint32_t delay, GifPalette* pPal)
{
    (void)delay; (void)pPal;
    fputc(0x2c, f); // image descriptor
    fput_word(f, left);
    fput_word(f, top);
    fput_word(f, width);
    fput_word(f, height);
    fputc(0x00, f); // no local color table

    fputc(0x08, f); // LZW minimum code size
    const int clear_code = 256;
    const int eoi_code = 257;
    int next_code_slot = 258;

    std::vector<GifLzwNode> lzw_table(4096);
    fput_bits(f, clear_code, 9);
    int current_code = -1;
    uint32_t bit_count = 9;
    for (uint32_t yy = 0; yy < height; ++yy)
    {
        for (uint32_t xx = 0; xx < width; ++xx)
        {
            uint8_t next_pixel = image[(yy * width + xx)];
            if (current_code < 0)
            {
                current_code = next_pixel;
            }
            else if (lzw_table[current_code].m_next[next_pixel])
            {
                current_code = lzw_table[current_code].m_next[next_pixel];
            }
            else
            {
                fput_bits(f, current_code, bit_count);
                lzw_table[current_code].m_next[next_pixel] = next_code_slot;

                if (next_code_slot >= (1 << bit_count) && bit_count < 12)
                {
                    ++bit_count;
                }
                current_code = next_pixel;
                next_code_slot++;
                if (next_code_slot > 4095)
                {
                    fput_bits(f, clear_code, bit_count);
                    for (auto& node : lzw_table) {
                        for (int i = 0; i < 256; ++i) node.m_next[i] = 0;
                    }
                    next_code_slot = 258;
                    bit_count = 9;
                }
            }
        }
    }
    fput_bits(f, current_code, bit_count);
    fput_bits(f, eoi_code, bit_count);
    fput_bits(f, 0, 7); // flush

    // block terminator
    fputc(0x01, f);
    fputc(0x00, f);
}

bool GifBegin(GifWriter& writer, const char* filename, uint32_t width, uint32_t height, uint32_t delay, int32_t bitDepth, bool dither)
{
    (void)delay; (void)dither;
    // FIX: Use fopen_s on Windows (_WIN32) and fopen on other platforms
#ifdef _WIN32
    fopen_s(&writer.f, filename, "wb");
#else
    writer.f = fopen(filename, "wb");
#endif
    if (!writer.f) return false;

    writer.oldImage = new uint8_t[width * height * 4];
    writer.firstFrame = true;

    fputs("GIF89a", writer.f);

    fput_word(writer.f, width);
    fput_word(writer.f, height);
    fputc(0xF0 | (bitDepth - 1), writer.f);
    fputc(0, writer.f);
    fputc(0, writer.f);

    return true;
}

bool GifWriteFrame(GifWriter& writer, const uint8_t* image, uint32_t width, uint32_t height, uint32_t delay, int bitDepth, bool dither)
{
    (void)dither;
    if (!writer.f) return false;

    const uint8_t* oldImage = writer.firstFrame ? nullptr : writer.oldImage;
    writer.firstFrame = false;

    GifPalette pal;
    pal.bitDepth = bitDepth;

    uint8_t* tempImage = new uint8_t[width * height * 4];
    memcpy(tempImage, image, width * height * 4);

    if (oldImage)
    {
        uint8_t* diffImage = new uint8_t[width * height * 4];
        GifThresholdImage(oldImage, image, diffImage, width, height, 32);
        GifMedianCut(&pal, diffImage, 0, width * height, 1);
        delete[] diffImage;
    }
    else
    {
        GifMedianCut(&pal, tempImage, 0, width * height, 1);
    }
    delete[] tempImage;

    // global color table
    for (int ii = 0; ii < (1 << bitDepth); ++ii)
    {
        fputc(pal.r[ii], writer.f);
        fputc(pal.g[ii], writer.f);
        fputc(pal.b[ii], writer.f);
    }

    uint8_t* quantizedImage = new uint8_t[width * height];
    for (uint32_t ii = 0; ii < width * height; ++ii)
    {
        int r = image[ii * 4];
        int g = image[ii * 4 + 1];
        int b = image[ii * 4 + 2];
        int bestInd = 1, bestDiff = 1000000;
        GifGetClosestPaletteColor(&pal, r, g, b, bestInd, bestDiff, 1);
        quantizedImage[ii] = bestInd;
    }

    // graphics control extension
    fputc(0x21, writer.f);
    fputc(0xf9, writer.f);
    fputc(4, writer.f);
    fputc(1, writer.f);
    fput_word(writer.f, delay / 10);
    fputc(0, writer.f); // transparent color index
    fputc(0, writer.f);

    GifWriteLzwImage(writer.f, quantizedImage, 0, 0, width, height, delay, &pal);
    delete[] quantizedImage;

    memcpy(writer.oldImage, image, width * height * 4);
    return true;
}

bool GifEnd(GifWriter& writer)
{
    if (!writer.f) return false;

    fputc(0x3b, writer.f);
    fclose(writer.f);
    delete[] writer.oldImage;

    writer.f = nullptr;
    writer.oldImage = nullptr;

    return true;
}

#endif // GIF_IMPLEMENTATION
