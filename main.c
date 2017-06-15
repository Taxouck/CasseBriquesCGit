#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define SCREEN_WIDTH 864
#define SCREEN_HEIGHT 600


//Option de build dans les linker setting mettre les libSDL2main.a libSDL2.dll.a
//Dans search directory
//Option de build compiler mettre les include
//Option de build linker mettre les lib

//mettre dans build linker -lmingw32

typedef struct game{


     SDL_Window *g_pWindow;
     SDL_Renderer *g_pRenderer;
     SDL_Texture *g_texture;
     SDL_Surface *g_surface;


}game;

typedef struct gameState{

    int g_bRunning;
    int left;
    int right;


}gameState;


typedef struct raquette{

    double position;
    double vitesse;
    int vies;

}raquette;

typedef struct brique{

    int pv;
    int color;

}brique;

typedef struct niveau{

    brique grille[13][15];

}niveau;

typedef struct balle{

    double x;
    double y;
    double sx;
    double sy;
    double speed;
    int attache;

}balle;

int init(char *title, int xpos,int ypos,int height, int width,int flags,game *myGame);
void initNiveau(niveau *niveau, int which);
void handleRaquette(game *myGame, gameState *state, raquette *joueur, int *pressstate, balle *balle);
void handleNiveau(game *myGame, gameState *state, niveau niveau);
void handleBalle(game *myGame, gameState *state, niveau *niveau, raquette *joueur, balle *balle);
void Affichage(game *myGame,gameState state);
void delay(unsigned int frameLimit);
void destroyTexture(game *myGame);
void destroy(game *myGame);



int main(int argc, char *argv[])
{

     game myGame;
     gameState state;
     niveau niveau;
     raquette joueur;
     balle balle;
     balle.attache=1;
     joueur.position=(SCREEN_WIDTH/2)-64;
     joueur.vies=3;
     joueur.vitesse=0;
     int choixniveau=1;



    //Pour les 60 fps
    unsigned int frameLimit = SDL_GetTicks() + 16;


    if(init("Chapter 1 setting up SDL",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN,&myGame)){

                state.g_bRunning=1;


    }else{

            return 1;//something's wrong

    }


    state.right=1;
    state.left=0;
    int menu=1;
    int pressstate=0;


    while(state.g_bRunning){

        initNiveau(&niveau,choixniveau); //TODO: Choix du niveau
        menu=0;

        while (menu==0){
           handleRaquette(&myGame,&state,&joueur, &pressstate, &balle);
           handleNiveau(&myGame,&state,niveau);
           handleBalle(&myGame,&state,&niveau,&joueur,&balle);
           if(joueur.vies>=0){
           Affichage(&myGame,state);
           }else{
               destroy(&myGame);
               SDL_Quit(); //TODO: Afficher game over avant de quitter/relancer le jeu
               return 0;
           }


            // Gestion des 60 fps (1000ms/60 = 16.6 -> 16
            delay(frameLimit);
            frameLimit = SDL_GetTicks() + 16;
        }


    }

        destroy(&myGame);

        SDL_Quit();




  return 0;
}


int init(char *title, int xpos,int ypos,int height, int width,int flags,game *myGame){


    //initialize SDL

    if(SDL_Init(SDL_INIT_EVERYTHING)>=0)
    {
            //if succeeded create our window
            myGame->g_pWindow=SDL_CreateWindow(title,xpos,ypos,height,width,flags);
            //if succeeded create window, create our render
            if(myGame->g_pWindow!=NULL){
                myGame->g_pRenderer=SDL_CreateRenderer(myGame->g_pWindow,-1,SDL_RENDERER_ACCELERATED);
                SDL_SetRenderDrawColor(myGame->g_pRenderer, 125, 0, 125, 255);
                SDL_RenderClear(myGame->g_pRenderer);

            }


    }else{


        return 0;
    }




    return 1;


}

void initNiveau(niveau *niveau, int which){
    int i,j;
    switch(which){
        case 1: for (i=0;i<13;i++){ for (j=0;j<15;j++){
                    niveau->grille[i][j].color=((i+j)%4)+1;
                    niveau->grille[i][j].pv=1;
                }};
    }
}

