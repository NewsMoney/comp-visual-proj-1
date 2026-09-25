#ifndef PROJ1_IMAGE_H
#define PROJ1_IMAGE_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include "types.h"

bool image_load_and_prepare(const char *filename, SDL_Renderer *renderer,
                            MyImage *image, bool *input_was_grayscale);
bool image_update_texture(MyImage *image, SDL_Renderer *renderer,
                          SDL_Surface *surface);
bool image_set_equalized(MyImage *image, SDL_Surface *equalized_surface);
bool image_show_original(MyImage *image, SDL_Renderer *renderer);
bool image_show_equalized(MyImage *image, SDL_Renderer *renderer);
bool image_save_current(const MyImage *image, const char *filename,
                        bool original_resolution, int display_width,
                        int display_height);
void image_destroy(MyImage *image);

#endif
