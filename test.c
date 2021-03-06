#include <stdint.h>
#include <string.h>
#include <stdio.h>

static inline int imax(const int a, const int b) {
    return a > b ? a : b;
}

static inline int imin(const int a, const int b) {
    return a < b ? a : b;
}

enum BlockSize {
    BS_128x128,
    BS_128x64,
    BS_64x128,
    BS_64x64,
    BS_64x32,
    BS_64x16,
    BS_32x64,
    BS_32x32,
    BS_32x16,
    BS_32x8,
    BS_16x64,
    BS_16x32,
    BS_16x16,
    BS_16x8,
    BS_16x4,
    BS_8x32,
    BS_8x16,
    BS_8x8,
    BS_8x4,
    BS_4x16,
    BS_4x8,
    BS_4x4,
    N_BS_SIZES,
};

enum WedgeDirectionType {
    WEDGE_HORIZONTAL = 0,
    WEDGE_VERTICAL = 1,
    WEDGE_OBLIQUE27 = 2,
    WEDGE_OBLIQUE63 = 3,
    WEDGE_OBLIQUE117 = 4,
    WEDGE_OBLIQUE153 = 5,
    N_WEDGE_DIRECTIONS
};

typedef struct {
    enum WedgeDirectionType direction;
    int x_offset;
    int y_offset;
} wedge_code_type;


static const wedge_code_type wedge_codebook_16_hgtw[16] = {
    { WEDGE_OBLIQUE27, 4, 4 },  { WEDGE_OBLIQUE63, 4, 4 },
    { WEDGE_OBLIQUE117, 4, 4 }, { WEDGE_OBLIQUE153, 4, 4 },
    { WEDGE_HORIZONTAL, 4, 2 }, { WEDGE_HORIZONTAL, 4, 4 },
    { WEDGE_HORIZONTAL, 4, 6 }, { WEDGE_VERTICAL, 4, 4 },
    { WEDGE_OBLIQUE27, 4, 2 },  { WEDGE_OBLIQUE27, 4, 6 },
    { WEDGE_OBLIQUE153, 4, 2 }, { WEDGE_OBLIQUE153, 4, 6 },
    { WEDGE_OBLIQUE63, 2, 4 },  { WEDGE_OBLIQUE63, 6, 4 },
    { WEDGE_OBLIQUE117, 2, 4 }, { WEDGE_OBLIQUE117, 6, 4 },
};

static const wedge_code_type wedge_codebook_16_hltw[16] = {
    { WEDGE_OBLIQUE27, 4, 4 },  { WEDGE_OBLIQUE63, 4, 4 },
    { WEDGE_OBLIQUE117, 4, 4 }, { WEDGE_OBLIQUE153, 4, 4 },
    { WEDGE_VERTICAL, 2, 4 },   { WEDGE_VERTICAL, 4, 4 },
    { WEDGE_VERTICAL, 6, 4 },   { WEDGE_HORIZONTAL, 4, 4 },
    { WEDGE_OBLIQUE27, 4, 2 },  { WEDGE_OBLIQUE27, 4, 6 },
    { WEDGE_OBLIQUE153, 4, 2 }, { WEDGE_OBLIQUE153, 4, 6 },
    { WEDGE_OBLIQUE63, 2, 4 },  { WEDGE_OBLIQUE63, 6, 4 },
    { WEDGE_OBLIQUE117, 2, 4 }, { WEDGE_OBLIQUE117, 6, 4 },
};

static const wedge_code_type wedge_codebook_16_heqw[16] = {
    { WEDGE_OBLIQUE27, 4, 4 },  { WEDGE_OBLIQUE63, 4, 4 },
    { WEDGE_OBLIQUE117, 4, 4 }, { WEDGE_OBLIQUE153, 4, 4 },
    { WEDGE_HORIZONTAL, 4, 2 }, { WEDGE_HORIZONTAL, 4, 6 },
    { WEDGE_VERTICAL, 2, 4 },   { WEDGE_VERTICAL, 6, 4 },
    { WEDGE_OBLIQUE27, 4, 2 },  { WEDGE_OBLIQUE27, 4, 6 },
    { WEDGE_OBLIQUE153, 4, 2 }, { WEDGE_OBLIQUE153, 4, 6 },
    { WEDGE_OBLIQUE63, 2, 4 },  { WEDGE_OBLIQUE63, 6, 4 },
    { WEDGE_OBLIQUE117, 2, 4 }, { WEDGE_OBLIQUE117, 6, 4 },
};