void handleNiveau(game *myGame, gameState *state, niveau niveau){
    //SDL_Event event;

    int i,j;
    SDL_Rect rectangleDest;
    SDL_Rect rectangleSource;
    myGame->g_surface = IMG_Load("./assets/bricks.png");//Chargement de l'image png

    if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        if(myGame->g_surface){


                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation du sprite
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                if(myGame->g_texture){
                    for (i=0;i<13;i++){ for (j=0;j<15;j++){

                        switch (niveau.grille[i][j].color)
                            {
                                case 1: rectangleSource.x=0;
                                        rectangleSource.y=0;
                                        break;
                                case 2: rectangleSource.x=64;
                                        rectangleSource.y=0;
                                        break;
                                case 3: rectangleSource.x=0;
                                        rectangleSource.y=24;
                                        break;
                                case 4: rectangleSource.x=64;
                                        rectangleSource.y=24;
                                        break;

                            }

                    rectangleSource.w=64;
                    rectangleSource.h=24;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    //Définition du rectangle dest pour dessiner Bitmap
                    if (niveau.grille[i][j].pv>=1){
                    rectangleDest.x=(i*64)+16;//debut x
                    rectangleDest.y=(j*24);//debut y
                    rectangleDest.w=rectangleSource.w; //Largeur
                    rectangleDest.h=rectangleSource.h; //Hauteur

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);
                    }
                    }}
                }
                destroyTexture(myGame);
        }
}

void handleRaquette(game *myGame, gameState *state, raquette *joueur, int *pressstate, balle *balle){


    SDL_Event event;

    if(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
              state->g_bRunning=0;break;
        case SDL_KEYDOWN:
                        switch (event.key.keysym.sym)
                            {
                                /*case SDLK_LEFT:joueur->position=joueur->position-10; if(joueur->position<=16){joueur->position=16;} break;
                                case SDLK_RIGHT:joueur->position=joueur->position+10 ; if(joueur->position>=SCREEN_WIDTH-144){joueur->position=SCREEN_WIDTH-144;}break;
                                case SDLK_SPACE:break;*/
                                /*case SDLK_LEFT:joueur->vitesse=joueur->vitesse-3; break;
                                case SDLK_RIGHT:joueur->vitesse=joueur->vitesse+3; break;
                                case SDLK_SPACE:break;*/
                                case SDLK_LEFT:if(*pressstate!=1){*pressstate=1;} break;
                                case SDLK_RIGHT:if(*pressstate!=2){*pressstate=2;} break;
                                case SDLK_SPACE:if(balle->attache=1){balle->attache=0; balle->sy=-3; balle->sx=0.1;} break;
                            }
                            break;

        case SDL_KEYUP:
                        switch (event.key.keysym.sym)
                            {
                                case SDLK_LEFT:if(*pressstate==1){*pressstate=0;} break;
                                case SDLK_RIGHT:if(*pressstate==2){*pressstate=0;} break;
                                case SDLK_SPACE:break;
                            }
                            break;

        default:break;

        }


    }
    switch(*pressstate){
    case 0:if(joueur->vitesse>=1){joueur->vitesse=joueur->vitesse-3;}
           if(joueur->vitesse<=-1){joueur->vitesse=joueur->vitesse+3;}
           if(joueur->vitesse<=1.5 && joueur->vitesse>=-1.5){joueur->vitesse=0;}break;
    case 1:joueur->vitesse=joueur->vitesse-1.5; break;
    case 2:joueur->vitesse=joueur->vitesse+1.5; break;
    }
    joueur->position=joueur->position+joueur->vitesse;
        if(joueur->position<=16){joueur->position=16; joueur->vitesse=0;}
        if(joueur->position>=SCREEN_WIDTH-144){joueur->position=SCREEN_WIDTH-144; joueur->vitesse=0;}


    SDL_Rect rectangleDest;
    SDL_Rect rectangleSource;
    myGame->g_surface = IMG_Load("./assets/paddle.png");//Chargement de l'image png

    if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        if(myGame->g_surface){


                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation du sprite
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                if(myGame->g_texture){

                    rectangleSource.x=0;//128*(int)((SDL_GetTicks()/100)%6);
                    rectangleSource.y=0;
                    rectangleSource.w=128;
                    rectangleSource.h=32;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    //Définition du rectangle dest pour dessiner Bitmap
                    rectangleDest.x=joueur->position;//debut x
                    rectangleDest.y=SCREEN_HEIGHT-64;//debut y
                    rectangleDest.w=rectangleSource.w; //Largeur
                    rectangleDest.h=rectangleSource.h; //Hauteur

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);
                }
                destroyTexture(myGame);
        }


}

