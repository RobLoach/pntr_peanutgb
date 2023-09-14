# pntr_peanutgb

[Peanut-GB](https://github.com/deltabeard/Peanut-GB) Game Boy emulator for [pntr](https://github.com/RobLoach/pntr).

![Screenshot](screenshot.png)

## API

``` c
struct gb_s* pntr_load_peanutgb(const void* data);
void pntr_unload_peanutgb(struct gb_s* gb);
bool pntr_update_peanutgb(struct gb_s* gb, pntr_image* dst, int posX, int posY);
void pntr_peanutgb_set_palette(struct gb_s* gb, pntr_color col1, pntr_color col2, pntr_color col3, pntr_color col4);
```

# License

[MIT](LICENSE)
