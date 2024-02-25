#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL/SDL_getenv.h>
#include <fmodex/fmod.h>

#include "constantes.h"
#include "jeu.h"

int menuJouerQuitter(FMOD_SYSTEM *system);
int menuChoixJoueurs(SDL_Surface* ecran, FMOD_SYSTEM *system, FMOD_CHANNEL *musiqueDeFond);


typedef struct
    {
        char key[SDLK_LAST];
        char quit;
        char focus;
    } Input;


void UpdateEvents(Input* in)
{
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                in->key[event.key.keysym.sym]=1;
                break;
            case SDL_KEYUP:
                in->key[event.key.keysym.sym]=0;
                break;
            case SDL_QUIT:
                in->quit =1;
                break;
            default:
                break;
            }

            if  (
                    (
                        ((event.active.state & SDL_APPINPUTFOCUS) == SDL_APPINPUTFOCUS)
                     || ((event.active.state & SDL_APPACTIVE) == SDL_APPACTIVE)
                    )
                    && event.active.gain == 0
                )
                            in->focus =1;
        }
}


int main (int argc, char* argv[])
{
    SDL_Surface *ecran = NULL, *accueil = NULL, *pressEnter = NULL;
    SDL_Rect positionAccueil, positionPressEnter;
    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);

    // Son
    FMOD_SYSTEM *system;
    FMOD_System_Create(&system);
    FMOD_System_Init(system, 5, FMOD_INIT_NORMAL, NULL);

    FMOD_SOUND *letsgo;
    FMOD_System_CreateSound(system, "sons/letsgo.mp3", FMOD_CREATESAMPLE, 0, &letsgo);

    // Initialisation SDL
    putenv("SDL_VIDEO_WINDOW_POS=center");
    SDL_WM_SetIcon(SDL_LoadBMP("Donkey_Kong_icon.bmp"), NULL);

    ecran = SDL_SetVideoMode(450, 300, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("Donkey Kong Arcade", NULL);
    TTF_Init();

    int continuer = 1;
    int press = 0;

    accueil = SDL_LoadBMP("menus et fonds/Accueil.bmp");
    positionAccueil.x = 0;
    positionAccueil.y = 0;

    pressEnter = IMG_Load("menus et fonds/pressentree.png");
    positionPressEnter.x = 350;
    positionPressEnter.y = 215;


    while(continuer)
        {
            SDL_PollEvent(&event);
            switch(event.type)
            {
                case SDL_QUIT:
                    continuer = 0;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            continuer = 0;
                            break;

                        case SDLK_RETURN:
                            FMOD_System_PlaySound(system, 0, letsgo, 0, NULL);
                            SDL_Delay(1000);
                            if(menuJouerQuitter(system))
                                {
                                    TTF_Quit();
                                    SDL_FreeSurface(accueil);
                                    remove("screenshot.bmp");
                                    remove("Gagnant niveau 1.bmp");
                                    remove("Gagnant niveau 2.bmp");
                                    remove("Gagnant niveau 3.bmp");
                                    SDL_Quit;
                                }
                            break;

                        default:
                            break;
                    }
                    break;
            }

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
        SDL_BlitSurface(accueil, NULL, ecran, &positionAccueil);

        press++;
        if (press % 30 >= 15)
            SDL_BlitSurface(pressEnter, NULL, ecran, &positionPressEnter);

        SDL_Flip(ecran);
        SDL_Delay(50);

        }

    TTF_Quit();
    SDL_FreeSurface(accueil);
    SDL_FreeSurface(pressEnter);
    SDL_Quit();

    return EXIT_SUCCESS;
}


