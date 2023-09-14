/**********************************************************************************************
*
*   pntr_peanutgb - Peanut-GB for pntr.
*
*   Copyright 2023 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       pntr_peanutgb https://github.com/deltabeard/peanut-gb
*
*   LICENSE: zlib/libpng
*
*   pntr_peanutgb is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef PNTR_PEANUTGB_H_
#define PNTR_PEANUTGB_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define ENABLE_LCD 1
#ifndef PNTR_PEANUTGB_PEANUT_GB_H
#define PNTR_PEANUTGB_PEANUT_GB_H "Peanut-GB/peanut_gb.h"
#endif

#ifndef PNTR_PEANUTGB_INCLUDED
#define PNTR_PEANUTGB_INCLUDED
#include PNTR_PEANUTGB_PEANUT_GB_H
#endif

#ifndef PNTR_PEANUTGB_API
    #define PNTR_PEANUTGB_API PNTR_API
#endif

PNTR_PEANUTGB_API void* pntr_load_peanutgb(const void* data);
PNTR_PEANUTGB_API void pntr_unload_peanutgb(struct gb_s* gb);
PNTR_PEANUTGB_API bool pntr_update_peanutgb(struct gb_s* gb, pntr_image* dst, int posX, int posY);

#ifdef __cplusplus
}
#endif

#endif  // PNTR_PEANUTGB_H_

#ifdef PNTR_PEANUTGB_IMPLEMENTATION
#ifndef PNTR_PEANUTGB_IMPLEMENTATION_ONCE
#define PNTR_PEANUTGB_IMPLEMENTATION_ONCE

#ifdef __cplusplus
extern "C" {
#endif

struct priv_t
{
	/* Pointer to allocated memory holding GB file. */
	uint8_t *rom;
	/* Pointer to allocated memory holding save file. */
	uint8_t *cart_ram;

	/* Frame buffer */
	pntr_image* fb;
    int posX;
    int posY;
    enum gb_error_e error;

    pntr_color palette[4];
};

/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t pntr_peanutgb_rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
	const struct priv_t * const p = gb->direct.priv;
	return p->rom[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t pntr_peanutgb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
	const struct priv_t * const p = gb->direct.priv;
	return p->cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void pntr_peanutgb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr,
		       const uint8_t val)
{
	const struct priv_t * const p = gb->direct.priv;
	p->cart_ram[addr] = val;
}

/**
 * Ignore all errors.
 */
void pntr_peanutgb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val)
{
	const char* gb_err_str[GB_INVALID_MAX] = {
		"UNKNOWN",
		"INVALID OPCODE",
		"INVALID READ",
		"INVALID WRITE",
		"HALT FOREVER"
	};
	struct priv_t *priv = gb->direct.priv;
    priv->error = gb_err;

	fprintf(stderr, "Error %d occurred: %s\n. Exiting.\n",
			gb_err, gb_err_str[gb_err]);

	/* Free memory and then exit. */
	free(priv->cart_ram);
	free(priv->rom);
}

#ifdef ENABLE_LCD
/**
 * Draws scanline into framebuffer.
 */
void pntr_peanutgb_lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160],
		   const uint_least8_t line)
{
	struct priv_t *priv = gb->direct.priv;

    int posY = priv->posY + line;
	for(unsigned int x = 0; x < LCD_WIDTH; x++) {
        int posX = priv->posX + x;

        pntr_color color = priv->palette[pixels[x] & 3];
        pntr_draw_point(priv->fb, posX, posY, color);
    }
}
#endif

#include <stdio.h> // TODO: REmove this
PNTR_PEANUTGB_API void* pntr_load_peanutgb(const void* data) {
    if (data == NULL) {
        return NULL;
    }

    struct gb_s* gb = pntr_load_memory(sizeof(struct gb_s));
    if (gb == NULL) {
        return NULL;
    }

    struct priv_t* priv = pntr_load_memory(sizeof(struct priv_t));
    if (priv == NULL) {
        pntr_unload_peanutgb(gb);
        return NULL;
    }

    priv->rom = (uint8_t*)data;
    priv->error = false;
    
    priv->palette[0] = pntr_get_color(0xe0f8d0FF);
    priv->palette[1] = pntr_get_color(0x88c070FF);
    priv->palette[2] = pntr_get_color(0x346856FF);
    priv->palette[3] = pntr_get_color(0x081820FF);

    enum gb_init_error_e ret = gb_init(gb,
            &pntr_peanutgb_rom_read,
            &pntr_peanutgb_cart_ram_read,
			&pntr_peanutgb_cart_ram_write,
			&pntr_peanutgb_error,
			priv);

	if(ret != GB_INIT_NO_ERROR)
	{
        pntr_unload_peanutgb(gb);
		return NULL;
	}

    priv->cart_ram = malloc(gb_get_save_size(gb));

    #ifdef ENABLE_LCD
    gb_init_lcd(gb, pntr_peanutgb_lcd_draw_line);
    #endif

    return gb;
}

PNTR_PEANUTGB_API bool pntr_update_peanutgb(struct gb_s* gb, pntr_image* dst, int posX, int posY) {
    if (gb == NULL) {
        return false;
    }

    /* Execute CPU cycles until the screen has to be redrawn. */

    struct priv_t* priv = gb->direct.priv;
    priv->fb = dst;
    priv->posX = posX;
    priv->posY = posY;


    gb_run_frame(gb);

    //gb_tick_rtc(gb);

    if (priv->error > 0) {
        return false;
    }

    return !priv->error;
}

PNTR_PEANUTGB_API void pntr_unload_peanutgb(struct gb_s* gb) {
    if (gb == NULL) {
        return;
    }

    struct priv_t* priv = gb->direct.priv;
    if (priv != NULL) {
        pntr_unload_memory(priv->cart_ram);
        pntr_unload_memory(priv->rom);
        pntr_unload_memory(priv);
    }

    pntr_unload_memory(gb);
    gb = NULL;
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_PEANUTGB_IMPLEMENTATION_ONCE
#endif  // PNTR_PEANUTGB_IMPLEMENTATION
