// Baseado nos exemplos 03-image, 04-invert_image e 05-filter_image da disciplina.
// Copyright (c) 2026 Andre Kishimoto - https://kishimoto.com.br/
// SPDX-License-Identifier: Apache-2.0
// Modificacoes do grupo: Diogo Cornelio Martins Rosa (10403852),
// Jose Victor Scheurich Roling (10418225), Milton Almeida Leoncio (10416764),
// Vitor Neudl Gandolfi (10408845).

#include "image.h"

#include <stdio.h>
#include <string.h>
#include <SDL3_image/SDL_image.h>

static bool file_exists(const char *filename)
{
  FILE *file = fopen(filename, "rb");
  if (!file)
    return false;
  fclose(file);
  return true;
}

static bool surface_is_grayscale(SDL_Surface *surface)
{
  if (!surface)
    return false;

  if (!SDL_LockSurface(surface))
    return false;

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  bool grayscale = true;

  for (int y = 0; y < surface->h && grayscale; ++y)
  {
    Uint32 *row = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch);
    for (int x = 0; x < surface->w; ++x)
    {
      Uint8 r = 0, g = 0, b = 0;
      SDL_GetRGB(row[x], format, NULL, &r, &g, &b);
      if (r != g || g != b)
      {
        grayscale = false;
        break;
      }
    }
  }

  SDL_UnlockSurface(surface);
  return grayscale;
}

static bool surface_convert_to_grayscale(SDL_Surface *surface)
{
  if (!surface)
    return false;

  if (!SDL_LockSurface(surface))
  {
    SDL_Log("Erro ao bloquear a superficie: %s", SDL_GetError());
    return false;
  }

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);

  for (int y = 0; y < surface->h; ++y)
  {
    Uint32 *row = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch);
    for (int x = 0; x < surface->w; ++x)
    {
      Uint8 r = 0, g = 0, b = 0, a = 255;
      SDL_GetRGBA(row[x], format, NULL, &r, &g, &b, &a);

      double y_value = 0.2125 * (double)r + 0.7154 * (double)g + 0.0721 * (double)b;
      if (y_value < 0.0)
        y_value = 0.0;
      if (y_value > 255.0)
        y_value = 255.0;

      Uint8 gray = (Uint8)(y_value + 0.5);
      row[x] = SDL_MapRGBA(format, NULL, gray, gray, gray, a);
    }
  }

  SDL_UnlockSurface(surface);
  return true;
}

bool image_update_texture(MyImage *image, SDL_Renderer *renderer, SDL_Surface *surface)
{
  if (!image || !renderer || !surface)
    return false;

  SDL_Texture *new_texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!new_texture)
  {
    SDL_Log("Erro ao criar textura: %s", SDL_GetError());
    return false;
  }

  SDL_SetTextureScaleMode(new_texture, SDL_SCALEMODE_LINEAR);

  SDL_DestroyTexture(image->texture);
  image->texture = new_texture;
  image->current_surface = surface;
  return true;
}

bool image_load_and_prepare(const char *filename, SDL_Renderer *renderer,
                            MyImage *image, bool *input_was_grayscale)
{
  if (!filename || !renderer || !image)
    return false;

  if (!file_exists(filename))
  {
    fprintf(stderr, "Erro: arquivo '%s' nao encontrado.\n", filename);
    return false;
  }

  SDL_Surface *loaded = IMG_Load(filename);
  if (!loaded)
  {
    fprintf(stderr, "Erro: '%s' nao e uma imagem valida ou nao pode ser carregada: %s\n",
            filename, SDL_GetError());
    return false;
  }

  SDL_Surface *rgba = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(loaded);
  if (!rgba)
  {
    fprintf(stderr, "Erro ao converter a imagem para RGBA32: %s\n", SDL_GetError());
    return false;
  }

  bool grayscale = surface_is_grayscale(rgba);
  if (input_was_grayscale)
    *input_was_grayscale = grayscale;

  if (grayscale)
  {
    printf("Imagem de entrada: escala de cinza.\n");
  }
  else
  {
    printf("Imagem de entrada: colorida. Convertendo para escala de cinza...\n");
    if (!surface_convert_to_grayscale(rgba))
    {
      SDL_DestroySurface(rgba);
      return false;
    }
  }

  image_destroy(image);
  image->gray_surface = rgba;
  image->current_surface = rgba;
  image->width = rgba->w;
  image->height = rgba->h;
  image->showing_equalized = false;

  if (!image_update_texture(image, renderer, image->gray_surface))
  {
    image_destroy(image);
    return false;
  }

  printf("Imagem preparada em escala de cinza: %dx%d pixels.\n",
         image->width, image->height);
  return true;
}

bool image_set_equalized(MyImage *image, SDL_Surface *equalized_surface)
{
  if (!image || !equalized_surface)
    return false;

  SDL_DestroySurface(image->equalized_surface);
  image->equalized_surface = equalized_surface;
  return true;
}

bool image_show_original(MyImage *image, SDL_Renderer *renderer)
{
  if (!image || !image->gray_surface)
    return false;

  if (!image_update_texture(image, renderer, image->gray_surface))
    return false;

  image->showing_equalized = false;
  return true;
}

bool image_show_equalized(MyImage *image, SDL_Renderer *renderer)
{
  if (!image || !image->equalized_surface)
    return false;

  if (!image_update_texture(image, renderer, image->equalized_surface))
    return false;

  image->showing_equalized = true;
  return true;
}

bool image_save_current(const MyImage *image, const char *filename,
                        bool original_resolution, int display_width,
                        int display_height)
{
  if (!image || !image->current_surface || !filename)
    return false;

  SDL_Surface *surface_to_save = image->current_surface;
  SDL_Surface *scaled_surface = NULL;

  if (!original_resolution &&
      (image->current_surface->w != display_width || image->current_surface->h != display_height))
  {
    scaled_surface = SDL_ScaleSurface(image->current_surface, display_width, display_height,
                                      SDL_SCALEMODE_LINEAR);
    if (!scaled_surface)
    {
      fprintf(stderr, "Erro ao gerar imagem %dx%d para salvamento: %s\n",
              display_width, display_height, SDL_GetError());
      return false;
    }
    surface_to_save = scaled_surface;
  }

  bool existed = file_exists(filename);
  bool ok = IMG_SavePNG(surface_to_save, filename);
  SDL_DestroySurface(scaled_surface);

  if (!ok)
  {
    fprintf(stderr, "Erro ao salvar '%s': %s\n", filename, SDL_GetError());
    return false;
  }

  printf("Arquivo %s %s.\n", filename, existed ? "sobrescrito" : "criado");
  return true;
}

void image_destroy(MyImage *image)
{
  if (!image)
    return;

  SDL_DestroyTexture(image->texture);
  image->texture = NULL;

  SDL_DestroySurface(image->equalized_surface);
  image->equalized_surface = NULL;

  SDL_DestroySurface(image->gray_surface);
  image->gray_surface = NULL;

  image->current_surface = NULL;
  image->width = 0;
  image->height = 0;
  image->showing_equalized = false;
}
