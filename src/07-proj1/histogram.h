#ifndef PROJ1_HISTOGRAM_H
#define PROJ1_HISTOGRAM_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include "types.h"

bool histogram_calculate(SDL_Surface *surface, HistogramData *histogram);
SDL_Surface *histogram_equalize(SDL_Surface *gray_surface,
                                const HistogramData *histogram);

#endif
