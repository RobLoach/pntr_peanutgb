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

    // TODO: Have pntr_app provide the full data so we don't have to load the file.
    unsigned int bytesRead;
    void* data = pntr_load_file(fileName, &bytesRead);

    // Load peanutgb
    struct gb_s* gb = pntr_load_peanutgb(data);
    if (gb == NULL) {
        pntr_unload_file(data);
        return false;
    }

    pntr_app_set_userdata(app, gb);
    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    struct gb_s* gb = (struct gb_s*)pntr_app_userdata(app);

    return pntr_update_peanutgb(gb, screen, 0, 0);
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
