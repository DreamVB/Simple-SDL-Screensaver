//SDL Screen Saver beta 1

#include <exception>
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

int WINDOW_WIDTH = 0;
int WINDOW_HEIGHT = 0;
const int FLAKES_COUNT = 128;
const int FLAKE_RADIUS = 48;
const int DRAW_INTERVAL = 12;
const int FALL_SPEED = 3;

typedef struct SnowFlakes
{
    int x, y;
    int radius;
    int speed;
} Flake;

void flakePos(Flake *flake, SDL_Texture * circle)
{
    flake->y+= flake->speed;

    if (flake->y > WINDOW_HEIGHT)
    {
        flake->y = 0;
    }
}

void flakeDraw(Flake * flake, SDL_Renderer * RenderTarget, SDL_Texture * circle)
{
    SDL_Rect rect;
    rect.x = flake->x;
    rect.y = flake->y;
    rect.w = flake->radius;
    rect.h = flake->radius;

    SDL_RenderCopy(RenderTarget, circle, NULL, &rect);
}

void _InitSnowFlakes(Flake * flake)
{
    flake->x = rand() % WINDOW_WIDTH;
    flake->y = rand() % WINDOW_HEIGHT;
    flake->radius = rand() % FLAKE_RADIUS + 1;
    flake->speed = (rand() % FALL_SPEED + 1);
}

int main(int argc, char *argv[])
{
    int mousex = 0;
    int i = 0;
    bool IsRunning = true;
    SDL_Event ev;
    SDL_DisplayMode DM;
    Flake flakes[FLAKES_COUNT] = {0};
    std::string lzBackGround = "";
    std::string lzObj = "";
    int img_flags = IMG_INIT_PNG;

    if(argc != 2)
    {
        std::cout << "Invalid Command." << std::endl;
        return 0;
    }

    //Check for command arg for -snow used for snow screen saver.
    if(strcmp(argv[1],"-snow") == 0)
    {
        lzBackGround = "data\\snow\\background.png";
        lzObj = "data\\snow\\flake.png";
        //Check for command arg for -rain used for rain screen saver.
    }
    else if(strcmp(argv[1],"-rain") == 0)
    {
        lzBackGround = "data\\rain\\background.png";
        lzObj = "data\\rain\\rain.png";
    }
    else
    {
        std::cout << "Invalid Screen Saver Load." << std::endl;
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "STD Init Video Error." << std::endl << SDL_GetError() << std::endl;
        exit(1);
    }
    else if(!IMG_Init(img_flags) && img_flags)
    {
        std::cout << "Image Init Error." << std::endl << IMG_GetError() << std::endl;
        exit(1);
    }
    else
    {
        //Get screen res size.
        if(SDL_GetCurrentDisplayMode(0,&DM) != 0)
        {
            std::cout << "SDL_GetDesktopDisplayMode Failed." <<std::endl << SDL_GetError() << std::endl;
            exit(1);
        }
        else
        {
            WINDOW_WIDTH = DM.w;
            WINDOW_HEIGHT = DM.h;
        }

        //Create a full screen window for this program.
        SDL_Window *window = SDL_CreateWindow("Snow Flakes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);

        if (window == NULL)
        {
            std::cout << "SDL Video Error." << std::endl<< SDL_GetError() << std::endl;
            exit(1);
        }

        SDL_Renderer * RenderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Surface * BackgroundSurface = IMG_Load(lzBackGround.c_str());
        SDL_Surface * FlakeSurface = IMG_Load(lzObj.c_str());

        //Check surfaces load
        if(BackgroundSurface == NULL)
        {
            std::cout << "Background Surface Error." << std::endl << SDL_GetError() << std::endl;
            exit(1);
        }
        if(FlakeSurface == NULL)
        {
            std::cout << "Snow Flake Surface Error." << std::endl << SDL_GetError() << std::endl;
            exit(1);
        }

        SDL_SetColorKey(FlakeSurface, SDL_TRUE, SDL_MapRGB(FlakeSurface->format, 255, 0, 255));

        SDL_Texture *bkGround = SDL_CreateTextureFromSurface(RenderTarget, BackgroundSurface);
        SDL_Texture *snowFlake = SDL_CreateTextureFromSurface(RenderTarget, FlakeSurface);

        SDL_SetTextureBlendMode(snowFlake, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(bkGround, SDL_BLENDMODE_BLEND);

        SDL_SetTextureAlphaMod(bkGround, 255);
        SDL_SetTextureAlphaMod(snowFlake, 255);
        SDL_SetTextureColorMod(snowFlake, 255, 255, 255);
        //Hide cursor
        SDL_ShowCursor(0);

        //Prepare snow flakes loactions
        while(i < FLAKES_COUNT)
        {
            _InitSnowFlakes(&flakes[i]);
            i++;
        }

        while (IsRunning)
        {
            while (SDL_PollEvent(&ev))
            {
                //Check if quite was pressed.
                if(ev.type == SDL_QUIT)
                {
                    IsRunning = false;
                }
                //Check for mouse movement
                if(ev.type == SDL_MOUSEMOTION)
                {
                    //Check if the mousex is greater then 6
                    mousex++;
                    if(mousex > 6)
                        IsRunning = false;
                }
                //Check for key press.
                if(ev.type == SDL_KEYDOWN)
                {
                    IsRunning = false;
                }
                //Check for mouse click.
                if(ev.type == SDL_MOUSEBUTTONDOWN)
                {
                    IsRunning = false;
                }
            }
            //Clear the screen
            SDL_RenderClear(RenderTarget);
            //Render the background on the screen
            SDL_RenderCopy(RenderTarget, bkGround, NULL, NULL);

            i = 0;
            //Put the snow flakes to the screen.
            while(i < FLAKES_COUNT)
            {
                flakeDraw(&flakes[i], RenderTarget, snowFlake);
                flakePos(&flakes[i], snowFlake);
                i++;
            }

            SDL_RenderPresent(RenderTarget);
            //Add a small delay
            SDL_Delay(DRAW_INTERVAL);
        }
        //Clear up.
        SDL_FreeSurface(FlakeSurface);
        SDL_FreeSurface(BackgroundSurface);
        SDL_DestroyWindow(window);
        //Destroy pointers.
        FlakeSurface = BackgroundSurface = nullptr;
        memset(flakes,sizeof(flakes),0);
        //Show the cursor.
        SDL_ShowCursor(1);
        SDL_Quit();
        //Exit message.
        std::cout << "Thank you for trying my screen saver." << std::endl
                  << "Check out more of my projects at https://github.com/DreamVB" << std::endl;
    }
    return 0;
}
