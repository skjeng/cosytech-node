/*
*   ____               _____         _
*  / ___|___  ___ _   |_   _|__  ___| |__
* | |   / _ \/ __| | | || |/ _ \/ __| '_ \
* | |__| (_) \__ \ |_| || |  __/ (__| | | |
*  \____\___/|___/\__, ||_|\___|\___|_| |_|
*                 |___/
*
* CosyTech Skunkworks - Visual Program
* By Joakim Skjefstad (joakim@cosytech.net)
* Based on tutorials by http://www.willusher.io/ and http://lazyfoo.net/
* Developed on OS X using dependencies:
* brew install sdl2
* brew install sdl2_image #PNG Support
* brew install sdl2_mixer #MP3 Support
*/

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "res_path.h"
#include "cleanup.h"


// Window screen dimensions, will be changed later to current display size
int SCREEN_WIDTH  = 640;
int SCREEN_HEIGHT = 480;

/**
* Log an SDL error with some error message to the output stream of our choice
* @param os The output stream to write the message to
* @param msg The error message to write, format will be msg error: SDL_GetError()
*/
void logSDLError(std::ostream &os, const std::string &msg){
  os << msg << " error: " << SDL_GetError() << std::endl;
}

/**
* Loads an image into a texture on the rendering device
* @param file The image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
  SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
  if (texture == nullptr){
    logSDLError(std::cout, "LoadTexture");
  }
  return texture;
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
  //Setup the destination rectangle to be at the position we want
  SDL_Rect dst;
  dst.x = x-w;
  dst.y = y;
  dst.w = w;
  dst.h = h;


  //Angle of rotation
  double degrees = 270;

  // Point of rotation origin, upper right corner
  SDL_Point origin={w,0};

  //Flip type
  SDL_RendererFlip flipType = SDL_FLIP_NONE;


  SDL_RenderCopyEx(ren, tex, NULL, &dst, degrees, &origin, flipType);
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &w, &h);
  renderTexture(tex, ren, x, y, w, h);
}


int main(int argc, char **argv){
  SDL_DisplayMode current;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
    logSDLError(std::cout, "SDL_Init");
    return 1;
  }

  // List all displays
  for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i){
    int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);
    if(should_be_zero != 0){
      logSDLError(std::cout, "Could not get display mode for video display.");
    } else {
      // On success, print the current display mode.
      printf("Display #%d: current display mode is %dx%dpx @ %dhz. \n", i, current.w, current.h, current.refresh_rate);
    }
  }

  // Retrieve dimensions of first display
  int should_be_zero = SDL_GetCurrentDisplayMode(0, &current);
  if(should_be_zero != 0){
      logSDLError(std::cout, "Could not get display mode for video display #0.");
    } else {
      std::cout << "Using Display #0." << std::endl;

      SCREEN_WIDTH = current.w;
      SCREEN_HEIGHT = current.h;
    }


  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
    logSDLError(std::cout, "IMG_Init");
    SDL_Quit();
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("InteractiveAd", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window == nullptr){
    logSDLError(std::cout, "CreateWindow Fullscreen failed");
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr){
    logSDLError(std::cout, "CreateRenderer");
    cleanup(window);
    SDL_Quit();
    return 1;
  }

  const std::string resPath = getResourcePath("img");
  SDL_Texture *image = loadTexture(resPath + "1.png", renderer);
  //Make sure loaded ok
  if (image == nullptr){
    cleanup(image, renderer, window);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  int iW, iH;
  SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
  std::cout << "Image W=" << iW << " H=" << iH << std::endl;

  //Our event structure
  SDL_Event e;
  bool quit = false;
  while (!quit){
    while (SDL_PollEvent(&e)){
      if (e.type == SDL_QUIT){
        quit = true;
      }
      if (e.type == SDL_KEYDOWN){
        quit = true;
      }
      if (e.type == SDL_MOUSEBUTTONDOWN){
        quit = true;
      }
    }
    //Render the scene
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    renderTexture(image, renderer, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH);
    SDL_RenderPresent(renderer);
  }

  cleanup(image, renderer, window);
  IMG_Quit();
  SDL_Quit();

	return 0;
}