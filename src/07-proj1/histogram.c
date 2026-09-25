// Histograma e equalizacao implementados para o Projeto 1 de Computacao Visual.
// O acesso aos pixels segue a abordagem apresentada nos exemplos da disciplina.

#include "histogram.h"

#include <math.h>
#include <string.h>

bool histogram_calculate(SDL_Surface *surface, HistogramData *histogram)
{
  if (!surface || !histogram)
    return false;

  memset(histogram, 0, sizeof(*histogram));

  if (!SDL_LockSurface(surface))
  {
    SDL_Log("Erro ao bloquear superficie para histograma: %s", SDL_GetError());
    return false;
  }

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);

  for (int y = 0; y < surface->h; ++y)
  {
    Uint32 *row = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch);
    for (int x = 0; x < surface->w; ++x)
    {
      Uint8 intensity = 0;
      SDL_GetRGB(row[x], format, NULL, &intensity, NULL, NULL);
      ++histogram->bins[intensity];
      ++histogram->total_pixels;
    }
  }

  SDL_UnlockSurface(surface);

  if (histogram->total_pixels == 0)
    return false;

  double sum = 0.0;
  for (int i = 0; i < 256; ++i)
    sum += (double)i * (double)histogram->bins[i];

  histogram->mean = sum / (double)histogram->total_pixels;

  double variance_sum = 0.0;
  for (int i = 0; i < 256; ++i)
  {
    double delta = (double)i - histogram->mean;
    variance_sum += delta * delta * (double)histogram->bins[i];
  }
  histogram->stddev = sqrt(variance_sum / (double)histogram->total_pixels);

  // A intensidade [0,255] foi dividida em tres faixas de mesmo tamanho.
  if (histogram->mean < 85.0)
    histogram->intensity_class = "escura";
  else if (histogram->mean < 170.0)
    histogram->intensity_class = "media";
  else
    histogram->intensity_class = "clara";

  // Para 8 bits, o desvio-padrao maximo teorico e aproximadamente 127,5.
  // O intervalo foi dividido em tres faixas para a classificacao do contraste.
  if (histogram->stddev < 42.5)
    histogram->contrast_class = "baixo";
  else if (histogram->stddev < 85.0)
    histogram->contrast_class = "medio";
  else
    histogram->contrast_class = "alto";

  return true;
}

SDL_Surface *histogram_equalize(SDL_Surface *gray_surface,
                                const HistogramData *histogram)
{
  if (!gray_surface || !histogram || histogram->total_pixels == 0)
    return NULL;

  SDL_Surface *output = SDL_DuplicateSurface(gray_surface);
  if (!output)
  {
    SDL_Log("Erro ao duplicar superficie para equalizacao: %s", SDL_GetError());
    return NULL;
  }

  Uint64 cdf[256] = { 0 };
  cdf[0] = histogram->bins[0];
  for (int i = 1; i < 256; ++i)
    cdf[i] = cdf[i - 1] + histogram->bins[i];

  Uint64 cdf_min = 0;
  for (int i = 0; i < 256; ++i)
  {
    if (histogram->bins[i] != 0)
    {
      cdf_min = cdf[i];
      break;
    }
  }

  Uint8 lut[256];
  Uint64 denominator = histogram->total_pixels - cdf_min;

  if (denominator == 0)
  {
    for (int i = 0; i < 256; ++i)
      lut[i] = (Uint8)i;
  }
  else
  {
    for (int i = 0; i < 256; ++i)
    {
      if (cdf[i] <= cdf_min)
      {
        lut[i] = 0;
      }
      else
      {
        double normalized = ((double)(cdf[i] - cdf_min) / (double)denominator) * 255.0;
        if (normalized < 0.0)
          normalized = 0.0;
        if (normalized > 255.0)
          normalized = 255.0;
        lut[i] = (Uint8)(normalized + 0.5);
      }
    }
  }

  if (!SDL_LockSurface(output))
  {
    SDL_Log("Erro ao bloquear superficie para equalizacao: %s", SDL_GetError());
    SDL_DestroySurface(output);
    return NULL;
  }

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(output->format);

  for (int y = 0; y < output->h; ++y)
  {
    Uint32 *row = (Uint32 *)((Uint8 *)output->pixels + y * output->pitch);
    for (int x = 0; x < output->w; ++x)
    {
      Uint8 intensity = 0, alpha = 255;
      SDL_GetRGBA(row[x], format, NULL, &intensity, NULL, NULL, &alpha);
      Uint8 value = lut[intensity];
      row[x] = SDL_MapRGBA(format, NULL, value, value, value, alpha);
    }
  }

  SDL_UnlockSurface(output);
  return output;
}
