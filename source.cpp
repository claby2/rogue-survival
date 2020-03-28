#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>
#include <cmath>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int SCREEN_FPS = 60;

const int WINDOW_SIZE = 15; // 15x15

const int SPRITE_SIZE = std::min(SCREEN_WIDTH, SCREEN_HEIGHT)/16;

const int WORLD_OFFSET = ceil(WINDOW_SIZE/2);
const int WORLD_SIZE = 100;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class LTexture {
	public:
		LTexture() {
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }
		~LTexture() {
            free();
        }
		bool loadFromFile( std::string path ) {
            free();
            SDL_Texture* newTexture = NULL;
            SDL_Surface* loadedSurface = IMG_Load(path.c_str());
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
            newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
            SDL_FreeSurface(loadedSurface);
            mTexture = newTexture;
            return mTexture != NULL;
        }
        // bool loadFromRenderedText(std::string textureText, SDL_Color textColor) {
        //     free();
        //     SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
        //     mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        //     mWidth = textSurface->w;
        //     mHeight = textSurface->h;
        //     SDL_FreeSurface(textSurface);
        //     return mTexture != NULL;
        // }
		void free() {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
            if(mTexture != NULL){
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
                mWidth = 0;
                mHeight = 0;
            }
        }
		void setColor( Uint8 red, Uint8 green, Uint8 blue ) {
            SDL_SetTextureColorMod(mTexture, red, green, blue);
        }
		void setBlendMode( SDL_BlendMode blending ) {
            SDL_SetTextureBlendMode(mTexture, blending);
        }
		void setAlpha( Uint8 alpha ) {
            SDL_SetTextureAlphaMod(mTexture, alpha);
        }
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE ){
            //RENDERS ONLY SPRITES
            SDL_Rect renderQuad = { x, y, mWidth, mHeight };
            if( clip != NULL )
            {
                renderQuad.w = SPRITE_SIZE;
                renderQuad.h = SPRITE_SIZE;
            }

            SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
        }
		int getWidth(){
            return mWidth;
        }
		int getHeight(){
            return mHeight;
        }

	private:
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};

LTexture gPlayerTexture;
LTexture gTileTexture;

SDL_Rect gSpriteClips[9];
LTexture gSpriteSheetTexture;

class Player {
    public:
        int curPosX = 30;
        int curPosY = 30;
        float mPosX = (SCREEN_WIDTH/2)-(SPRITE_SIZE/2);
        float mPosY = (SCREEN_HEIGHT/2)-(SPRITE_SIZE/2);
        int direction = 1; // 0 is left, 1 is right

        Player() {
            //Think of something to initialize
            //-> nah
        }

        void move(bool playerUp, bool playerDown, bool playerLeft, bool playerRight){
            if(playerUp){
                if(curPosY > 0) --curPosY;
            }
            if(playerDown){
                if(curPosY < WORLD_SIZE-1) ++curPosY;
            }
            if(playerLeft){
                if(curPosX > 0)--curPosX; direction = 0;
            }
            if(playerRight){
                if(curPosX < WORLD_SIZE-1)++curPosX; direction = 1;
            }
        }

        void render() {
            gSpriteSheetTexture.render(mPosX, mPosY, &gSpriteClips[8]);
        }
};

// class Tile {
//     public:
//         float mPosX;
//         float mPosY;
//         int tileType;
    
//     Tile(int type, int x, int y) { //Floor tile: pass in random
//         tileType = type;
//         mPosX = x;
//         mPosY = y;
//     }


        
//     void render() {
//         gSpriteSheetTexture.render(mPosX, mPosY, &gSpriteClips[tileType]);
//     }
// }

bool loadMedia(){
    bool success = true;

    if(!gSpriteSheetTexture.loadFromFile("images/8x.png")){
        printf("Failed to load sprite sheet texture!\n");
    } else {
        gSpriteClips[0].x = 56;
        gSpriteClips[0].y = 85;
        gSpriteClips[0].w = 8;
        gSpriteClips[0].h = 8;

        gSpriteClips[1].x = 65;
        gSpriteClips[1].y = 85;
        gSpriteClips[1].w = 8;
        gSpriteClips[1].h = 8;

        gSpriteClips[2].x = 74;
        gSpriteClips[2].y = 85;
        gSpriteClips[2].w = 8;
        gSpriteClips[2].h = 8;

        gSpriteClips[3].x = 56;
        gSpriteClips[3].y = 94;
        gSpriteClips[3].w = 8;
        gSpriteClips[3].h = 8;

        gSpriteClips[4].x = 74;
        gSpriteClips[4].y = 94;
        gSpriteClips[4].w = 8;
        gSpriteClips[4].h = 8;

        gSpriteClips[5].x = 56;
        gSpriteClips[5].y = 103;
        gSpriteClips[5].w = 8;
        gSpriteClips[5].h = 8;

        gSpriteClips[6].x = 65;
        gSpriteClips[6].y = 103;
        gSpriteClips[6].w = 8;
        gSpriteClips[6].h = 8;

        gSpriteClips[7].x = 74;
        gSpriteClips[7].y = 103;
        gSpriteClips[7].w = 8;
        gSpriteClips[7].h = 8;

        //PLAYER
        gSpriteClips[8].x = 12;
        gSpriteClips[8].y = 1;
        gSpriteClips[8].w = 7;
        gSpriteClips[8].h = 9;
    }
    return success;
}

