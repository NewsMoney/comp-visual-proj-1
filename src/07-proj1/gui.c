// Interface grafica baseada nos exemplos 04-invert_image e 06-primitives.
// Copyright (c) 2026 Andre Kishimoto - https://kishimoto.com.br/
// SPDX-License-Identifier: Apache-2.0

#include "gui.h"

#include <stdio.h>
#include <string.h>

static void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                      float x, float y, SDL_Color color)
{
  if (!renderer || !font || !text)
    return;

  SDL_Surface *surface = TTF_RenderText_Blended(font, text, strlen(text), color);
  if (!surface)
  {
    SDL_Log("Erro ao renderizar texto: %s", SDL_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
  {
    SDL_DestroySurface(surface);
    SDL_Log("Erro ao criar textura de texto: %s", SDL_GetError());
    return;
  }

  SDL_FRect dst = { x, y, (float)surface->w, (float)surface->h };
  SDL_RenderTexture(renderer, texture, NULL, &dst);

  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);
}

bool window_initialize(MyWindow *window, const char *title,
                       int width, int height, SDL_WindowFlags flags)
{
  if (!window)
    return false;

  return SDL_CreateWindowAndRenderer(title, width, height, flags,
                                     &window->window, &window->renderer);
}

void window_destroy(MyWindow *window)
{
  if (!window)
    return;

  SDL_DestroyRenderer(window->renderer);
  SDL_DestroyWindow(window->window);
  window->renderer = NULL;
  window->window = NULL;
}

bool position_main_window(SDL_Window *window, int width, int height)
{
  if (!window)
    return false;

  if (!SDL_SetWindowSize(window, width, height))
  {
    SDL_Log("Erro ao redimensionar janela principal: %s", SDL_GetError());
    return false;
  }

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  SDL_Rect bounds = { 0, 0, width, height };
  bool have_bounds = display != 0 && SDL_GetDisplayBounds(display, &bounds);

  if (have_bounds && (width > bounds.w || height > bounds.h))
  {
    int top = 0, left = 0;
    SDL_GetWindowBordersSize(window, &top, &left, NULL, NULL);
    SDL_SetWindowPosition(window, bounds.x + left, bounds.y + top);
  }
  else
  {
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  }

  SDL_SyncWindow(window);
  return true;
}

bool position_panel_window(SDL_Window *window)
{
  if (!window)
    return false;

  int top = 0, left = 0;
  SDL_GetWindowBordersSize(window, &top, &left, NULL, NULL);

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  SDL_Rect bounds = { 0, 0, 0, 0 };
  if (display != 0 && SDL_GetDisplayBounds(display, &bounds))
    SDL_SetWindowPosition(window, bounds.x + left, bounds.y + top);
  else
    SDL_SetWindowPosition(window, left, top);

  SDL_SyncWindow(window);
  return true;
}

bool button_contains(const Button *button, float x, float y)
{
  if (!button)
    return false;

  return x >= button->rect.x && x <= button->rect.x + button->rect.w &&
         y >= button->rect.y && y <= button->rect.y + button->rect.h;
}

void button_render(SDL_Renderer *renderer, TTF_Font *font,
                   const Button *button, const char *text)
{
  if (!renderer || !button || !text)
    return;

  SDL_Color fill;
  switch (button->state)
  {
    case BUTTON_HOVER:   fill = (SDL_Color){ 90, 160, 235, 255 }; break;
    case BUTTON_PRESSED: fill = (SDL_Color){ 20, 70, 135, 255 }; break;
    default:             fill = (SDL_Color){ 45, 115, 195, 255 }; break;
  }

  SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
  SDL_RenderFillRect(renderer, &button->rect);

  SDL_SetRenderDrawColor(renderer, 220, 235, 250, 255);
  SDL_RenderRect(renderer, &button->rect);

  SDL_Color text_color = { 255, 255, 255, 255 };

  SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, strlen(text), text_color);
  if (!text_surface)
    return;

  float x = button->rect.x + (button->rect.w - (float)text_surface->w) * 0.5f;
  float y = button->rect.y + (button->rect.h - (float)text_surface->h) * 0.5f;

  SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  if (text_texture)
  {
    SDL_FRect dst = { x, y, (float)text_surface->w, (float)text_surface->h };
    SDL_RenderTexture(renderer, text_texture, NULL, &dst);
    SDL_DestroyTexture(text_texture);
  }

  SDL_DestroySurface(text_surface);
}

