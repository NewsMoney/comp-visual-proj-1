#ifndef PROJ1_GUI_H
#define PROJ1_GUI_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "types.h"

bool window_initialize(MyWindow *window, const char *title,
                       int width, int height, SDL_WindowFlags flags);
void window_destroy(MyWindow *window);

bool position_main_window(SDL_Window *window, int width, int height);
bool position_panel_window(SDL_Window *window);

bool button_contains(const Button *button, float x, float y);
void button_render(SDL_Renderer *renderer, TTF_Font *font,
                   const Button *button, const char *text);

void render_main_window(const MyWindow *main_window, const MyImage *image);
void render_panel_window(const MyWindow *panel_window, TTF_Font *font,
                         const HistogramData *histogram,
                         const Button *equalize_button,
                         const Button *resolution_button,
                         bool showing_equalized,
                         bool original_resolution,
                         int image_width, int image_height);

#endif
