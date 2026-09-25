#ifndef PROJ1_TYPES_H
#define PROJ1_TYPES_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct MyWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
} MyWindow;

typedef struct MyImage
{
  SDL_Surface *gray_surface;
  SDL_Surface *equalized_surface;
  SDL_Surface *current_surface;
  SDL_Texture *texture;
  int width;
  int height;
  bool showing_equalized;
} MyImage;

typedef struct HistogramData
{
  Uint64 bins[256];
  Uint64 total_pixels;
  double mean;
  double stddev;
  const char *intensity_class;
  const char *contrast_class;
} HistogramData;

typedef enum ButtonState
{
  BUTTON_NEUTRAL = 0,
  BUTTON_HOVER,
  BUTTON_PRESSED
} ButtonState;

typedef struct Button
{
  SDL_FRect rect;
  ButtonState state;
} Button;

#endif