void close() {
    gPlayerTexture.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool init() {
	bool success = true;
	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if(gWindow == NULL) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if(gRenderer == NULL) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

                if(TTF_Init() == -1){
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
			}
		}
	}
	return success;
}

void createWindow(int windowMap[][WINDOW_SIZE], int xx, int yy, int tileMap[][WORLD_SIZE+(2*WORLD_OFFSET)]){ //Flag top left tile
    for(int i = 0; i < WINDOW_SIZE; i++){
        for(int j = 0; j < WINDOW_SIZE; j++){
            windowMap[i][j] = tileMap[yy+i][xx+j];
        }
    }
}


int main(int argc, char* args[]){
    srand((unsigned)time(NULL));
    if(!init()){
        printf("Failed to initialize!\n");
    } else if(!loadMedia()){
        printf("Failed to load media!\n");
    } else {

        Player player;

        int tileMap[WORLD_SIZE+(2*WORLD_OFFSET)][WORLD_SIZE+(2*WORLD_OFFSET)];

        for(int i = 0; i < WORLD_OFFSET; i++){
            for(int j = 0; j < WORLD_SIZE + (2*WORLD_OFFSET); j++){
                tileMap[i][j] = -1;
            }
        }

        for(int i = WORLD_OFFSET; i < WORLD_SIZE+WORLD_OFFSET; i++){
            for(int j = 0; j < WORLD_OFFSET; j++){
                tileMap[i][j] = -1;
            }
            for(int j = WORLD_OFFSET+WORLD_SIZE; j < WORLD_SIZE+(2*WORLD_OFFSET); j++){
                tileMap[i][j] = -1;
            }
        }

        for(int i = WORLD_OFFSET+WORLD_SIZE; i < WORLD_SIZE+(2*WORLD_OFFSET); i++) {
            for(int j = 0; j < WORLD_SIZE + (2*WORLD_OFFSET); j++){
                tileMap[i][j] = -1;
            }
        }

        for(int i = WORLD_OFFSET; i < WORLD_SIZE+WORLD_OFFSET; i++){
            for(int j = WORLD_OFFSET; j < WORLD_SIZE+WORLD_OFFSET; j++){
                tileMap[i][j] = rand()%8;
            }
        }


        for(int i = 0; i < WORLD_SIZE+(2*WORLD_OFFSET); i++){
            for(int j = 0; j < WORLD_SIZE+(2*WORLD_OFFSET); j++){
                std::cout << tileMap[i][j];
            }
            std::cout << "\n";
        }



        int windowMap[WINDOW_SIZE][WINDOW_SIZE];

        bool quit = false;
        SDL_Event e;

        bool playerUp = false;
        bool playerDown = false;
        bool playerLeft = false;
        bool playerRight = false;
        
        while(!quit){
            while( SDL_PollEvent( &e ) != 0 ) {
                if( e.type == SDL_QUIT ) {
                    quit = true;
                } else if(e.type == SDL_KEYDOWN){
                    switch(e.key.keysym.sym){
                        case SDLK_t: for(int i = 0; i < WINDOW_SIZE; i++){for(int j = 0; j < WINDOW_SIZE; j++){std::cout << windowMap[i][j] << " ";}std::cout << "\n";}; std::cout << "--";break;
                    }
                }

                if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
                    switch(e.key.keysym.sym){
                        case SDLK_w: if(player.curPosY > 0) playerUp = true; break;
                        case SDLK_s: if(player.curPosY < WORLD_SIZE-1) playerDown = true; break;
                        case SDLK_a: if(player.curPosX > 0) playerLeft = true; break;
                        case SDLK_d: if(player.curPosX < WORLD_SIZE-1) playerRight = true; break;
                    }
                } else if(e.type == SDL_KEYUP && e.key.repeat == 0){
                    switch(e.key.keysym.sym){
                        case SDLK_w: playerUp = false; break;
                        case SDLK_s: playerDown = false; break;
                        case SDLK_a: playerLeft = false; break;
                        case SDLK_d: playerRight = false; break;
                    }
                }
            }
            player.move(playerUp, playerDown, playerLeft, playerRight);

            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            createWindow(windowMap, player.curPosX, player.curPosY, tileMap);

            for(int i = 0; i < WINDOW_SIZE; i++){
                for(int j = 0; j < WINDOW_SIZE; j++){
                    if(windowMap[i][j] != -1){
                        gSpriteSheetTexture.render(j*SPRITE_SIZE+(SPRITE_SIZE/2), i*SPRITE_SIZE+(SPRITE_SIZE/2), &gSpriteClips[windowMap[i][j]]);
                    }
                }
            }

            // gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);

            player.render();

            //Render

            SDL_RenderPresent(gRenderer);
            // std::cout << player.curPosX << " " << player.curPosY << "\n";
        }
    }
    close();

    return 0;
}