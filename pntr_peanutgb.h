/**********************************************************************************************
*
*   pntr_peanutgb - Peanut-GB Gameboy emulator for pntr.
*
*   Copyright 2023 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       Peanut-GB https://github.com/deltabeard/peanut-gb
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

#ifndef PNTR_PEANUTGB_API
    #define PNTR_PEANUTGB_API PNTR_API
#endif

struct gb_s;

PNTR_PEANUTGB_API struct gb_s* pntr_load_peanutgb(const char* fileName);

/**
 * Takes ownership of the data.
 *
 * @param data The rom data.
 */
PNTR_PEANUTGB_API struct gb_s* pntr_load_peanutgb_from_memory(const void* data);
PNTR_PEANUTGB_API void pntr_unload_peanutgb(struct gb_s* gb);
PNTR_PEANUTGB_API bool pntr_update_peanutgb(struct gb_s* gb);
PNTR_PEANUTGB_API int pntr_peanutgb_width();
PNTR_PEANUTGB_API int pntr_peanutgb_height();
PNTR_PEANUTGB_API void pntr_draw_peanutgb(pntr_image* dst, struct gb_s* gb, int posX, int posY);
PNTR_PEANUTGB_API void pntr_draw_peanutgb_scaled(pntr_image* dst, struct gb_s* gb, int posX, int posY, float scaleX, float scaleY, float offsetX, float offsetY, pntr_filter filter);
PNTR_PEANUTGB_API void pntr_peanutgb_set_palette(struct gb_s* gb, pntr_color col1, pntr_color col2, pntr_color col3, pntr_color col4);
PNTR_PEANUTGB_API pntr_image* pntr_peanutgb_image(struct gb_s* gb);

#ifdef PNTR_APP_API
PNTR_PEANUTGB_API void pntr_peanutgb_event(struct gb_s* gb, pntr_app_event* event);
#endif

#ifdef __cplusplus
}
#endif

#endif  // PNTR_PEANUTGB_H_

#ifdef PNTR_PEANUTGB_IMPLEMENTATION
#ifndef PNTR_PEANUTGB_IMPLEMENTATION_ONCE
#define PNTR_PEANUTGB_IMPLEMENTATION_ONCE

#ifndef PNTR_PEANUTGB_PEANUT_GB_H
#define PNTR_PEANUTGB_PEANUT_GB_H "Peanut-GB/peanut_gb.h"
#endif
#include PNTR_PEANUTGB_PEANUT_GB_H

#ifdef __cplusplus
extern "C" {
#endif

struct priv_t {
	/* Pointer to allocated memory holding GB file. */
	uint8_t *rom;
    
	/* Pointer to allocated memory holding save file. */
	uint8_t *cart_ram;

	/* Frame buffer */
	pntr_image* fb;
    enum gb_error_e error;
    int tickCounter;

    pntr_color palette[4];
};

PNTR_PEANUTGB_API void pntr_peanutgb_set_palette(struct gb_s* gb, pntr_color col1, pntr_color col2, pntr_color col3, pntr_color col4) {
    if (gb == NULL) {
        return;
    }

	struct priv_t *priv = gb->direct.priv;
    if (priv == NULL) {
        return;
    }

    priv->palette[0] = col1;
    priv->palette[1] = col2;
    priv->palette[2] = col3;
    priv->palette[3] = col4;
}

/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t pntr_peanutgb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
	const struct priv_t * const p = gb->direct.priv;
	return p->rom[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t pntr_peanutgb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
	const struct priv_t * const p = gb->direct.priv;
	return p->cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void pntr_peanutgb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
	const struct priv_t * const p = gb->direct.priv;
	p->cart_ram[addr] = val;
}

/**
 * Ignore all errors.
 */
void pntr_peanutgb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
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
	pntr_unload_memory(priv->cart_ram);
	pntr_unload_memory(priv->rom);
}

#ifdef ENABLE_LCD
/**
 * Draws scanline into framebuffer.
 */
void pntr_peanutgb_lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160], const uint_least8_t line) {
	struct priv_t *priv = gb->direct.priv;

    int posY = line;
	for(unsigned int x = 0; x < LCD_WIDTH; x++) {
        pntr_color color = priv->palette[pixels[x] & 3];
        pntr_draw_point_unsafe(priv->fb, x, posY, color);
    }
}
#endif

struct gb_s* pntr_load_peanutgb(const char* fileName) {
    unsigned int bytesRead;
    const void* data = pntr_load_file(fileName, &bytesRead);
    return pntr_load_peanutgb_from_memory(data);
}

