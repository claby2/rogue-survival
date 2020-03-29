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
#include <chrono>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 900;

const int WINDOW_SIZE = 15; // 15x15

const int SPRITE_SIZE_WIDTH = SCREEN_WIDTH/WINDOW_SIZE;
const int SPRITE_SIZE_HEIGHT = SCREEN_HEIGHT/WINDOW_SIZE;

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
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));
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
                renderQuad.w = SPRITE_SIZE_WIDTH;
                renderQuad.h = SPRITE_SIZE_HEIGHT;
            }

            SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
        }
        void renderProjectile( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE ){
            //RENDERS ONLY PROJECTILES
            SDL_Rect renderQuad = { x, y, mWidth, mHeight };
            if( clip != NULL )
            {
                renderQuad.w = SPRITE_SIZE_WIDTH/2;
                renderQuad.h = SPRITE_SIZE_HEIGHT/2;
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

SDL_Rect gSpriteClips[100];
LTexture gSpriteSheetTexture;

class Player {
    public:
        float curPosX = 10;
        float curPosY = 10;
        float mPosX = (SCREEN_WIDTH/2)-(SPRITE_SIZE_WIDTH/2);
        float mPosY = (SCREEN_HEIGHT/2)-(SPRITE_SIZE_HEIGHT/2);
        float vel = 0.1;
        int direction = 1; // 0 is left, 1 is right
        int state = 8; // 8 is normal, 9 is attack

        Player() {
            //Think of something to initialize
            //-> nah
        }

        void move(bool playerUp, bool playerDown, bool playerLeft, bool playerRight){
            if((playerUp && playerLeft) || (playerUp && playerRight) || (playerDown && playerLeft) || (playerDown && playerRight)){
                vel = 0.05;
                if(playerUp){
                    if(playerLeft){
                        if(curPosY > 0) curPosY -= vel;
                        if(curPosX > 0) curPosX -= vel; direction = 0;
                    }
                    if(playerRight){
                        if(curPosY > 0) curPosY -= vel;
                        if(curPosX < WORLD_SIZE-1) curPosX += vel; direction = 1;
                    }
                }   
                if(playerDown){
                    if(playerLeft){
                        if(curPosY < WORLD_SIZE-1) curPosY += vel;
                        if(curPosX > 0) curPosX -= vel; direction = 0;
                    }
                    if(playerRight){
                        if(curPosY < WORLD_SIZE-1) curPosY += vel;  
                        if(curPosX < WORLD_SIZE-1) curPosX += vel; direction = 1;
                    }
                }
            } else {
                vel = 0.1;
                if(playerUp){
                    if(curPosY > 0) curPosY -= vel;
                }
                if(playerDown){
                    if(curPosY < WORLD_SIZE-1) curPosY += vel;
                }
                if(playerLeft){
                    if(curPosX > 0) curPosX -= vel; direction = 0;
                }
                if(playerRight){
                    if(curPosX < WORLD_SIZE-1) curPosX += vel; direction = 1;
                }
            }
        }

        void render() {
            if(direction == 0){
                gSpriteSheetTexture.render(mPosX, mPosY, &gSpriteClips[state], 0.0, NULL, SDL_FLIP_HORIZONTAL);
            } else {
                gSpriteSheetTexture.render(mPosX, mPosY, &gSpriteClips[state], 0.0, NULL, SDL_FLIP_NONE);
            }
        }
};

class Projectile {
    public:
        static const int PROJECTILE_WIDTH = SPRITE_SIZE_WIDTH/2;
        static const int PROJECTILE_HEIGHT = SPRITE_SIZE_HEIGHT/2;
        static const int PROJECTILE_VEL = 10;
        float mPosX, mPosY;

    Projectile(int mouseX, int mouseY){
        mPosX = SCREEN_WIDTH/2 - PROJECTILE_WIDTH/2;
        mPosY = SCREEN_HEIGHT/2 - PROJECTILE_HEIGHT/2;

        float Dlen = sqrt(((mPosX - mouseX) * (mPosX - mouseX)) + ((mPosY - mouseY) * (mPosY - mouseY)));
        mVelX = (mPosX - mouseX)/Dlen;
        mVelY = (mPosY - mouseY)/Dlen;
    }

    void move() {
        mPosX += -mVelX*PROJECTILE_VEL;
        mPosY += -mVelY*PROJECTILE_VEL;
    }


    bool invalidPos(int windowMap[][WINDOW_SIZE]) {
        int x, y;
        x = (int)(ceil(mPosX/SPRITE_SIZE_WIDTH));
        y = (int)(ceil(mPosY/SPRITE_SIZE_HEIGHT));
        if(((mPosX < -PROJECTILE_WIDTH) || mPosX > SCREEN_WIDTH || mPosY < -PROJECTILE_HEIGHT || mPosY > SCREEN_HEIGHT) || windowMap[y-1][x-1] >= 10 && windowMap[y-1][x-1] <= 13){
            return true;
        }
        return false;
    }

    void shift(int tx, int ty, int nx, int ny){;
        int dx = nx-tx; //Positive means gone right
        int dy = ny-ty; //Negative means gone left

        if(dx > 0){ //Gone Right, Move Left
            mPosX -= (dx*SPRITE_SIZE_WIDTH);
        } else if(dx < 0){
            mPosX += (abs(dx)*SPRITE_SIZE_WIDTH);
        }

        if(dy > 0){ //Gone Down, Move Up
            mPosY -= (dy*SPRITE_SIZE_HEIGHT);
        } else if(dy < 0){
            mPosY += (abs(dy)*SPRITE_SIZE_HEIGHT);
        }
    }

    void render() {
        gSpriteSheetTexture.renderProjectile(mPosX, mPosY, &gSpriteClips[14], 0.0, NULL, SDL_FLIP_NONE);
    }

    private:
        float mVelX, mVelY;
};

Projectile createProjectile(){
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Projectile p(mouseX, mouseY);
    return p;
}

bool loadMedia(){
    bool success = true;

    if(!gSpriteSheetTexture.loadFromFile("images/8xn.png")){
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

        //PLAYER DEFAULT
        gSpriteClips[8].x = 12;
        gSpriteClips[8].y = 1;
        gSpriteClips[8].w = 8;
        gSpriteClips[8].h = 9;

        // PLAYER ATTACK
        gSpriteClips[9].x = 12;
        gSpriteClips[9].y = 12;
        gSpriteClips[9].w = 8;
        gSpriteClips[9].h = 10;

        // OUTSIDE WALL 1
        gSpriteClips[10].x = 83;
        gSpriteClips[10].y = 94;
        gSpriteClips[10].w = 8;
        gSpriteClips[10].h = 8;

        // OUTSIDE WALL 2
        gSpriteClips[11].x = 92;
        gSpriteClips[11].y = 94;
        gSpriteClips[11].w = 8;
        gSpriteClips[11].h = 8;

        // OBJECT WALL 1
        gSpriteClips[12].x = 83;
        gSpriteClips[12].y = 85;
        gSpriteClips[12].w = 8;
        gSpriteClips[12].h = 8;

        // OBJECT WALL 2
        gSpriteClips[13].x = 92;
        gSpriteClips[13].y = 85;
        gSpriteClips[13].w = 8;
        gSpriteClips[13].h = 8;

        // PROJECTILE
        gSpriteClips[14].x = 31;
        gSpriteClips[14].y = 56;
        gSpriteClips[14].w = 4;
        gSpriteClips[14].h = 4;
    }
    return success;
}

void close() {
    gSpriteSheetTexture.free();
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
		gWindow = SDL_CreateWindow( "Accolade", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
                tileMap[i][j] = rand()%2+10;
            }
        }
        for(int i = WORLD_OFFSET; i < WORLD_SIZE+WORLD_OFFSET; i++){
            for(int j = 0; j < WORLD_OFFSET; j++){
                tileMap[i][j] = rand()%2+10;
            }
            for(int j = WORLD_OFFSET+WORLD_SIZE; j < WORLD_SIZE+(2*WORLD_OFFSET); j++){
                tileMap[i][j] = rand()%2+10;
            }
        }
        for(int i = WORLD_OFFSET+WORLD_SIZE; i < WORLD_SIZE+(2*WORLD_OFFSET); i++) {
            for(int j = 0; j < WORLD_SIZE + (2*WORLD_OFFSET); j++){
                tileMap[i][j] = rand()%2+10;
            }
        }

        for(int i = WORLD_OFFSET; i < WORLD_SIZE+WORLD_OFFSET; i++){
            for(int j = WORLD_OFFSET; j < WORLD_SIZE+WORLD_OFFSET; j++){
                int p = rand()%100;
                if(p >=0 && p <= 3){
                    tileMap[i][j] = rand()%2+12;
                } else {
                    tileMap[i][j] = rand()%8;
                }
            }
        }

        // for(int i = 0; i < WORLD_SIZE+(2*WORLD_OFFSET); i++){
        //     for(int j = 0; j < WORLD_SIZE+(2*WORLD_OFFSET); j++){
        //         std::cout << tileMap[i][j];
        //     }
        //     std::cout << "\n";
        // }

        int windowMap[WINDOW_SIZE][WINDOW_SIZE];

        bool quit = false;
        SDL_Event e;

        bool playerUp = false;
        bool playerDown = false;
        bool playerLeft = false;
        bool playerRight = false;

        int playerAttack = 0;
        int fireRate = 1;
        std::vector<Projectile> projectiles;

        int cx = floor(WINDOW_SIZE/2); //centerx
        int cy = floor(WINDOW_SIZE/2); //centery

        int tx = player.curPosX;
        int ty = player.curPosY;
        
        while(!quit){
            for(int i = 0; i < projectiles.size(); i++){
                projectiles[i].shift(tx, ty, round(player.curPosX), round(player.curPosY));
            }
            createWindow(windowMap, round(player.curPosX), round(player.curPosY), tileMap);
            tx = round(player.curPosX);
            ty = round(player.curPosY);
            auto start = std::chrono::high_resolution_clock::now();
            if(playerAttack){
                playerAttack--;
                if(playerAttack == 0){
                    player.state = 8;
                }
            }
            while(SDL_PollEvent( &e ) != 0) {
                if( e.type == SDL_QUIT ) {
                    quit = true;
                } else if(e.type == SDL_KEYDOWN){
                    switch(e.key.keysym.sym){
                        case SDLK_t: for(int i = 0; i < WINDOW_SIZE; i++){for(int j = 0; j < WINDOW_SIZE; j++){std::cout << windowMap[i][j] << " ";}std::cout << "\n";}; std::cout << "--";break;
                    }
                }
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && playerAttack == 0){
                    playerAttack = fireRate;
                    player.state = 9;
                    projectiles.push_back(createProjectile());
                }

                if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
                    switch(e.key.keysym.sym){
                        case SDLK_w: 
                            if(player.curPosY > 0){
                                playerUp = true; 
                            }break;
                        case SDLK_s: 
                            if(player.curPosY < WORLD_SIZE-1){
                                playerDown = true;
                            }break;
                        case SDLK_a:
                            if(player.curPosX > 0){
                                playerLeft = true; 
                            }break;
                        case SDLK_d: 
                            if(player.curPosX < WORLD_SIZE-1){
                                playerRight = true; 
                            }break;
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

            if(playerUp && (windowMap[cy-1][cx] == 12 || windowMap[cy-1][cx] == 13)){
                playerUp = false;
            }
            if(playerDown && (windowMap[cy+1][cx] == 12 || windowMap[cy+1][cx] == 13)){
                playerDown = false;
            }
            if(playerLeft && (windowMap[cy][cx-1] == 12 || windowMap[cy][cx-1] == 13)){
                playerLeft = false;
            }
            if(playerRight && (windowMap[cy][cx+1] == 12 || windowMap[cy][cx+1] == 13)){
                playerRight = false;
            }
            player.move(playerUp, playerDown, playerLeft, playerRight);

            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(gRenderer);

            for(int i = 0; i < WINDOW_SIZE; i++){
                for(int j = 0; j < WINDOW_SIZE; j++){
                    gSpriteSheetTexture.render(j*SPRITE_SIZE_WIDTH, i*SPRITE_SIZE_HEIGHT, &gSpriteClips[windowMap[i][j]]);
                }
            }

            for(int i = 0; i < projectiles.size(); i++){
                projectiles[i].move();
                if(projectiles[i].invalidPos(windowMap)){
                    projectiles.erase(projectiles.begin()+i);
                } else {
                    projectiles[i].render();
                }
            }

            player.render();


            SDL_RenderPresent(gRenderer);
            // std::cout << player.curPosX << " " << player.curPosY << "\n";
            double time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-start).count();
            // std::cout << 1/(time/1000000)*1000 << "\n";
        }
    }
    close();

    return 0;
}