int menuJouerQuitter(FMOD_SYSTEM *system)
{
    SDL_Surface *ecran = NULL, *fond = NULL, *jouer = NULL, *quitter = NULL;
    SDL_Surface *transition[10] = {NULL};
    SDL_Rect positionFond, positionJouer, positionQuitter;

    // Son
    FMOD_SOUND *duketales;
    FMOD_System_CreateSound(system, "sons/duketales-stereo.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D | FMOD_CREATESTREAM, 0, &duketales);

    FMOD_Sound_SetLoopCount(duketales, -1);                         // On active la répétition de la musique à l'infini
	FMOD_System_PlaySound(system, 0, duketales, 0, NULL);           // On joue la musique

	FMOD_CHANNEL *musiqueDeFond;
	FMOD_System_GetChannel(system, 0, &musiqueDeFond);
    FMOD_Channel_SetVolume(musiqueDeFond, 0.2);

    // Initialisation SDL
    SDL_Init(SDL_INIT_VIDEO);
    ecran = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    int continuer = 1;
    int deroulementFond = -1160;
    int choix = 0;

    fond = SDL_LoadBMP("menus et fonds/nuages.bmp");
    positionFond.y = 0;

    jouer = IMG_Load("menus et fonds/Jouer.png");
    positionJouer.x = 0;
    positionJouer.y = 0;

    quitter = IMG_Load("menus et fonds/Quitter.png");
    positionQuitter.x = 0;
    positionQuitter.y = 0;

    // Chargements de la transition :
    transition[1] = IMG_Load("menus et fonds/transitions/transition30.png");
    transition[2] = IMG_Load("menus et fonds/transitions/transition50.png");
    transition[3] = IMG_Load("menus et fonds/transitions/transition70.png");
    transition[4] = IMG_Load("menus et fonds/transitions/transition80.png");
    transition[5] = IMG_Load("menus et fonds/transitions/transition85.png");
    transition[6] = IMG_Load("menus et fonds/transitions/transition90.png");
    transition[7] = IMG_Load("menus et fonds/transitions/transition95.png");
    transition[8] = IMG_Load("menus et fonds/transitions/transition100.png");
    int deroulementTransition = 8;

    Input in;
    memset(&in,0,sizeof(in));

    while(continuer)
        {
            positionFond.x = deroulementFond;
            if (deroulementFond == 0)
                deroulementFond = -1160;
            else
                deroulementFond++;

            UpdateEvents(&in);

            if (in.key[SDLK_RETURN] && choix % 2 == 0)
                {
                    FMOD_Channel_SetVolume(musiqueDeFond, 0.1);
                    if(menuChoixJoueurs(ecran, system, musiqueDeFond))
                        {
                            SDL_FreeSurface(fond);
                            SDL_FreeSurface(jouer);
                            SDL_FreeSurface(quitter);
                            FMOD_Sound_Release(duketales);
                            FMOD_System_Close(system);
                            FMOD_System_Release(system);
                            return QUITTER;
                        }
                    memset(&in,0,sizeof(in));
                    FMOD_Channel_SetVolume(musiqueDeFond, 0.2);
                }
            if (in.key[SDLK_ESCAPE] || in.quit || (in.key[SDLK_RETURN] && choix % 2 == 1))
                {
                    SDL_FreeSurface(fond);
                    SDL_FreeSurface(jouer);
                    SDL_FreeSurface(quitter);
                    FMOD_Sound_Release(duketales);
                    FMOD_System_Close(system);
                    FMOD_System_Release(system);
                    return QUITTER;
                }
            if (in.key[SDLK_UP] || in.key[SDLK_DOWN])
                {
                    choix++;
                    in.key[SDLK_UP] = 0;
                    in.key[SDLK_DOWN] = 0;
                }

            SDL_Delay(35);
            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
            SDL_BlitSurface(fond, NULL, ecran, &positionFond);

            if(choix % 2 == 0)
                SDL_BlitSurface(jouer, NULL, ecran, &positionJouer);
            else
                SDL_BlitSurface(quitter, NULL, ecran, &positionQuitter);

            // EFFET DE FONDU :
                if (deroulementTransition)
                    {
                        SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionJouer);
                        deroulementTransition--;
                    }

            SDL_Flip(ecran);
        }

}