#define ALIGN(line, align) \
    line __attribute__((aligned(align)))

static uint8_t ALIGN(wedge_masks_444_32x32[2 * 16 * 32 * 32], 32);
static uint8_t ALIGN(wedge_masks_444_32x16[2 * 16 * 32 * 16], 32);
static uint8_t ALIGN(wedge_masks_444_32x8 [2 * 16 * 32 *  8], 32);
static uint8_t ALIGN(wedge_masks_444_16x32[2 * 16 * 16 * 32], 32);
static uint8_t ALIGN(wedge_masks_444_16x16[2 * 16 * 16 * 16], 32);
static uint8_t ALIGN(wedge_masks_444_16x8 [2 * 16 * 16 *  8], 32);
static uint8_t ALIGN(wedge_masks_444_8x32 [2 * 16 *  8 * 32], 32);
static uint8_t ALIGN(wedge_masks_444_8x16 [2 * 16 *  8 * 16], 32);
static uint8_t ALIGN(wedge_masks_444_8x8  [2 * 16 *  8 *  8], 32);

static uint8_t ALIGN(wedge_masks_422_16x32[2 * 16 * 16 * 32], 32);
static uint8_t ALIGN(wedge_masks_422_16x16[2 * 16 * 16 * 16], 32);
static uint8_t ALIGN(wedge_masks_422_16x8 [2 * 16 * 16 *  8], 32);
static uint8_t ALIGN(wedge_masks_422_8x32 [2 * 16 *  8 * 32], 32);
static uint8_t ALIGN(wedge_masks_422_8x16 [2 * 16 *  8 * 16], 32);
static uint8_t ALIGN(wedge_masks_422_8x8  [2 * 16 *  8 *  8], 32);
static uint8_t ALIGN(wedge_masks_422_4x32 [2 * 16 *  4 * 32], 32);
static uint8_t ALIGN(wedge_masks_422_4x16 [2 * 16 *  4 * 16], 32);
static uint8_t ALIGN(wedge_masks_422_4x8  [2 * 16 *  4 *  8], 32);

static uint8_t ALIGN(wedge_masks_420_16x16[2 * 16 * 16 * 16], 32);
static uint8_t ALIGN(wedge_masks_420_16x8 [2 * 16 * 16 *  8], 32);
static uint8_t ALIGN(wedge_masks_420_16x4 [2 * 16 * 16 *  4], 32);
static uint8_t ALIGN(wedge_masks_420_8x16 [2 * 16 *  8 * 16], 32);
static uint8_t ALIGN(wedge_masks_420_8x8  [2 * 16 *  8 *  8], 32);
static uint8_t ALIGN(wedge_masks_420_8x4  [2 * 16 *  8 *  4], 32);
static uint8_t ALIGN(wedge_masks_420_4x16 [2 * 16 *  4 * 16], 32);
static uint8_t ALIGN(wedge_masks_420_4x8  [2 * 16 *  4 *  8], 32);
static uint8_t ALIGN(wedge_masks_420_4x4  [2 * 16 *  4 *  4], 32);

const uint8_t *dav1d_wedge_masks[N_BS_SIZES][3][2][16];

static void insert_border(uint8_t *const dst, const uint8_t *src,
                          const int ctr)
{
    if (ctr > 4) memset(dst, 0, ctr - 4);
    memcpy(dst + imax(ctr, 4) - 4, src + imax(4 - ctr, 0), imin(64 - ctr, 8));
    if (ctr < 64 - 4)
        memset(dst + ctr + 4, 64, 64 - 4 - ctr);
}

