#include <stdio.h>

//#define PNTR_ENABLE_DEFAULT_FONT
//#define PNTR_ENABLE_FILTER_SMOOTH
//#define PNTR_ENABLE_TTF
//#define PNTR_ENABLE_MATH
#define PNTR_DISABLE_MATH

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_PEANUTGB_IMPLEMENTATION
#include "../pntr_peanutgb.h"

bool Init(pntr_app* app) {
    const char* fileName = (const char*)pntr_app_userdata(app);

    // Load Peanut-GB
    // TODO: Use pntr_load_peanutgb_from_memory() for the whole app data instead
    struct gb_s* gb = pntr_load_peanutgb(fileName);
    if (gb == NULL) {
        return false;
    }

    // Save the gameboy state as the userdata for the application.
    pntr_app_set_userdata(app, gb);

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    struct gb_s* gb = (struct gb_s*)pntr_app_userdata(app);

    // Update the gb state
    if (!pntr_update_peanutgb(gb)) {
        return false;
    };

    // Render on the screen
    pntr_draw_peanutgb(screen, gb, 0, 0);

    return true;
}

void Event(pntr_app* app, pntr_app_event* event) {
    struct gb_s* gb = (struct gb_s*)pntr_app_userdata(app);

    pntr_peanutgb_event(gb, event);
}

void Close(pntr_app* app) {
    struct gb_s* gb = (struct gb_s*)pntr_app_userdata(app);

    pntr_unload_peanutgb(gb);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = LCD_WIDTH,
        .height = LCD_HEIGHT,
        .title = "pntr_peanutgb",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 60,
        .userData = argv[1]
    };
}
