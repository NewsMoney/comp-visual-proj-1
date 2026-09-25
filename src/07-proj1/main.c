// Projeto 1 - Processamento de imagens - Computacao Visual - 2026.2
// Baseado no repositorio da disciplina CompVis262, especialmente nos exemplos
// 03-image, 04-invert_image, 05-filter_image e 06-primitives.
//
// Integrantes:
// Diogo Cornelio Martins Rosa - 10403852
// Jose Victor Scheurich Roling - 10418225
// Milton Almeida Leoncio - 10416764
// Vitor Neudl Gandolfi - 10408845

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "types.h"
#include "image.h"
#include "histogram.h"
#include "gui.h"

static const char *MAIN_WINDOW_TITLE = "Projeto 1 - Processamento de Imagens";
static const char *PANEL_WINDOW_TITLE = "Projeto 1 - Histograma e Controles";
static const char *FONT_PATH = "assets/DejaVuSans.ttf";
static const char *OUTPUT_FILENAME = "output_image.png";

enum constants
{
  MAIN_WIDTH = 1024,
  MAIN_HEIGHT = 768,
  PANEL_WIDTH = 520,
  PANEL_HEIGHT = 690
};

static MyWindow g_main_window = { 0 };
static MyWindow g_panel_window = { 0 };
static MyImage g_image = { 0 };
static HistogramData g_histogram = { 0 };
static TTF_Font *g_font = NULL;

static Button g_equalize_button = {
  .rect = { .x = 20.0f, .y = 465.0f, .w = 480.0f, .h = 55.0f },
  .state = BUTTON_NEUTRAL
};

static Button g_resolution_button = {
  .rect = { .x = 20.0f, .y = 535.0f, .w = 480.0f, .h = 55.0f },
  .state = BUTTON_NEUTRAL
};

static bool g_original_resolution = false;
static bool g_running = true;
static bool g_refresh_main = true;
static bool g_refresh_panel = true;

static void shutdown(void)
{
  if (g_font)
  {
    TTF_CloseFont(g_font);
    g_font = NULL;
  }

  image_destroy(&g_image);
  window_destroy(&g_panel_window);
  window_destroy(&g_main_window);

  if (TTF_WasInit() > 0)
    TTF_Quit();
  SDL_Quit();
}

static bool initialize(void)
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    fprintf(stderr, "Erro ao iniciar SDL: %s\n", SDL_GetError());
    return false;
  }

  if (!TTF_Init())
  {
    fprintf(stderr, "Erro ao iniciar SDL_ttf: %s\n", SDL_GetError());
    return false;
  }

  if (!window_initialize(&g_main_window, MAIN_WINDOW_TITLE,
                         MAIN_WIDTH, MAIN_HEIGHT, 0))
  {
    fprintf(stderr, "Erro ao criar janela principal: %s\n", SDL_GetError());
    return false;
  }

  if (!window_initialize(&g_panel_window, PANEL_WINDOW_TITLE,
                         PANEL_WIDTH, PANEL_HEIGHT, 0))
  {
    fprintf(stderr, "Erro ao criar janela secundaria: %s\n", SDL_GetError());
    return false;
  }

  if (!SDL_SetWindowParent(g_panel_window.window, g_main_window.window))
  {
    fprintf(stderr, "Erro ao definir janela secundaria como filha: %s\n", SDL_GetError());
    return false;
  }

  position_main_window(g_main_window.window, MAIN_WIDTH, MAIN_HEIGHT);
  position_panel_window(g_panel_window.window);

  g_font = TTF_OpenFont(FONT_PATH, 18.0f);
  if (!g_font)
  {
    fprintf(stderr,
            "Erro ao carregar a fonte '%s': %s\n"
            "Inclua o arquivo DejaVuSans.ttf na pasta assets antes de executar.\n",
            FONT_PATH, SDL_GetError());
    return false;
  }

  return true;
}

static bool prepare_equalized_image(void)
{
  HistogramData original_histogram = { 0 };
  if (!histogram_calculate(g_image.gray_surface, &original_histogram))
    return false;

  SDL_Surface *equalized = histogram_equalize(g_image.gray_surface, &original_histogram);
  if (!equalized)
    return false;

  return image_set_equalized(&g_image, equalized);
}

static bool refresh_histogram(void)
{
  if (!histogram_calculate(g_image.current_surface, &g_histogram))
  {
    fprintf(stderr, "Erro ao calcular histograma.\n");
    return false;
  }

  printf("Histograma: media = %.2f (%s), desvio padrao = %.2f (%s).\n",
         g_histogram.mean, g_histogram.intensity_class,
         g_histogram.stddev, g_histogram.contrast_class);
  return true;
}

static void toggle_equalization(void)
{
  bool ok;
  if (g_image.showing_equalized)
    ok = image_show_original(&g_image, g_main_window.renderer);
  else
    ok = image_show_equalized(&g_image, g_main_window.renderer);

  if (!ok)
  {
    fprintf(stderr, "Erro ao alternar equalizacao.\n");
    return;
  }

  refresh_histogram();
  g_refresh_main = true;
  g_refresh_panel = true;
}