static void transpose(uint8_t *const dst, const uint8_t *const src) {
    for (int y = 0, y_off = 0; y < 64; y++, y_off += 64)
        for (int x = 0, x_off = 0; x < 64; x++, x_off += 64)
            dst[x_off + y] = src[y_off + x];
}

static void hflip(uint8_t *const dst, const uint8_t *const src) {
    for (int y = 0, y_off = 0; y < 64; y++, y_off += 64)
        for (int x = 0; x < 64; x++)
            dst[y_off + 64 - 1 - x] = src[y_off + x];
}

static void invert(uint8_t *const dst, const uint8_t *const src,
                   const int w, const int h)
{
    for (int y = 0, y_off = 0; y < h; y++, y_off += w)
        for (int x = 0; x < w; x++)
            dst[y_off + x] = 64 - src[y_off + x];
}

static void copy2d(uint8_t *dst, const uint8_t *src,
                   const int w, const int h, const int x_off, const int y_off)
{
    src += y_off * 64 + x_off;
    for (int y = 0; y < h; y++) {
        memcpy(dst, src, w);
        src += 64;
        dst += w;
    }
}

static void init_chroma(uint8_t *chroma, const uint8_t *luma,
                        const int sign, const int w, const int h, const int ss_ver)
{
    for (int y = 0; y < h; y += 1 + ss_ver) {
        for (int x = 0; x < w; x += 2) {
            int sum = luma[x] + luma[x + 1] + 1;
            if (ss_ver) sum += luma[w + x] + luma[w + x + 1] + 1;
            chroma[x >> 1] = (sum - sign) >> (1 + ss_ver);
        }
        luma += w << ss_ver;
        chroma += w >> 1;
    }
}

static void fill2d_16x2(uint8_t *dst, const int w, const int h,
                        const enum BlockSize bs,
                        const uint8_t (*const master)[64 * 64],
                        const wedge_code_type *const cb,
                        uint8_t *masks_444, uint8_t *masks_422,
                        uint8_t *masks_420, const unsigned signs)
{
    uint8_t *ptr = dst;
    for (int n = 0; n < 16; n++) {
        copy2d(ptr, master[cb[n].direction], w, h,
               32 - (w * cb[n].x_offset >> 3), 32 - (h * cb[n].y_offset >> 3));
        ptr += w * h;
    }
    for (int n = 0, off = 0; n < 16; n++, off += w * h)
        invert(ptr + off, dst + off, w, h);

    const int n_stride_444 = (w * h);
    const int n_stride_422 = n_stride_444 >> 1;
    const int n_stride_420 = n_stride_444 >> 2;
    const int sign_stride_444 = 16 * n_stride_444;
    const int sign_stride_422 = 16 * n_stride_422;
    const int sign_stride_420 = 16 * n_stride_420;
    // assign pointers in externally visible array
    for (int n = 0; n < 16; n++) {
        const int sign = (signs >> n) & 1;
        dav1d_wedge_masks[bs][0][0][n] = &masks_444[ sign * sign_stride_444];
        // not using !sign is intentional here, since 444 does not require
        // any rounding since no chroma subsampling is applied.
        dav1d_wedge_masks[bs][0][1][n] = &masks_444[ sign * sign_stride_444];
        dav1d_wedge_masks[bs][1][0][n] = &masks_422[ sign * sign_stride_422];
        dav1d_wedge_masks[bs][1][1][n] = &masks_422[!sign * sign_stride_422];
        dav1d_wedge_masks[bs][2][0][n] = &masks_420[ sign * sign_stride_420];
        dav1d_wedge_masks[bs][2][1][n] = &masks_420[!sign * sign_stride_420];
        masks_444 += n_stride_444;
        masks_422 += n_stride_422;
        masks_420 += n_stride_420;

        // since the pointers come from inside, we know that
        // violation of the const is OK here. Any other approach
        // means we would have to duplicate the sign correction
        // logic in two places, which isn't very nice, or mark
        // the table faced externally as non-const, which also sucks
        init_chroma((uint8_t *)dav1d_wedge_masks[bs][1][0][n],
                    dav1d_wedge_masks[bs][0][0][n], 0, w, h, 0);
        init_chroma((uint8_t *)dav1d_wedge_masks[bs][1][1][n],
                    dav1d_wedge_masks[bs][0][0][n], 1, w, h, 0);
        init_chroma((uint8_t *)dav1d_wedge_masks[bs][2][0][n],
                    dav1d_wedge_masks[bs][0][0][n], 0, w, h, 1);
        init_chroma((uint8_t *)dav1d_wedge_masks[bs][2][1][n],
                    dav1d_wedge_masks[bs][0][0][n], 1, w, h, 1);
    }
}

