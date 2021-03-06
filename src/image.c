// image.c

#include "../include/simple2d.h"


/*
 * Create an image, given a file path
 */
S2D_Image *S2D_CreateImage(const char *path) {
  S2D_Init();
  
  // Check if image file exists
  if (!S2D_FileExists(path)) {
    S2D_Error("S2D_CreateImage", "Image file not found");
    return NULL;
  }
  
  // Allocate the image structure
  S2D_Image *img = (S2D_Image *) malloc(sizeof(S2D_Image));
  if (!img) {
    S2D_Error("IMG_Load", "Out of memory!");
    return NULL;
  }
  
  // Load image from file as SDL_Surface
  img->surface = IMG_Load(path);
  if (!img->surface) {
    S2D_Error("IMG_Load", IMG_GetError());
    return NULL;
  }
  
  // Initialize values
  img->x = 0;
  img->y = 0;
  img->color.r = 1.f;
  img->color.g = 1.f;
  img->color.b = 1.f;
  img->color.a = 1.f;
  img->orig_width  = img->surface->w;
  img->orig_height = img->surface->h;
  img->width  = img->orig_width;
  img->height = img->orig_height;
  img->texture_id = 0;
  
  // Detect image mode
  img->format = GL_RGB;
  if (img->surface->format->BytesPerPixel == 4) {
    img->format = GL_RGBA;
  }
  
  // Flip image bits if BGA
  
  Uint32 r = img->surface->format->Rmask;
  Uint32 g = img->surface->format->Gmask;
  Uint32 a = img->surface->format->Amask;
  
  if (r&0xFF000000 || r&0xFF0000) {
    char *p = (char *)img->surface->pixels;
    int bpp = img->surface->format->BytesPerPixel;
    int w = img->surface->w;
    int h = img->surface->h;
    char tmp;
    for (int i = 0; i < bpp * w * h; i += bpp) {
      if (a&0xFF) {
        tmp = p[i];
        p[i] = p[i+3];
        p[i+3] = tmp;
      }
      if (g&0xFF0000) {
        tmp = p[i+1];
        p[i+1] = p[i+2];
        p[i+2] = tmp;
      }
      if (r&0xFF0000) {
        tmp = p[i];
        p[i] = p[i+2];
        p[i+2] = tmp;
      }
    }
  }
  
  return img;
}


/*
 * Draw an image
 */
void S2D_DrawImage(S2D_Image *img) {
  if (!img) return;
  
  if (img->texture_id == 0) {
    S2D_GL_CreateTexture(&img->texture_id, img->format,
                         img->orig_width, img->orig_height,
                         img->surface->pixels, GL_NEAREST);
    SDL_FreeSurface(img->surface);
  }
  
  S2D_GL_DrawImage(img);
}


/*
 * Free an image
 */
void S2D_FreeImage(S2D_Image *img) {
  if (!img) return;
  S2D_GL_FreeTexture(&img->texture_id);
  free(img);
}