PNTR_PEANUTGB_API struct gb_s* pntr_load_peanutgb_from_memory(const void* data) {
    if (data == NULL) {
        return NULL;
    }

    struct gb_s* gb = pntr_load_memory(sizeof(struct gb_s));
    if (gb == NULL) {
        pntr_unload_memory((void*)data);
        return NULL;
    }

    struct priv_t* priv = pntr_load_memory(sizeof(struct priv_t));
    if (priv == NULL) {
        pntr_unload_memory((void*)data);
        pntr_unload_peanutgb(gb);
        return NULL;
    }

    priv->rom = (uint8_t*)data;
    priv->error = false;
    priv->tickCounter = 0;
    priv->fb = pntr_new_image(pntr_peanutgb_width(), pntr_peanutgb_height());

    enum gb_init_error_e ret = gb_init(gb,
            &pntr_peanutgb_rom_read,
            &pntr_peanutgb_cart_ram_read,
			&pntr_peanutgb_cart_ram_write,
			&pntr_peanutgb_error,
			priv);

	if(ret != GB_INIT_NO_ERROR) {
        pntr_unload_peanutgb(gb);
		return NULL;
	}

    // https://lospec.com/palette-list/nintendo-gameboy-bgb
    pntr_peanutgb_set_palette(gb,
        pntr_get_color(0xe0f8d0FF),
        pntr_get_color(0x88c070FF),
        pntr_get_color(0x346856FF),
        pntr_get_color(0x081820FF)
    );

    priv->cart_ram = pntr_load_memory(gb_get_save_size(gb));

    #ifdef ENABLE_LCD
    gb_init_lcd(gb, pntr_peanutgb_lcd_draw_line);
    #endif

    return gb;
}

PNTR_PEANUTGB_API void pntr_draw_peanutgb(pntr_image* dst, struct gb_s* gb, int posX, int posY) {
    pntr_draw_image(dst, pntr_peanutgb_image(gb), posX, posY);
}

PNTR_PEANUTGB_API pntr_image* pntr_peanutgb_image(struct gb_s* gb) {
    if (gb == NULL) {
        return NULL;
    }

    struct priv_t* priv = (struct priv_t*)gb->direct.priv;
    return priv->fb;
}

PNTR_PEANUTGB_API void pntr_draw_peanutgb_scaled(pntr_image* dst, struct gb_s* gb, int posX, int posY, float scaleX, float scaleY, float offsetX, float offsetY, pntr_filter filter) {
    pntr_draw_image_scaled(dst, pntr_peanutgb_image(gb), posX, posY, scaleX, scaleY, offsetX, offsetY, filter);
}

PNTR_PEANUTGB_API bool pntr_update_peanutgb(struct gb_s* gb) {
    if (gb == NULL) {
        return false;
    }

    /* Execute CPU cycles until the screen has to be redrawn. */
    struct priv_t* priv = gb->direct.priv;

    gb_run_frame(gb);

    // RTC Timer
    if (priv->tickCounter++ > 60) {
        gb_tick_rtc(gb);
    }

    if (priv->error > 0) {
        return false;
    }

    return true;
}

PNTR_PEANUTGB_API void pntr_unload_peanutgb(struct gb_s* gb) {
    if (gb == NULL) {
        return;
    }

    struct priv_t* priv = gb->direct.priv;
    if (priv != NULL) {
        pntr_unload_memory(priv->cart_ram);
        pntr_unload_memory(priv->rom);
        pntr_unload_image(priv->fb);
        pntr_unload_memory(priv);
    }

    pntr_unload_memory(gb);
    gb = NULL;
}

#ifdef PNTR_APP_API
PNTR_PEANUTGB_API void pntr_peanutgb_event(struct gb_s* gb, pntr_app_event* event) {
    if (gb == NULL || event == NULL) {
        return;
    }

    switch (event->type) {
        case PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN:
        case PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_UP:
            switch(event->gamepadButton) {
                case PNTR_APP_GAMEPAD_BUTTON_A:
                    gb->direct.joypad_bits.a = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_B:
                    gb->direct.joypad_bits.b = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_SELECT:
                    gb->direct.joypad_bits.select = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_START:
                    gb->direct.joypad_bits.start = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_UP:
                    gb->direct.joypad_bits.up = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_RIGHT:
                    gb->direct.joypad_bits.right = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_DOWN:
                    gb->direct.joypad_bits.down = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_LEFT:
                    gb->direct.joypad_bits.left = event->type != PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN;
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_MENU:
                    gb_reset(gb);
                    break;
            }
        break;

        case PNTR_APP_EVENTTYPE_KEY_DOWN:
        case PNTR_APP_EVENTTYPE_KEY_UP:
            switch(event->key) {
                case PNTR_APP_KEY_Z:
                    gb->direct.joypad_bits.a = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_X:
                    gb->direct.joypad_bits.b = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_RIGHT_SHIFT:
                case PNTR_APP_KEY_BACKSPACE:
                    gb->direct.joypad_bits.select = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_ENTER:
                    gb->direct.joypad_bits.start = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_UP:
                    gb->direct.joypad_bits.up = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_RIGHT:
                    gb->direct.joypad_bits.right = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_DOWN:
                    gb->direct.joypad_bits.down = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_LEFT:
                    gb->direct.joypad_bits.left = event->type != PNTR_APP_EVENTTYPE_KEY_DOWN;
                    break;
                case PNTR_APP_KEY_R:
                    if (event->type == PNTR_APP_EVENTTYPE_KEY_UP) {
                        gb_reset(gb);
                    }
                    break;
            }
        break;
    }
}
#endif

PNTR_PEANUTGB_API int pntr_peanutgb_width() {
    return LCD_WIDTH;
}

PNTR_PEANUTGB_API int pntr_peanutgb_height() {
    return LCD_HEIGHT;
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_PEANUTGB_IMPLEMENTATION_ONCE
#endif  // PNTR_PEANUTGB_IMPLEMENTATION