void handleBalle(game *myGame, gameState *state, niveau *niveau, raquette *joueur, balle *balle){

    if(balle->attache==1){
        balle->x=joueur->position+52;
        balle->y=SCREEN_HEIGHT-88;
    }else{
        if( ((int)((balle->y+12)/24)<15) && ((int)((balle->x-4)/64)<13) ){
                if(niveau->grille[(int)((balle->x-4)/64)][(int)((balle->y+12)/24)].pv>=1){ //Si la balle cogne une brique...

                    /*if( ((((int)balle->x-4)%64>=0) && (((int)balle->x-4)%64<=balle->sx+1)) && (balle->sx>0) || ((((int)balle->x+4)%64<=64) && (((int)balle->x+4)%64>=64-(abs(balle->sx)+1))) && (balle->sx<0) ){ //...Depuis le côté...
                        balle->sx=balle->sx*(-1); //...Inverser le mouvement horizontal...
                        if( ((int)balle->y+12)%24<=2 ){ //...Si même depuis un coin...
                            balle->sy=abs(balle->sy*(1.05)); //...Inverser le mouvement vertical aussi, tout en l'accélérant...
                        }else{
                        if( ((int)balle->y+12)%24>=22 ){
                            balle->sy=-abs(balle->sy*(1.05));
                        }else{
                            balle->sy=balle->sy*(1.05); //...Si pas depuis un coin, seulement l'accélérer...
                        }}
                        if(balle->sy>9.0){balle->sy=9;}
                        if(balle->sy<-9.0){balle->sy=-9;}
                    }else{ //...Si pas depuis un côté...
                        balle->sy=balle->sy*(-1.05); //...Inverser et accélérer le mouvement vertical.
                        if(balle->sy>9.0){balle->sy=9;} //Si le mouvement vertical en devient trop grand, le mettre à son max.
                        if(balle->sy<-9.0){balle->sy=-9;}
                    }*/
                    if( (balle->x-4)<=((int)((balle->x-4)/64))*64+18 && abs(balle->sx)>=1 ){ //Si cogne de la gauche...
                        balle->sx=-abs(balle->sx); //...Envoyer le mouvement horizontal vers la gauche...
                    }else{
                    if( (balle->x-4)>=((int)((balle->x-4)/64))*64+78 && abs(balle->sx)>=1 ){ //Si cogne de la droite...
                        balle->sx=abs(balle->sx); //...Envoyer le mouvement horizontal vers la droite...
                    }else{
                    if( (balle->y+12) <= (int)((balle->y+12)/24)*24+12 ){ //Si cogne du haut... (balle->y+12)<=(int)((balle->y+12)/24)*24
                        balle->sy=-abs(balle->sy); //...Envoyer le mouvement vertical vers le haut...
                    }
                    if( (balle->y+12) >= (int)((balle->y+12)/24)*24+12 ){ //Si cogne du bas... (balle->y+12)<=(int)((balle->y+12)/24)*24+24
                        balle->sy=abs(balle->sy); //...Envoyer le mouvement vertical vers le bas...
                    }}}
                    balle->sy=balle->sy*(1.1); //...Accélérer le mouvement vertical.
                    if(balle->sy>10.0){balle->sy=10;} //Si le mouvement vertical en devient trop grand, le mettre à son max.
                    if(balle->sy<-10.0){balle->sy=-10;}
                    niveau->grille[(int)((balle->x-4)/64)][(int)((balle->y+12)/24)].pv=niveau->grille[(int)((balle->x-4)/64)][(int)((balle->y+12)/24)].pv-1; //Et la brique perd un PV.
                }
        }
        balle->x=balle->x+balle->sx; //La balle change de position horizontale...
        balle->y=balle->y+balle->sy; //...Et verticale.
        if (balle->y>=SCREEN_HEIGHT){  //Si la balle cogne le sol...
            balle->attache=1; //Perdre une vie et recoller la balle à la raquette.
            joueur->vies=joueur->vies-1;
        }
        if (balle->y<=0){ //Si la balle cogne un des trois autres murs, inverser le mouvement correspondant.
            balle->sy=-balle->sy;
            balle->y=1;
        }
        if (balle->x<=16){
            balle->sx=-balle->sx;
            balle->x=17;
        }
        if (balle->x>=SCREEN_WIDTH-40){
            balle->sx=-balle->sx;
            balle->x=SCREEN_WIDTH-41;
        }
        if ( (balle->y>=SCREEN_HEIGHT-86) && (balle->y<=SCREEN_HEIGHT-44) && (balle->x+12>=joueur->position) && (balle->x+12<=joueur->position+128) && (balle->sy>0) ){ //Si la balle cogne la raquette...
            balle->sy=-balle->sy; //...Inverser le mouvement vertical...
            balle->sx=((balle->x)-((joueur->position+64)))/6; //...Et changer le mouvement horizontal selon l'angle de contact.
        }
    };

    SDL_Rect rectangleDest;
    SDL_Rect rectangleSource;
    myGame->g_surface = IMG_Load("./assets/ball.png");//Chargement de l'image png

    if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        if(myGame->g_surface){


                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation du sprite
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                if(myGame->g_texture){

                    rectangleSource.x=0;//128*(int)((SDL_GetTicks()/100)%6);
                    rectangleSource.y=0;
                    rectangleSource.w=24;
                    rectangleSource.h=24;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    //Définition du rectangle dest pour dessiner Bitmap
                    rectangleDest.x=balle->x;//debut x
                    rectangleDest.y=balle->y;//debut y
                    rectangleDest.w=rectangleSource.w; //Largeur
                    rectangleDest.h=rectangleSource.h; //Hauteur

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);
                }
                destroyTexture(myGame);
        }


}