void dav1d_init_wedge_masks(void) {
    // This function is guaranteed to be called only once

    enum WedgeMasterLineType {
        WEDGE_MASTER_LINE_ODD,
        WEDGE_MASTER_LINE_EVEN,
        WEDGE_MASTER_LINE_VERT,
        N_WEDGE_MASTER_LINES,
    };
    static const uint8_t wedge_master_border[N_WEDGE_MASTER_LINES][8] = {
        [WEDGE_MASTER_LINE_ODD]  = {  1,  2,  6, 18, 37, 53, 60, 63 },
        [WEDGE_MASTER_LINE_EVEN] = {  1,  4, 11, 27, 46, 58, 62, 63 },
        [WEDGE_MASTER_LINE_VERT] = {  0,  2,  7, 21, 43, 57, 62, 64 },
    };
    uint8_t master[6][64 * 64];

    // create master templates
    for (int y = 0, off = 0; y < 64; y++, off += 64)
        insert_border(&master[WEDGE_VERTICAL][off],
                      wedge_master_border[WEDGE_MASTER_LINE_VERT], 32);
    for (int y = 0, off = 0, ctr = 48; y < 64; y += 2, off += 128, ctr--)
    {
        insert_border(&master[WEDGE_OBLIQUE63][off],
                      wedge_master_border[WEDGE_MASTER_LINE_EVEN], ctr);
        insert_border(&master[WEDGE_OBLIQUE63][off + 64],
                      wedge_master_border[WEDGE_MASTER_LINE_ODD], ctr - 1);
    }

    transpose(master[WEDGE_OBLIQUE27], master[WEDGE_OBLIQUE63]);
    transpose(master[WEDGE_HORIZONTAL], master[WEDGE_VERTICAL]);
    hflip(master[WEDGE_OBLIQUE117], master[WEDGE_OBLIQUE63]);
    hflip(master[WEDGE_OBLIQUE153], master[WEDGE_OBLIQUE27]);

#define fill(w, h, sz_422, sz_420, hvsw, signs) \
    fill2d_16x2((uint8_t *) wedge_masks_444_##w##x##h,  w, h, BS_##w##x##h, \
                master, wedge_codebook_16_##hvsw, wedge_masks_444_##w##x##h, \
                wedge_masks_422_##sz_422, wedge_masks_420_##sz_420, signs)

    fill(32, 32, 16x32, 16x16, heqw, 0x7bfb);
    fill(32, 16, 16x16, 16x8,  hltw, 0x7beb);
    fill(32,  8, 16x8,  16x4,  hltw, 0x6beb);
    fill(16, 32,  8x32,  8x16, hgtw, 0x7beb);
    fill(16, 16,  8x16,  8x8,  heqw, 0x7bfb);
    fill(16,  8,  8x8,   8x4,  hltw, 0x7beb);
    fill( 8, 32,  4x32,  4x16, hgtw, 0x7aeb);
    fill( 8, 16,  4x16,  4x8,  hgtw, 0x7beb);
    fill( 8,  8,  4x8,   4x4,  heqw, 0x7bfb);
#undef fill
}

int main(int argc, const char **argv) {
    printf("running...\n");
    dav1d_init_wedge_masks();
    printf("done.\n");
    return 0;
}