static void toggle_resolution(void)
{
  g_original_resolution = !g_original_resolution;

  int width = g_original_resolution ? g_image.width : MAIN_WIDTH;
  int height = g_original_resolution ? g_image.height : MAIN_HEIGHT;
  position_main_window(g_main_window.window, width, height);

  g_refresh_main = true;
  g_refresh_panel = true;
}

static void update_hover(float x, float y)
{
  if (g_equalize_button.state != BUTTON_PRESSED)
    g_equalize_button.state = button_contains(&g_equalize_button, x, y)
                            ? BUTTON_HOVER : BUTTON_NEUTRAL;

  if (g_resolution_button.state != BUTTON_PRESSED)
    g_resolution_button.state = button_contains(&g_resolution_button, x, y)
                             ? BUTTON_HOVER : BUTTON_NEUTRAL;

  g_refresh_panel = true;
}

static void handle_event(SDL_Event *event)
{
  if (!event)
    return;

  if (event->type == SDL_EVENT_QUIT ||
      event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
  {
    g_running = false;
    return;
  }

  if (event->type == SDL_EVENT_WINDOW_EXPOSED ||
      event->type == SDL_EVENT_WINDOW_RESIZED ||
      event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
  {
    g_refresh_main = true;
    g_refresh_panel = true;
  }

  if (event->type == SDL_EVENT_KEY_DOWN && !event->key.repeat)
  {
    if (event->key.key == SDLK_ESCAPE)
    {
      g_running = false;
    }
    else if (event->key.key == SDLK_S)
    {
      image_save_current(&g_image, OUTPUT_FILENAME, g_original_resolution,
                         MAIN_WIDTH, MAIN_HEIGHT);
    }
  }

  SDL_Window *event_window = SDL_GetWindowFromEvent(event);
  if (event_window != g_panel_window.window)
    return;

  if (event->type == SDL_EVENT_MOUSE_MOTION)
  {
    update_hover(event->motion.x, event->motion.y);
  }
  else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
           event->button.button == SDL_BUTTON_LEFT)
  {
    if (button_contains(&g_equalize_button, event->button.x, event->button.y))
      g_equalize_button.state = BUTTON_PRESSED;

    if (button_contains(&g_resolution_button, event->button.x, event->button.y))
      g_resolution_button.state = BUTTON_PRESSED;

    g_refresh_panel = true;
  }
  else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP &&
           event->button.button == SDL_BUTTON_LEFT)
  {
    bool equalize_clicked = g_equalize_button.state == BUTTON_PRESSED &&
      button_contains(&g_equalize_button, event->button.x, event->button.y);

    bool resolution_clicked = g_resolution_button.state == BUTTON_PRESSED &&
      button_contains(&g_resolution_button, event->button.x, event->button.y);

    g_equalize_button.state = button_contains(&g_equalize_button,
                                              event->button.x, event->button.y)
                            ? BUTTON_HOVER : BUTTON_NEUTRAL;
    g_resolution_button.state = button_contains(&g_resolution_button,
                                                event->button.x, event->button.y)
                             ? BUTTON_HOVER : BUTTON_NEUTRAL;

    if (equalize_clicked)
      toggle_equalization();
    else if (resolution_clicked)
      toggle_resolution();

    g_refresh_panel = true;
  }
}

static void loop(void)
{
  SDL_Event event;

  while (g_running)
  {
    while (SDL_PollEvent(&event))
      handle_event(&event);

    if (g_refresh_main)
    {
      render_main_window(&g_main_window, &g_image);
      g_refresh_main = false;
    }

    if (g_refresh_panel)
    {
      render_panel_window(&g_panel_window, g_font, &g_histogram,
                          &g_equalize_button, &g_resolution_button,
                          g_image.showing_equalized, g_original_resolution,
                          g_image.width, g_image.height);
      g_refresh_panel = false;
    }

    SDL_Delay(10);
  }
}

int main(int argc, char *argv[])
{
  atexit(shutdown);

  if (argc != 2)
  {
    fprintf(stderr, "Uso: %s caminho_da_imagem.ext\n", argc > 0 ? argv[0] : "projeto1");
    return SDL_APP_FAILURE;
  }

  if (!initialize())
    return SDL_APP_FAILURE;

  bool input_was_grayscale = false;
  if (!image_load_and_prepare(argv[1], g_main_window.renderer,
                              &g_image, &input_was_grayscale))
    return SDL_APP_FAILURE;

  if (!prepare_equalized_image())
  {
    fprintf(stderr, "Erro ao preparar versao equalizada da imagem.\n");
    return SDL_APP_FAILURE;
  }

  if (!refresh_histogram())
    return SDL_APP_FAILURE;

  // O requisito define 1024x768 como estado inicial da janela principal.
  g_original_resolution = false;
  position_main_window(g_main_window.window, MAIN_WIDTH, MAIN_HEIGHT);
  position_panel_window(g_panel_window.window);

  printf("Programa iniciado. S salva a imagem atual; ESC encerra.\n");
  loop();

  return 0;
}