void render_main_window(const MyWindow *main_window, const MyImage *image)
{
  if (!main_window || !main_window->renderer || !image || !image->texture)
    return;

  int width = 0, height = 0;
  SDL_GetWindowSize(main_window->window, &width, &height);

  SDL_SetRenderDrawColor(main_window->renderer, 32, 32, 32, 255);
  SDL_RenderClear(main_window->renderer);

  SDL_FRect dst = { 0.0f, 0.0f, (float)width, (float)height };
  SDL_RenderTexture(main_window->renderer, image->texture, NULL, &dst);
  SDL_RenderPresent(main_window->renderer);
}

void render_panel_window(const MyWindow *panel_window, TTF_Font *font,
                         const HistogramData *histogram,
                         const Button *equalize_button,
                         const Button *resolution_button,
                         bool showing_equalized,
                         bool original_resolution,
                         int image_width, int image_height)
{
  if (!panel_window || !panel_window->renderer || !font || !histogram)
    return;

  SDL_Renderer *renderer = panel_window->renderer;
  SDL_SetRenderDrawColor(renderer, 24, 27, 32, 255);
  SDL_RenderClear(renderer);

  SDL_Color white = { 240, 240, 240, 255 };
  SDL_Color soft = { 190, 200, 215, 255 };

  draw_text(renderer, font, "Histograma da imagem atual", 20.0f, 15.0f, white);

  SDL_FRect plot = { 20.0f, 55.0f, 480.0f, 250.0f };
  SDL_SetRenderDrawColor(renderer, 70, 76, 86, 255);
  SDL_RenderRect(renderer, &plot);

  Uint64 max_bin = 1;
  for (int i = 0; i < 256; ++i)
  {
    if (histogram->bins[i] > max_bin)
      max_bin = histogram->bins[i];
  }

  float bar_width = plot.w / 256.0f;
  SDL_SetRenderDrawColor(renderer, 210, 215, 225, 255);
  for (int i = 0; i < 256; ++i)
  {
    float normalized = (float)((double)histogram->bins[i] / (double)max_bin);
    float height = normalized * (plot.h - 2.0f);
    SDL_FRect bar = {
      plot.x + (float)i * bar_width,
      plot.y + plot.h - height,
      bar_width + 0.25f,
      height
    };
    SDL_RenderFillRect(renderer, &bar);
  }

  char line[160];
  snprintf(line, sizeof(line), "Media: %.2f  -> imagem %s",
           histogram->mean, histogram->intensity_class);
  draw_text(renderer, font, line, 20.0f, 325.0f, white);

  snprintf(line, sizeof(line), "Desvio padrao: %.2f  -> contraste %s",
           histogram->stddev, histogram->contrast_class);
  draw_text(renderer, font, line, 20.0f, 355.0f, white);

  snprintf(line, sizeof(line), "Imagem: %dx%d | exibicao: %s",
           image_width, image_height,
           original_resolution ? "resolucao original" : "1024x768");
  draw_text(renderer, font, line, 20.0f, 385.0f, soft);

  draw_text(renderer, font,
            showing_equalized ? "Estado: histograma equalizado" : "Estado: escala de cinza original",
            20.0f, 415.0f, soft);

  button_render(renderer, font, equalize_button,
                showing_equalized ? "Ver original" : "Equalizar");
  button_render(renderer, font, resolution_button,
                original_resolution ? "1024x768" : "Resolucao original");

  draw_text(renderer, font, "Tecla S: salvar output_image.png", 20.0f, 610.0f, soft);
  draw_text(renderer, font, "ESC: sair", 20.0f, 638.0f, soft);

  SDL_RenderPresent(renderer);
}
