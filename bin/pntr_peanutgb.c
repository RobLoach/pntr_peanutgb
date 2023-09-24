#include <stdio.h>

#define PNTR_ENABLE_DEFAULT_FONT
//#define PNTR_ENABLE_FILTER_SMOOTH
//#define PNTR_ENABLE_TTF
//#define PNTR_ENABLE_MATH
#define PNTR_DISABLE_MATH

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_PEANUTGB_IMPLEMENTATION
#include "../pntr_peanutgb.h"

#include <stdio.h>

typedef struct AppData {
    struct gb_s* gb;
    pntr_font* font;
} AppData;

bool pntr_peanutgb_load_cart(pntr_app* app, void* fileData) {
    if (fileData == NULL) {
        return false;
    }

    AppData* appData = pntr_app_userdata(app);
    if (appData == NULL) {
        return false;
    }

    if (appData->gb != NULL) {
        pntr_unload_peanutgb(appData->gb);
        appData->gb = NULL;
    }

    // Load Peanut-GB
    appData->gb = pntr_load_peanutgb_from_memory(fileData);
    if (appData->gb == NULL) {
        pntr_app_log(PNTR_APP_LOG_ERROR, "Failed to load given cart");
        pntr_unload_file(fileData);
        return false;
    }

    return true;
}

bool Init(pntr_app* app) {
    // Set up the application data.
    AppData* appData = pntr_load_memory(sizeof(AppData));
    if (appData == NULL) {
        return false;
    }

    appData->font = pntr_load_font_default();
    if (appData->font == NULL) {
        pntr_unload_memory(appData);
        return false;
    }
    pntr_app_set_userdata(app, appData);

    void* fileData = pntr_app_load_arg_file(app, NULL);
    if (fileData != NULL) {
        pntr_peanutgb_load_cart(app, fileData);
    }

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    if (appData == NULL) {
        return false;
    }

    if (appData->gb == NULL) {
        pntr_clear_background(screen, pntr_get_color(0x346856FF));
        pntr_draw_text(screen, appData->font, "pntr peanutgb", 30, 10, pntr_get_color(0xe0f8d0FF));
        pntr_draw_line(screen, 30, 20, 134, 20, pntr_get_color(0xe0f8d0FF));
        pntr_draw_text(screen, appData->font, "Drag and Drop", 30, 60, pntr_get_color(0xe0f8d0FF));
        pntr_draw_text(screen, appData->font, "a cart on top", 30, 72, pntr_get_color(0xe0f8d0FF));
        return true;
    }

    // Update the gb state
    pntr_update_peanutgb(appData->gb);

    // Render on the screen
    pntr_draw_peanutgb(screen, appData->gb, 0, 0);

    return true;
}

void Event(pntr_app* app, pntr_app_event* event) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    if (appData == NULL || event == NULL) {
        return;
    }

    switch (event->type) {
        case PNTR_APP_EVENTTYPE_FILE_DROPPED: {
            unsigned int size;
            pntr_peanutgb_load_cart(app, pntr_load_file(event->fileDropped, &size));
            return;
        }
        break;
    }

    pntr_peanutgb_event(appData->gb, event);
}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    pntr_unload_peanutgb(appData->gb);
    pntr_unload_font(appData->font);
    pntr_unload_memory(appData);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = pntr_peanutgb_width(),
        .height = pntr_peanutgb_height(),
        .title = "pntr_peanutgb",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 60
    };
}
