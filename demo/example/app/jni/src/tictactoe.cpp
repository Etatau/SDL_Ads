// internal header
#include <vector>

// SDL headers
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "SDL_Ads.hpp"

#define SCREEN_WIDTH 720 // 40
#define SCREEN_HEIGHT 1280 // 25

// Global control var
float touchx = 0, touchy = 0;
bool touched = false;

/*
(x1,y1)(x2,y1)
(x1,y2)(x2,y2)
*/
struct Rect {
	int x1, y1, x2, y2;
};

class Collider
{
public:
	/*
	entity1 refers to the entity which is going to collide
	entity2 refers to the entity with which entity1 will collide
	entity2 should preferably be static entity
	this collision check is mainly for entity1
	*/
	bool rect_collision(Rect rect1, Rect rect2)
	{
		if ((rect1.y1 >= rect2.y1 && rect1.y1 <= rect2.y2) && ((rect1.x1 >= rect2.x1 && rect1.x1 <= rect2.x2) || (rect1.x2 >= rect2.x1 && rect1.x2 <= rect2.x2)))
			return true;
		if ((rect1.y2 >= rect2.y1 && rect1.y2 <= rect2.y2) && ((rect1.x1 >= rect2.x1 && rect1.x1 <= rect2.x2) || (rect1.x2 >= rect2.x1 && rect1.x2 <= rect2.x2)))
			return true;
		return false;
	}

	bool point_rect_collision(int x, int y, Rect rect)
	{
		if ((x >= rect.x1 && x <= rect.x2) && (y >= rect.y1 && y <= rect.y2))
			return true;
		return false;
	}
}collider;

// global initialization of renderwindow at start
class RenderWindow
{
public:
	SDL_Window* window;
	SDL_Renderer* renderer;

	void init_renderer_window()
	{
		// error check
		SDL_Init(SDL_INIT_VIDEO);

		// create window
		window = SDL_CreateWindow("TEST", SCREEN_WIDTH, SCREEN_HEIGHT, 0);

		// create renderer
		renderer = SDL_CreateRenderer(window, NULL);
		SDL_SetRenderVSync(renderer, 1);
	}

	SDL_Texture* loadTexture(const char* p_filePath)
	{
		SDL_Texture* texture = NULL;
		SDL_IOStream* file = SDL_IOFromFile(p_filePath, "rb");
		if(file == NULL)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Asset not in path", SDL_GetError(), NULL);
		}
		texture = SDL_CreateTextureFromSurface(renderer, IMG_Load_IO(file, true));
		return texture;
	}

	void cleanup(void)
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void clear(void)
	{
		SDL_RenderClear(renderer);
	}

	void display(void) //render
	{
		SDL_RenderPresent(renderer);
	}
}window;

// sprite reference
SDL_Texture* button_sprite;

class TempButton
{
public:
    SDL_Texture* sprite;
    SDL_Rect rect;

    bool once = false;

    TempButton(SDL_Rect _rect, SDL_Texture* _sprite)
    {
        rect = _rect;
        sprite = _sprite;
    }

    void button()
    {
        if(touched && collider.point_rect_collision(touchx,touchy,{rect.x,rect.y,rect.x+rect.w,rect.y+rect.h}))
        {
            if(!once) {
                playAd();
                once = true;
            }
            return;
        }
        once = false;
    }
};

class objectRenderer
{
public:
    void render(SDL_Texture* sprite, SDL_Rect rect)
    {
        SDL_FRect temp_frect = {0};
        SDL_RectToFRect(&rect, &temp_frect);
        SDL_RenderTexture(window.renderer, sprite, NULL, &temp_frect);
    }
}renderobj;

int main(int argc, char* argv[])
{
    initializeAdSystem();

	window.init_renderer_window();
	button_sprite = window.loadTexture("button.png");

    TempButton newbutton = TempButton({300,200,128,128},button_sprite);

	SDL_Event event;
	while(true)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_EVENT_QUIT:
				exit(0);
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				touched = true;
				SDL_GetMouseState(&touchx, &touchy);
				break;
			}
		}

		// display logic is here since all logic has to be calculated before displaying the current frames
		window.clear();

        newbutton.button();
        renderobj.render(newbutton.sprite, newbutton.rect);

        window.display();

        touched = false;
        touchx = 0;
        touchy = 0;
	}

	window.cleanup();
	return 0;
}