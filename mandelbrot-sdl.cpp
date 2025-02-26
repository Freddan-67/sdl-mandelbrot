#include <SDL2/SDL.h>

#include <stdio.h>
#include <math.h>
#include <complex>

#define WIDTH 800
#define HEIGHT 600

#define START_POS   -0.5
#define START_ZOOM  (WIDTH * 0.25296875f)

#define BAIL_OUT        2.0
#define FLIPS           24

#define ZOOM_FACTOR     4
using namespace std;

const complex<double> Complex_I(0.0,1.0);
void sdl_draw_mandelbrot(SDL_Window *window, SDL_Surface *surface, complex<double> center, double zoom)
{
    int f,x,y,n;
    int maxiter = (WIDTH/2) * 0.049715909 * log10(zoom);
    complex <double> z, c;
    float C;

    static SDL_Rect rects[HEIGHT/FLIPS];

    fprintf(stderr, "zoom: %f\n", zoom);
    fprintf(stderr, "center point: %f %f\n", center.real(),
                                              center.imag() );
    fprintf(stderr, "iterations: %d\n", maxiter);

    for (f = 0; f < FLIPS; f++)
    {
        for  (y = f; y < HEIGHT; y += FLIPS)
        {
            for (x = 0; x < WIDTH; x++)
            {
                /* Get the complex poing on gauss space to be calculate */
                z = c = center.real() + ((x - (WIDTH/2))/zoom) + 
                    ((center.imag() + ((y - (HEIGHT/2))/zoom))*Complex_I);

                //#define X creal(z)
                //#define Y cimag(z)

                /* Check if point lies within the main cardiod or 
                   in the period-2 buld */
                // if ( (pow(X-.25,2) + pow(Y,2))*(pow(X,2) + (X/2) + pow(Y,2) - .1875) < pow(Y,2)/4 ||
                //       pow(X+1,2) + pow(Y,2) < .0625 )

                if ( (pow(z.real()-.25,2) + pow(z.imag(),2))*
		     (pow(z.real(),2.0) + (z.real()/2) + pow(z.imag(),2.0) - .1875) < pow(z.imag(),2.0)/4 ||
                     pow(z.real()+1,2) + pow(z.imag(),2) < .0625 )
                    n = maxiter;
                else
                    /* Applies the actual mandelbrot formula on that point */
                    for (n = 0; n <= maxiter && abs(z) < BAIL_OUT; n ++)
                        z = pow(z,2) + c;

                C = n - log2f(logf(abs(z)) / M_LN2 );

                /* Paint the pixel calculated depending on the number 
                   of iterations found */
                ((Uint32*)surface->pixels)[(y*surface->w) + x] = (n >= maxiter)? 0 :
                    SDL_MapRGB( surface->format,
                    (1+sin(C*0.27 + 5))*127., (1+cos(C*0.85))*127., (1+sin(C*0.15))*127. );
            }
            rects[y/FLIPS].x = 0;
            rects[y/FLIPS].y = y;
            rects[y/FLIPS].w = WIDTH;
            rects[y/FLIPS].h = 1;
        }
        
        SDL_UpdateWindowSurface(window);
    }
}

int main(int argc, char **argv)
{
    /* SDL SEtup */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_Window *window;

    window = SDL_CreateWindow("SDL Mandelbrot",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_OPENGL
            );

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    /* Initialize variables */
    complex<double> center = START_POS;
    double zoom = START_ZOOM;

    sdl_draw_mandelbrot(window, surface, center, zoom);

    SDL_Event event;
    while(1)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == ' ')
                {
                    center = START_POS;
                    zoom = START_ZOOM;
                    sdl_draw_mandelbrot(window, surface, center, zoom);
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                center = center.real() + ((event.button.x - (WIDTH/2))/zoom) +
                         (center.imag() + ((event.button.y - (HEIGHT/2))/zoom )) * Complex_I;

                if (event.button.button == 1)
                    zoom *= ZOOM_FACTOR;
                else if (event.button.button == 3)
                    zoom /= ZOOM_FACTOR;

                sdl_draw_mandelbrot(window, surface, center, zoom);
                break;
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