int menuChoixJoueurs(SDL_Surface* ecran, FMOD_SYSTEM *system, FMOD_CHANNEL *musiqueDeFond)
{
    SDL_Surface *menu = NULL, *cadre = NULL, *perso1 = NULL, *perso2 = NULL;
    SDL_Surface *perso[7] = {NULL};
    SDL_Surface  *cache = NULL, *tabtojoin = NULL, *game_over = NULL, *victoire = NULL, *win1 = NULL, *win2 = NULL;
    SDL_Rect positionMenu, positionCadre1, positionCadre2, positionPerso1, positionPerso2;
    SDL_Rect positionCache, positionTabtojoin;

    // Son
    FMOD_SOUND *bip;
    FMOD_System_CreateSound(system, "sons/bipmenu.mp3", FMOD_CREATESAMPLE, 0, &bip);
    FMOD_SOUND *seeyou;
    FMOD_System_CreateSound(system, "sons/seeyou.wav", FMOD_CREATESAMPLE, 0, &seeyou);
    FMOD_SOUND *igotit;
    FMOD_System_CreateSound(system, "sons/victoire.wav", FMOD_CREATESAMPLE, 0, &igotit);

	FMOD_CHANNEL *bipmenu;
	FMOD_System_GetChannel(system, 1, &bipmenu);
	FMOD_CHANNEL *bipmenu2;
	FMOD_System_GetChannel(system, 2, &bipmenu2);

    // Chargement images
    perso[ROUGEBLEU] = IMG_Load("menus et fonds/choix joueur/rougebleu.png");
    perso[BLEUROUGE] = IMG_Load("menus et fonds/choix joueur/bleurouge.png");
    perso[VERTBLANC] = IMG_Load("menus et fonds/choix joueur/vertblanc.png");
    perso[BLANCVERT] = IMG_Load("menus et fonds/choix joueur/blancvert.png");
    perso[VIOLETJAUNE] = IMG_Load("menus et fonds/choix joueur/violetjaune.png");
    perso[JAUNEVIOLET] = IMG_Load("menus et fonds/choix joueur/jauneviolet.png");

    menu = SDL_LoadBMP("menus et fonds/choixjoueur.bmp");
    positionMenu.x = 0;
    positionMenu.y = 0;

    cache = SDL_LoadBMP("menus et fonds/choix joueur/cache.bmp");
    positionCache.x = 300;
    positionCache.y = 300;

    tabtojoin = IMG_Load("menus et fonds/choix joueur/tabtojoin.png");
    positionTabtojoin.x = 0;
    positionTabtojoin.y = 0;

    perso1 = perso[ROUGEBLEU];
    positionPerso1.x = 112;
    positionPerso1.y = 325;

    perso2 = perso[ROUGEBLEU];
    positionPerso2.x = 400;
    positionPerso2.y = 325;

    cadre = IMG_Load("menus et fonds/choix joueur/cadre selection blanc.png");
    positionCadre1.x = 37;
    positionCadre1.y = 473;
    positionCadre2.x = 345;
    positionCadre2.y = 473;

    game_over = IMG_Load("menus et fonds/menu pause/game_over.png");
    victoire = IMG_Load("menus et fonds/menu pause/victoire.png");
    win1 = IMG_Load("menus et fonds/menu pause/win1.png");
    win2 = IMG_Load("menus et fonds/menu pause/win2.png");

    int i = 0;
    int vertical1 = 0;
    int horizontal1 = 0;
    int vertical2 = 0;
    int horizontal2 = 0;
    int continuer = 1;
    int exit = 0;
    int victoires = 0;
    int victoiresJ1 = 0;
    int victoiresJ2 = 0;
    int player2 = 0;
    int persoChoisi = ROUGEBLEU;
    int perso2Choisi = ROUGEBLEU;
    int presstabtojoin = 1;

    Input in;
    memset(&in,0,sizeof(in));


    while(continuer)
        {
            SDL_Delay(50);
            UpdateEvents(&in);

            if (in.key[SDLK_ESCAPE])
                return PRECEDENT;

            if (in.quit)
                return QUITTER;

            if (in.key[SDLK_RETURN])
                {
                    memset(&in,0,sizeof(in));
                    FMOD_Channel_SetVolume(musiqueDeFond, 0.02);
                    for (i=1;i<4;i++)
                        {
                            exit = game(ecran, system, musiqueDeFond, player2, persoChoisi, perso2Choisi, i);
                            switch(exit)
                                {
                                    case WIN:
                                        victoires++;
                                        if (victoires==3)
                                            {
                                                FMOD_System_PlaySound(system, 1, igotit, 0, NULL);
                                                FMOD_Channel_SetVolume(bipmenu, 0.5);
                                                SDL_BlitSurface(victoire, NULL, ecran, &positionMenu);
                                                SDL_Flip(ecran);
                                                SDL_Delay(2500);
                                                return PRECEDENT;
                                            }
                                        break;

                                    case WINJ1:
                                        victoiresJ1++;
                                        if ((victoiresJ1==2 && victoiresJ2==1) || victoiresJ1==3)
                                            {
                                                FMOD_System_PlaySound(system, 1, igotit, 0, NULL);
                                                FMOD_Channel_SetVolume(bipmenu, 0.5);
                                                SDL_BlitSurface(win1, NULL, ecran, &positionMenu);
                                                SDL_Flip(ecran);
                                                SDL_Delay(2500);
                                                return PRECEDENT;
                                            }
                                        if (victoiresJ2==2 && victoiresJ1==1)
                                            {
                                                FMOD_System_PlaySound(system, 1, igotit, 0, NULL);
                                                FMOD_Channel_SetVolume(bipmenu, 0.5);
                                                SDL_BlitSurface(win2, NULL, ecran, &positionMenu);
                                                SDL_Flip(ecran);
                                                SDL_Delay(2500);
                                                return PRECEDENT;
                                            }
                                        break;

                                    case WINJ2:
                                        victoiresJ2++;
                                        if ((victoiresJ2==2 && victoiresJ1==1) || victoiresJ2==3)
                                            {
                                                FMOD_System_PlaySound(system, 1, igotit, 0, NULL);
                                                FMOD_Channel_SetVolume(bipmenu, 0.5);
                                                SDL_BlitSurface(win2, NULL, ecran, &positionMenu);
                                                SDL_Flip(ecran);
                                                SDL_Delay(2500);
                                                return PRECEDENT;
                                            }
                                        if (victoiresJ1==2 && victoiresJ2==1)
                                            {
                                                FMOD_System_PlaySound(system, 1, igotit, 0, NULL);
                                                FMOD_Channel_SetVolume(bipmenu, 0.5);
                                                SDL_BlitSurface(win1, NULL, ecran, &positionMenu);
                                                SDL_Flip(ecran);
                                                SDL_Delay(2500);
                                                return PRECEDENT;
                                            }
                                        break;

                                    case GAMEOVER:
                                        FMOD_System_PlaySound(system, 1, seeyou, 0, NULL);
                                        FMOD_Channel_SetVolume(bipmenu, 0.5);
                                        SDL_BlitSurface(game_over, NULL, ecran, &positionMenu);
                                        SDL_Flip(ecran);
                                        SDL_Delay(4000);
                                        return PRECEDENT;
                                        break;

                                    case RETOURMENU:
                                        return PRECEDENT;

                                    case QUITTER:
                                        return QUITTER;
                                }
                        }

                    return QUITTER;
                }
            if (in.key[SDLK_UP] || in.key[SDLK_DOWN])
                {
                    vertical1++;
                    in.key[SDLK_UP] = 0;
                    in.key[SDLK_DOWN] = 0;
                    FMOD_System_PlaySound(system, 1, bip, 0, NULL);
                    FMOD_Channel_SetVolume(bipmenu, 0.3);
                }
            if (in.key[SDLK_LEFT])
                {
                    horizontal1--;
                    in.key[SDLK_LEFT] = 0;
                    FMOD_System_PlaySound(system, 1, bip, 0, NULL);
                    FMOD_Channel_SetVolume(bipmenu, 0.3);
                }
            if (in.key[SDLK_RIGHT])
                {
                    horizontal1++;
                    in.key[SDLK_RIGHT] = 0;
                    FMOD_System_PlaySound(system, 1, bip, 0, NULL);
                    FMOD_Channel_SetVolume(bipmenu, 0.3);
                }
            if (in.key[SDLK_TAB] && !player2)
                {
                    player2 = 1;
                    in.key[SDLK_TAB] = 0;
                }
            if (in.key[SDLK_TAB] && player2)
                {
                    player2 = 0;
                    in.key[SDLK_TAB] = 0;
                }
            if (player2)
                {
                    if (in.key[SDLK_w] || in.key[SDLK_s])
                        {
                            vertical2++;
                            in.key[SDLK_w] = 0;
                            in.key[SDLK_s] = 0;
                            FMOD_System_PlaySound(system, 2, bip, 0, NULL);
                            FMOD_Channel_SetVolume(bipmenu2, 0.5);
                        }
                    if (in.key[SDLK_a])
                        {
                            horizontal2--;
                            in.key[SDLK_a] = 0;
                            FMOD_System_PlaySound(system, 2, bip, 0, NULL);
                            FMOD_Channel_SetVolume(bipmenu2, 0.5);
                        }
                    if (in.key[SDLK_d])
                        {
                            horizontal2++;
                            in.key[SDLK_d] = 0;
                            FMOD_System_PlaySound(system, 2, bip, 0, NULL);
                            FMOD_Channel_SetVolume(bipmenu2, 0.5);
                        }
                }

            if (vertical1 % 2 == 0)
                positionCadre1.y = 473;
            else
                positionCadre1.y = 530;
            if (horizontal1 % 3 == 0)
                positionCadre1.x = 47;
            else
                if (horizontal1 % 3 == 1 || horizontal1 % 3 == -2)
                    positionCadre1.x = 119;
                else
                    positionCadre1.x = 191;

            if (vertical2 % 2 == 0)
                positionCadre2.y = 473;
            else
                positionCadre2.y = 530;
            if (horizontal2 % 3 == 0)
                positionCadre2.x = 335;
            else
                if (horizontal2 % 3 == 1 || horizontal2 % 3 == -2)
                    positionCadre2.x = 407;
                else
                    positionCadre2.x = 479;

            switch(positionCadre1.y)
                {
                    case 473:
                            switch(positionCadre1.x)
                            {
                                case 47:
                                    perso1 = perso[ROUGEBLEU];
                                    persoChoisi = ROUGEBLEU;
                                    break;
                                case 119:
                                    perso1 = perso[VERTBLANC];
                                    persoChoisi = VERTBLANC;
                                    break;
                                case 191:
                                    perso1 = perso[VIOLETJAUNE];
                                    persoChoisi = VIOLETJAUNE;
                                    break;
                            }
                            break;

                    case 530:
                            switch(positionCadre1.x)
                            {
                                case 47:
                                    perso1 = perso[BLEUROUGE];
                                    persoChoisi = BLEUROUGE;
                                    break;
                                case 119:
                                    perso1 = perso[BLANCVERT];
                                    persoChoisi = BLANCVERT;
                                    break;
                                case 191:
                                    perso1 = perso[JAUNEVIOLET];
                                    persoChoisi = JAUNEVIOLET;
                                    break;
                            }
                            break;
                }

            switch(positionCadre2.y)
                {
                    case 473:
                            switch(positionCadre2.x)
                            {
                                case 335:
                                    perso2 = perso[ROUGEBLEU];
                                    perso2Choisi = ROUGEBLEU;
                                    break;
                                case 407:
                                    perso2 = perso[VERTBLANC];
                                    perso2Choisi = VERTBLANC;
                                    break;
                                case 479:
                                    perso2 = perso[VIOLETJAUNE];
                                    perso2Choisi = VIOLETJAUNE;
                                    break;
                            }
                            break;

                    case 530:
                            switch(positionCadre2.x)
                            {
                                case 335:
                                    perso2 = perso[BLEUROUGE];
                                    perso2Choisi = BLEUROUGE;
                                    break;
                                case 407:
                                    perso2 = perso[BLANCVERT];
                                    perso2Choisi = BLANCVERT;
                                    break;
                                case 479:
                                    perso2 = perso[JAUNEVIOLET];
                                    perso2Choisi = JAUNEVIOLET;
                                    break;
                            }
                            break;
                }


            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));      // Efface l'écran (rempli de noir)
            SDL_BlitSurface(menu, NULL, ecran, &positionMenu);                  // Affiche le fond (décor non animé)
            SDL_BlitSurface(cadre, NULL, ecran, &positionCadre1);
            SDL_BlitSurface(perso1, NULL, ecran, &positionPerso1);
            if (player2)
                {
                    SDL_BlitSurface(perso2, NULL, ecran, &positionPerso2);
                    SDL_BlitSurface(cadre, NULL, ecran, &positionCadre2);
                }
            else
                {
                    SDL_BlitSurface(cache, NULL, ecran, &positionCache);
                    presstabtojoin++;
                    if(presstabtojoin % 20 < 10 )
                        {
                            SDL_BlitSurface(tabtojoin, NULL, ecran, &positionTabtojoin);
                        }
                }

            SDL_Flip(ecran);

        }

}


//void transition (SDL_Surface* ecran)