void Affichage(game *myGame,gameState state){

        SDL_Rect rectangleDest;
        SDL_Rect rectangleSource;
        SDL_Rect rectangleDest2;


        myGame->g_surface = IMG_Load("./assets/side.png");//Chargement de l'image png

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        if(myGame->g_surface){


                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation du sprite
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                if(myGame->g_texture){

                    rectangleSource.x=0;//128*(int)((SDL_GetTicks()/100)%6);
                    rectangleSource.y=0;
                    rectangleSource.w=16;
                    rectangleSource.h=600;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    //Définition du rectangle dest pour dessiner Bitmap
                    rectangleDest.x=0;//debut x
                    rectangleDest.y=0;//debut y
                    rectangleDest.w=rectangleSource.w; //Largeur
                    rectangleDest.h=rectangleSource.h; //Hauteur

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);

                    rectangleDest2.x=SCREEN_WIDTH-16;//debut x
                    rectangleDest2.y=0;//debut y
                    rectangleDest2.w=rectangleSource.w; //Largeur
                    rectangleDest2.h=rectangleSource.h; //Hauteur

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest2);

                    /*if(state.right)
                        SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest); // Copie du sprite grâce au SDL_Renderer
                    if(state.left)
                        SDL_RenderCopyEx(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest,0,0,SDL_FLIP_HORIZONTAL);*/

                    //SDL_FLIP_NONE
                    //SDL_FLIP_HORIZONTAL
                    //SDL_FLIP_VERTICAL



                    SDL_RenderPresent(myGame->g_pRenderer); // Affichage

                    SDL_RenderClear(myGame->g_pRenderer);

                }

                else{
                        fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
                }



        }else{
            fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
        }


    destroyTexture(myGame);

}

void delay(unsigned int frameLimit){
    // Gestion des 60 fps (images/seconde)
    unsigned int ticks = SDL_GetTicks();

    if (frameLimit < ticks)
    {
        return;
    }

    if (frameLimit > ticks + 16)
    {
        SDL_Delay(16);
    }

    else
    {
        SDL_Delay(frameLimit - ticks);
    }
}




void destroy(game *myGame){

    //Destroy render
    if(myGame->g_pRenderer!=NULL)
        SDL_DestroyRenderer(myGame->g_pRenderer);


    //Destroy window
    if(myGame->g_pWindow!=NULL)
        SDL_DestroyWindow(myGame->g_pWindow);

}



void destroyTexture(game *myGame){

    //Destroy texture
    if(myGame->g_texture!=NULL)
            SDL_DestroyTexture(myGame->g_texture);


}










