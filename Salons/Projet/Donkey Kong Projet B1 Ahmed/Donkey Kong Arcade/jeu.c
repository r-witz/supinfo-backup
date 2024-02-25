#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <fmodex/fmod.h>

#include "constantes.h"
#include "jeu.h"

int son = 1;
int debug = 0;


typedef struct
    {
        char key[SDLK_LAST];
        char quit;
        char focus;
    } Input;


int game(SDL_Surface* ecran, FMOD_SYSTEM *system, FMOD_CHANNEL *musiqueDeFond, int player2, int persoChoisi, int perso2Choisi, int level)
{

//////////////////
// DECLARATIONS //
//////////////////

    SDL_Surface *fond = NULL, *structure = NULL, *structureCine = NULL, *header = NULL, *texteVies = NULL, *texteLevel = NULL, *screenshot = NULL;
    SDL_Surface *peach = NULL, *help = NULL, *coeur = NULL, *coeurBrise = NULL, *pierre = NULL, *tonneaux = NULL, *sacs;
    SDL_Surface *marioPosture[2] = {NULL};
    SDL_Surface *mario[2][11] = {NULL};
    SDL_Surface *donkey[19] = {NULL};
    SDL_Surface *donkeyMarche[12] = {NULL};
    SDL_Surface *tonneau[7] = {NULL};
    SDL_Surface *tonneauPosture[9] = {NULL};
    SDL_Surface *explosion[9] = {NULL};
    SDL_Surface *explosionPosture[9] = {NULL};
    SDL_Surface *transition[10] = {NULL};
    SDL_Surface *gagnantNiveau[5] = {NULL};

    SDL_Rect positionFond, positionStructure, positionHeader, positionTexteVies, positionTexteLevel, positionPierre, positionTonneaux, positionSacs;
    SDL_Rect positionMiniatureInit, positionMiniature, positionMarioInitiale, positionDonkey, positionDK, positionPeach, positionHelp, positionCoeur;
    SDL_Rect positionMario[2] = {0, 0};
    SDL_Rect positionMarioAvantSaut[2] = {0, 0};
    SDL_Rect positionTonneau[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    SDL_Rect positionExplosion[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    int cinematiqueDebut = 0;
    int continuer = 1;                  // Permet la continuité du jeu (1) en tant que condition de la boucle principale (0 : retour au menu)
    int cinematiqueLevelup = 0;
    int cinematiqueHappyend = 0;
    int exit = 0;
    int win[2] = {0, 0};
    int lose[2] = {0, 0};
    int nbVies = 3;
    int clignotement[2] = {0, 0};
    int gagnant = 0;

    int etape = 1;
    int animDonkey = 0;
    int donkeyPosture = 0;

    int varPostureMario[2] = {0, 0};            // Utilisé pour animer le personnage permmettant un nombre d'image égal au modulo choisi (voir suite)
    int varPostureTonneau = 0;
    int lastPosture[2] = {DROITE, DROITE};

    int vitesseTonneaux = 1;
    int lancementTonneau = 0;
    int explosionEnCours[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int statutExplosion[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    int saut[2] = {0, 0};                      // Détermine si un saut est en cours ou non
    int sautHaut[2] = {0, 0};
    int sautLeft[2] = {0, 0};
    int sautRight[2] = {0, 0};
    int hauteurSaut[2] = {0, 0};
    int tourneDansLeVide[2] = {0, 0};

    int diffY[2] = {0, 0};
    int diffYtotale[2] = {0, 0};

    int i, j = 0;
    int randomValue = 0;
    int randomEchelle = 0;
    int hauteurEchelle[2] = {0, 0};                 // Hauteur de l'échelle selon notre position, 0 = pas d'échelle
    int deplacementMarioSurEchelle[2] = {0, 0};     // Booléen : 0 = Ne se déplace pas sur une échelle (mouvement libre)
                                            //           1 = Se déplace sur une échelle (mouvement horizontal et saut impossible)
    int deplacementTonneauSurEchelle[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    Input in;                               // Structure comportant les booléens indiquants l'appui ou non des touches du clavier
    memset(&in,0,sizeof(in));               // Réinitialize cette structure


    // DEBUG :
    SDL_Surface *texte = NULL, *texte2 = NULL, *texte3 = NULL, *texte4 = NULL;
    SDL_Surface *ligneTest = NULL, *ligneTest2 = NULL, *marioCadreTest = NULL, *collisionCadreTest = NULL;
    SDL_Rect positionTexte, positionTexte2, positionTexte3, positionTexte4, positionLigneTest;
    SDL_Rect positionCadreCollisions[2] = {0, 0};
    SDL_Color couleurNoire = {0, 0, 0};
    TTF_Font *police = NULL;
    char positionnementX[15] = "";
    char positionnementY[15] = "";
    char positionnementZ[15] = "";
    char positionnementZbis[15] = "";
    int posX = 0, posY = 0, posZ = 0, posZbis = 0;
    positionTexte.x = 20;
    positionTexte.y = 365;
    positionTexte2.x = 20;
    positionTexte2.y = 385;
    positionTexte3.x = 20;
    positionTexte3.y = 405;
    positionTexte4.x = 20;
    positionTexte4.y = 425;
    ligneTest = IMG_Load("debugging/test.png");
    ligneTest2 = IMG_Load("debugging/test2.png");
    marioCadreTest = IMG_Load("debugging/cadre noir taille sprite.png");
    collisionCadreTest = IMG_Load("debugging/cadre noir collisions.png");



/////////////////
// CHARGEMENTS //
/////////////////


    // Chargement du fond :
    fond = SDL_LoadBMP("menus et fonds/nuages.bmp");
    int deroulementFond = -1160;
        positionFond.x = deroulementFond;
        positionFond.y = 0;
    structure = IMG_Load("menus et fonds/fond.png");
    structureCine = IMG_Load("menus et fonds/fondcine.png");
        positionStructure.x = 0;
        positionStructure.y = 0;
    sacs = IMG_Load("sprites autres/tonneaux/sac blocage.png");
        positionSacs.x = 10;
        positionSacs.y = 175;
    tonneaux = IMG_Load("sprites autres/tonneaux/tonneaux.png");
        positionTonneaux.x = 45;
        positionTonneaux.y = 175;
    peach = IMG_Load("sprites autres/peach/peach.png");
        positionPeach.x = 280;
        positionPeach.y = 95;
    help = IMG_Load("sprites autres/peach/peach_help.png");
        positionHelp.x = 315;
        positionHelp.y = 95;
    pierre = IMG_Load("sprites autres/explosions/rock8.png");
        positionPierre.x = 235;
        positionPierre.y = 124;
    coeur = IMG_Load("sprites autres/peach/peach_coeur.png");
    coeurBrise = IMG_Load("sprites autres/peach/peach_coeur_brise.png");
        positionCoeur.x = 272;
        positionCoeur.y = 95;
    header = IMG_Load("menus et fonds/header.png");
        positionHeader.x = 90;
        positionHeader.y = 0;
        positionTexteVies.x = 140;
        positionTexteVies.y = 10;
        positionMiniatureInit.x = 200;
        positionMiniatureInit.y = 5;
        positionMiniature.y = 5;
        positionTexteLevel.x = 350;
        positionTexteLevel.y = 10;
    SDL_Color couleurBlanche = {255, 255, 255};
    police = TTF_OpenFont("stencil.ttf", 15);
    char textVies[10] = "";
    char textLevel[10] = "";

    sprintf(textLevel, "NIVEAU : %d", level);
    texteLevel = TTF_RenderText_Blended(police, textLevel, couleurBlanche);

    if (player2)
        {
            sprintf(textVies, "SCORE : ");
            texteVies = TTF_RenderText_Solid(police, textVies, couleurBlanche);
        }
    else
        {
            sprintf(textVies, " VIES : ");
            texteVies = TTF_RenderText_Solid(police, textVies, couleurBlanche);
        }


    // Chargement des personnages :
    switch(persoChoisi)
    {
        case ROUGEBLEU:
                mario[J1][HAUT1] = IMG_Load("sprites mario/mario rouge et bleu/haut1.png");
                mario[J1][HAUT2] = IMG_Load("sprites mario/mario rouge et bleu/haut2.png");
                mario[J1][GAUCHE1] = IMG_Load("sprites mario/mario rouge et bleu/gauche1.png");
                mario[J1][GAUCHE2] = IMG_Load("sprites mario/mario rouge et bleu/gauche2.png");
                mario[J1][GAUCHE3] = IMG_Load("sprites mario/mario rouge et bleu/gauche3.png");
                mario[J1][DROITE1] = IMG_Load("sprites mario/mario rouge et bleu/droite1.png");
                mario[J1][DROITE2] = IMG_Load("sprites mario/mario rouge et bleu/droite2.png");
                mario[J1][DROITE3] = IMG_Load("sprites mario/mario rouge et bleu/droite3.png");
                mario[J1][PETIT] = IMG_Load("sprites mario/mario rouge et bleu/petit.png");
                break;

        case BLEUROUGE:
                mario[J1][HAUT1] = IMG_Load("sprites mario/mario bleu et rouge/haut1.png");
                mario[J1][HAUT2] = IMG_Load("sprites mario/mario bleu et rouge/haut2.png");
                mario[J1][GAUCHE1] = IMG_Load("sprites mario/mario bleu et rouge/gauche1.png");
                mario[J1][GAUCHE2] = IMG_Load("sprites mario/mario bleu et rouge/gauche2.png");
                mario[J1][GAUCHE3] = IMG_Load("sprites mario/mario bleu et rouge/gauche3.png");
                mario[J1][DROITE1] = IMG_Load("sprites mario/mario bleu et rouge/droite1.png");
                mario[J1][DROITE2] = IMG_Load("sprites mario/mario bleu et rouge/droite2.png");
                mario[J1][DROITE3] = IMG_Load("sprites mario/mario bleu et rouge/droite3.png");
                mario[J1][PETIT] = IMG_Load("sprites mario/mario bleu et rouge/petit.png");
                break;

        case VERTBLANC:
                mario[J1][HAUT1] = IMG_Load("sprites mario/mario vert et blanc/haut1.png");
                mario[J1][HAUT2] = IMG_Load("sprites mario/mario vert et blanc/haut2.png");
                mario[J1][GAUCHE1] = IMG_Load("sprites mario/mario vert et blanc/gauche1.png");
                mario[J1][GAUCHE2] = IMG_Load("sprites mario/mario vert et blanc/gauche2.png");
                mario[J1][GAUCHE3] = IMG_Load("sprites mario/mario vert et blanc/gauche3.png");
                mario[J1][DROITE1] = IMG_Load("sprites mario/mario vert et blanc/droite1.png");
                mario[J1][DROITE2] = IMG_Load("sprites mario/mario vert et blanc/droite2.png");
                mario[J1][DROITE3] = IMG_Load("sprites mario/mario vert et blanc/droite3.png");
                mario[J1][PETIT] = IMG_Load("sprites mario/mario vert et blanc/petit.png");
                break;

        case BLANCVERT:
                mario[J1][HAUT1] = IMG_Load("sprites mario/mario blanc et vert/haut1.png");
                mario[J1][HAUT2] = IMG_Load("sprites mario/mario blanc et vert/haut2.png");
                mario[J1][GAUCHE1] = IMG_Load("sprites mario/mario blanc et vert/gauche1.png");
                mario[J1][GAUCHE2] = IMG_Load("sprites mario/mario blanc et vert/gauche2.png");
                mario[J1][GAUCHE3] = IMG_Load("sprites mario/mario blanc et vert/gauche3.png");
                mario[J1][DROITE1] = IMG_Load("sprites mario/mario blanc et vert/droite1.png");
                mario[J1][DROITE2] = IMG_Load("sprites mario/mario blanc et vert/droite2.png");
                mario[J1][DROITE3] = IMG_Load("sprites mario/mario blanc et vert/droite3.png");
                mario[J1][PETIT] = IMG_Load("sprites mario/mario blanc et vert/petit.png");
                break;

        case VIOLETJAUNE:
                mario[J1][HAUT1] = IMG_Load("sprites mario/mario violet et jaune/haut1.png");
                mario[J1][HAUT2] = IMG_Load("sprites mario/mario violet et jaune/haut2.png");
                mario[J1][GAUCHE1] = IMG_Load("sprites mario/mario violet et jaune/gauche1.png");
                mario[J1][GAUCHE2] = IMG_Load("sprites mario/mario violet et jaune/gauche2.png");
                mario[J1][GAUCHE3] = IMG_Load("sprites mario/mario violet et jaune/gauche3.png");
                mario[J1][DROITE1] = IMG_Load("sprites mario/mario violet et jaune/droite1.png");
                mario[J1][DROITE2] = IMG_Load("sprites mario/mario violet et jaune/droite2.png");
                mario[J1][DROITE3] = IMG_Load("sprites mario/mario violet et jaune/droite3.png");
                mario[J1][PETIT] = IMG_Load("sprites mario/mario violet et jaune/petit.png");
                break;

        case JAUNEVIOLET:
                mario[J1][HAUT1] = IMG_Load("sprites mario/mario jaune et violet/haut1.png");
                mario[J1][HAUT2] = IMG_Load("sprites mario/mario jaune et violet/haut2.png");
                mario[J1][GAUCHE1] = IMG_Load("sprites mario/mario jaune et violet/gauche1.png");
                mario[J1][GAUCHE2] = IMG_Load("sprites mario/mario jaune et violet/gauche2.png");
                mario[J1][GAUCHE3] = IMG_Load("sprites mario/mario jaune et violet/gauche3.png");
                mario[J1][DROITE1] = IMG_Load("sprites mario/mario jaune et violet/droite1.png");
                mario[J1][DROITE2] = IMG_Load("sprites mario/mario jaune et violet/droite2.png");
                mario[J1][DROITE3] = IMG_Load("sprites mario/mario jaune et violet/droite3.png");
                mario[J1][PETIT] = IMG_Load("sprites mario/mario jaune et violet/petit.png");
                break;
    }

    switch(perso2Choisi)
    {
        case ROUGEBLEU:
                mario[J2][HAUT1] = IMG_Load("sprites mario/mario rouge et bleu/haut1.png");
                mario[J2][HAUT2] = IMG_Load("sprites mario/mario rouge et bleu/haut2.png");
                mario[J2][GAUCHE1] = IMG_Load("sprites mario/mario rouge et bleu/gauche1.png");
                mario[J2][GAUCHE2] = IMG_Load("sprites mario/mario rouge et bleu/gauche2.png");
                mario[J2][GAUCHE3] = IMG_Load("sprites mario/mario rouge et bleu/gauche3.png");
                mario[J2][DROITE1] = IMG_Load("sprites mario/mario rouge et bleu/droite1.png");
                mario[J2][DROITE2] = IMG_Load("sprites mario/mario rouge et bleu/droite2.png");
                mario[J2][DROITE3] = IMG_Load("sprites mario/mario rouge et bleu/droite3.png");
                mario[J2][PETIT] = IMG_Load("sprites mario/mario rouge et bleu/petit.png");
                break;

        case BLEUROUGE:
                mario[J2][HAUT1] = IMG_Load("sprites mario/mario bleu et rouge/haut1.png");
                mario[J2][HAUT2] = IMG_Load("sprites mario/mario bleu et rouge/haut2.png");
                mario[J2][GAUCHE1] = IMG_Load("sprites mario/mario bleu et rouge/gauche1.png");
                mario[J2][GAUCHE2] = IMG_Load("sprites mario/mario bleu et rouge/gauche2.png");
                mario[J2][GAUCHE3] = IMG_Load("sprites mario/mario bleu et rouge/gauche3.png");
                mario[J2][DROITE1] = IMG_Load("sprites mario/mario bleu et rouge/droite1.png");
                mario[J2][DROITE2] = IMG_Load("sprites mario/mario bleu et rouge/droite2.png");
                mario[J2][DROITE3] = IMG_Load("sprites mario/mario bleu et rouge/droite3.png");
                mario[J2][PETIT] = IMG_Load("sprites mario/mario bleu et rouge/petit.png");
                break;

        case VERTBLANC:
                mario[J2][HAUT1] = IMG_Load("sprites mario/mario vert et blanc/haut1.png");
                mario[J2][HAUT2] = IMG_Load("sprites mario/mario vert et blanc/haut2.png");
                mario[J2][GAUCHE1] = IMG_Load("sprites mario/mario vert et blanc/gauche1.png");
                mario[J2][GAUCHE2] = IMG_Load("sprites mario/mario vert et blanc/gauche2.png");
                mario[J2][GAUCHE3] = IMG_Load("sprites mario/mario vert et blanc/gauche3.png");
                mario[J2][DROITE1] = IMG_Load("sprites mario/mario vert et blanc/droite1.png");
                mario[J2][DROITE2] = IMG_Load("sprites mario/mario vert et blanc/droite2.png");
                mario[J2][DROITE3] = IMG_Load("sprites mario/mario vert et blanc/droite3.png");
                mario[J2][PETIT] = IMG_Load("sprites mario/mario vert et blanc/petit.png");
                break;

        case BLANCVERT:
                mario[J2][HAUT1] = IMG_Load("sprites mario/mario blanc et vert/haut1.png");
                mario[J2][HAUT2] = IMG_Load("sprites mario/mario blanc et vert/haut2.png");
                mario[J2][GAUCHE1] = IMG_Load("sprites mario/mario blanc et vert/gauche1.png");
                mario[J2][GAUCHE2] = IMG_Load("sprites mario/mario blanc et vert/gauche2.png");
                mario[J2][GAUCHE3] = IMG_Load("sprites mario/mario blanc et vert/gauche3.png");
                mario[J2][DROITE1] = IMG_Load("sprites mario/mario blanc et vert/droite1.png");
                mario[J2][DROITE2] = IMG_Load("sprites mario/mario blanc et vert/droite2.png");
                mario[J2][DROITE3] = IMG_Load("sprites mario/mario blanc et vert/droite3.png");
                mario[J2][PETIT] = IMG_Load("sprites mario/mario blanc et vert/petit.png");
                break;

        case VIOLETJAUNE:
                mario[J2][HAUT1] = IMG_Load("sprites mario/mario violet et jaune/haut1.png");
                mario[J2][HAUT2] = IMG_Load("sprites mario/mario violet et jaune/haut2.png");
                mario[J2][GAUCHE1] = IMG_Load("sprites mario/mario violet et jaune/gauche1.png");
                mario[J2][GAUCHE2] = IMG_Load("sprites mario/mario violet et jaune/gauche2.png");
                mario[J2][GAUCHE3] = IMG_Load("sprites mario/mario violet et jaune/gauche3.png");
                mario[J2][DROITE1] = IMG_Load("sprites mario/mario violet et jaune/droite1.png");
                mario[J2][DROITE2] = IMG_Load("sprites mario/mario violet et jaune/droite2.png");
                mario[J2][DROITE3] = IMG_Load("sprites mario/mario violet et jaune/droite3.png");
                mario[J2][PETIT] = IMG_Load("sprites mario/mario violet et jaune/petit.png");
                break;

        case JAUNEVIOLET:
                mario[J2][HAUT1] = IMG_Load("sprites mario/mario jaune et violet/haut1.png");
                mario[J2][HAUT2] = IMG_Load("sprites mario/mario jaune et violet/haut2.png");
                mario[J2][GAUCHE1] = IMG_Load("sprites mario/mario jaune et violet/gauche1.png");
                mario[J2][GAUCHE2] = IMG_Load("sprites mario/mario jaune et violet/gauche2.png");
                mario[J2][GAUCHE3] = IMG_Load("sprites mario/mario jaune et violet/gauche3.png");
                mario[J2][DROITE1] = IMG_Load("sprites mario/mario jaune et violet/droite1.png");
                mario[J2][DROITE2] = IMG_Load("sprites mario/mario jaune et violet/droite2.png");
                mario[J2][DROITE3] = IMG_Load("sprites mario/mario jaune et violet/droite3.png");
                mario[J2][PETIT] = IMG_Load("sprites mario/mario jaune et violet/petit.png");
                break;
    }



    // Posture et position initiale du personnage :
    positionMarioInitiale.x = LIM_BL_4_5;
    positionMarioInitiale.y = 547;
    marioPosture[J1] = mario[J1][DROITE1];
    positionMario[J1].x = positionMarioInitiale.x;
    positionMario[J1].y = positionMarioInitiale.y;
    marioPosture[J2] = mario[J2][DROITE1];
    positionMario[J2].x = positionMarioInitiale.x-30;
    positionMario[J2].y = positionMarioInitiale.y;
    positionDonkey.x = LIM_BL_2_3;
    positionDonkey.y = 156;


    // Chargements des donkeys :
    donkey[ATTEND] = IMG_Load("sprites autres/DK/donkey_attend.png");
    donkey[PREND1] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_0.png");
    donkey[PREND2] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_1.png");
    donkey[PREND3] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_2.png");
    donkey[PREND4] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_3.png");
    donkey[LANCE1] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_4.png");
    donkey[LANCE2] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_5.png");
    donkey[LANCE3] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_6.png");
    donkey[LANCE4] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_7.png");
    donkey[LANCE5] = IMG_Load("sprites autres/DK/donkey_lance_tonneau_8.png");
    donkey[MONTEGAUCHE] = IMG_Load("sprites autres/DK/donkey_monte_gauche.png");
    donkey[MONTEDROITE] = IMG_Load("sprites autres/DK/donkey_monte_droite.png");
    donkey[MONTEGAUCHEPEACH] = IMG_Load("sprites autres/DK/donkey_monte_gauche_peach.png");
    donkey[MONTEDROITEPEACH] = IMG_Load("sprites autres/DK/donkey_monte_droite_peach.png");
    donkey[ATTEND2] = IMG_Load("sprites autres/DK/donkey_attend_2.png");
    donkey[SAUT1] = IMG_Load("sprites autres/DK/donkey_saut_1.png");
    donkey[SAUT2] = IMG_Load("sprites autres/DK/donkey_saut_2.png");
    donkey[SAUT3] = IMG_Load("sprites autres/DK/donkey_saut_3.png");
    donkey[FIN1] = IMG_Load("sprites autres/DK/donkey_fin_0.png");
    donkey[FIN2] = IMG_Load("sprites autres/DK/donkey_fin_1.png");
    donkey[FIN3] = IMG_Load("sprites autres/DK/donkey_fin_2.png");
    donkey[FIN4] = IMG_Load("sprites autres/DK/donkey_fin_3.png");
    donkey[FIN5] = IMG_Load("sprites autres/DK/donkey_fin_4.png");


    donkeyMarche[0] = IMG_Load("sprites autres/DK/donkey_marche_0.png");
    donkeyMarche[1] = IMG_Load("sprites autres/DK/donkey_marche_1.png");
    donkeyMarche[2] = IMG_Load("sprites autres/DK/donkey_marche_2.png");
    donkeyMarche[3] = IMG_Load("sprites autres/DK/donkey_marche_3.png");
    donkeyMarche[4] = IMG_Load("sprites autres/DK/donkey_marche_4.png");
    donkeyMarche[5] = IMG_Load("sprites autres/DK/donkey_marche_5.png");
    donkeyMarche[6] = IMG_Load("sprites autres/DK/donkey_marche_6.png");
    donkeyMarche[7] = IMG_Load("sprites autres/DK/donkey_marche_7.png");
    donkeyMarche[8] = IMG_Load("sprites autres/DK/donkey_marche_8.png");
    donkeyMarche[9] = IMG_Load("sprites autres/DK/donkey_marche_9.png");
    donkeyMarche[10] = IMG_Load("sprites autres/DK/donkey_marche_10.png");


    // Chargements des tonneaux :
    tonneau[TONNEAU1] = IMG_Load("sprites autres/tonneaux/ton1.png");
    tonneau[TONNEAU2] = IMG_Load("sprites autres/tonneaux/ton2.png");
    tonneau[TONNEAU3] = IMG_Load("sprites autres/tonneaux/ton3.png");
    tonneau[TONNEAU4] = IMG_Load("sprites autres/tonneaux/ton4.png");
    tonneau[TONNEAU5] = IMG_Load("sprites autres/tonneaux/ton5.png");
    tonneau[TONNEAU6] = IMG_Load("sprites autres/tonneaux/ton6.png");
    tonneau[TONNEAU7] = IMG_Load("sprites autres/tonneaux/ton7.png");


    // Chargements des explosions :
    explosion[0] = IMG_Load("sprites autres/explosions/explo1.png");
    explosion[1] = IMG_Load("sprites autres/explosions/explo2.png");
    explosion[2] = IMG_Load("sprites autres/explosions/explo3.png");
    explosion[3] = IMG_Load("sprites autres/explosions/explo4.png");
    explosion[4] = IMG_Load("sprites autres/explosions/explo5.png");
    explosion[5] = IMG_Load("sprites autres/explosions/explo6.png");
    explosion[6] = IMG_Load("sprites autres/explosions/explo7.png");
    explosion[7] = IMG_Load("sprites autres/explosions/explo8.png");

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

    // Son
    FMOD_SOUND *jump;
    FMOD_System_CreateSound(system, "sons/jump.wav", FMOD_CREATESAMPLE, 0, &jump);
    FMOD_SOUND *expl;
    FMOD_System_CreateSound(system, "sons/firework.wav", FMOD_CREATESAMPLE, 0, &expl);
    FMOD_SOUND *peachdistress;
    FMOD_System_CreateSound(system, "sons/peachdistress.wav", FMOD_CREATESAMPLE, 0, &peachdistress);
    FMOD_SOUND *peachaah;
    FMOD_System_CreateSound(system, "sons/peachaah.wav", FMOD_CREATESAMPLE, 0, &peachaah);
    FMOD_SOUND *DKroar;
    FMOD_System_CreateSound(system, "sons/DKroar.wav", FMOD_CREATESAMPLE, 0, &DKroar);
    FMOD_SOUND *crash;
    FMOD_System_CreateSound(system, "sons/crash1.wav", FMOD_CREATESAMPLE, 0, &crash);
    FMOD_SOUND *herewego;
    FMOD_System_CreateSound(system, "sons/here we go.wav", FMOD_CREATESAMPLE, 0, &herewego);
    FMOD_SOUND *victoire;
    FMOD_System_CreateSound(system, "sons/highscore.wav", FMOD_CREATESAMPLE, 0, &victoire);
    FMOD_SOUND *ooh;
    FMOD_System_CreateSound(system, "sons/ooh!.wav", FMOD_CREATESAMPLE, 0, &ooh);


	FMOD_CHANNEL *channel1;
	FMOD_System_GetChannel(system, 1, &channel1);
	FMOD_CHANNEL *channel2;
	FMOD_System_GetChannel(system, 2, &channel2);
	FMOD_CHANNEL *channel3;
	FMOD_System_GetChannel(system, 3, &channel3);
	FMOD_CHANNEL *channel4;
	FMOD_System_GetChannel(system, 4, &channel4);

    if(player2)
        {
            switch(level)
                {
                    case 2:
                        gagnantNiveau[1] = SDL_LoadBMP ("Gagnant niveau 1.bmp");
                        SDL_SetColorKey(gagnantNiveau[1], SDL_SRCCOLORKEY, SDL_MapRGB(gagnantNiveau[1]->format, 0, 0, 0));
                        FMOD_System_PlaySound(system, 3, expl, 0, NULL);
                        break;

                    case 3:
                        gagnantNiveau[1] = SDL_LoadBMP ("Gagnant niveau 1.bmp");
                        SDL_SetColorKey(gagnantNiveau[1], SDL_SRCCOLORKEY, SDL_MapRGB(gagnantNiveau[1]->format, 0, 0, 0));
                        gagnantNiveau[2] = SDL_LoadBMP ("Gagnant niveau 2.bmp");
                        SDL_SetColorKey(gagnantNiveau[2], SDL_SRCCOLORKEY, SDL_MapRGB(gagnantNiveau[2]->format, 0, 0, 0));
                        break;
                }
        }

    if (level==1)
        {
            cinematiqueDebut = 1;
            deroulementTransition = 0;
            positionDK.x = 255;
            positionDK.y = 528;
            positionMario[J1].y = positionMarioInitiale.y+55;
            positionMario[J2].y = positionMarioInitiale.y+55;
        }



///////////////////////
// CINEMATIQUE DEBUT //
///////////////////////

    while(cinematiqueDebut)
    {
        UpdateEvents(&in);
        if (in.key[SDLK_o])
            SDL_Delay(10);
        else
            SDL_Delay(50);

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

        // CADRE : FOND ANIME, STRUCTURE ET HEADER(VIES/SCORE + NIVEAU)
        positionFond.x = deroulementFond;
        SDL_BlitSurface(fond, NULL, ecran, &positionFond);                  // Affiche le fond (décor non animé)
        SDL_BlitSurface(structureCine, NULL, ecran, &positionStructure);
        SDL_BlitSurface(tonneaux, NULL, ecran, &positionTonneaux);
        SDL_BlitSurface(sacs, NULL, ecran, &positionSacs);
        SDL_BlitSurface(pierre, NULL, ecran, &positionPierre);
        SDL_BlitSurface(header, NULL, ecran, &positionHeader);
        SDL_BlitSurface(texteVies, NULL, ecran, &positionTexteVies);
        SDL_BlitSurface(texteLevel, NULL, ecran, &positionTexteLevel);

        if (player2)
            {
                for (i=1;i<level;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(gagnantNiveau[i], NULL, ecran, &positionMiniature);
                    }
            }
        if (!player2)
            {
                for (i=0;i<nbVies;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(mario[J1][PETIT], NULL, ecran, &positionMiniature);
                    }
            }

        if (deroulementFond == 0)
            deroulementFond = -1160;
        else
            deroulementFond++;

        animDonkey++;

        // CINEMATIQUE 1er NIVEAU

        switch(etape)
            {
                case 0:
                        SDL_BlitSurface(donkey[SAUT3], NULL, ecran, &positionDK);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 1:
                        if (animDonkey % 10 >= 5)
                            {
                                positionDK.y -= 3;
                                SDL_BlitSurface(donkey[MONTEGAUCHEPEACH], NULL, ecran, &positionDK);
                            }
                        else if(animDonkey % 10 < 5)
                            {
                                positionDK.y -= 3;
                                positionHelp.x = positionDK.x + 55;
                                positionHelp.y = positionDK.y + 15;
                                SDL_BlitSurface(donkey[MONTEDROITEPEACH], NULL, ecran, &positionDK);
                                SDL_BlitSurface(help, NULL, ecran, &positionHelp);
                            }
                        if(animDonkey == 149)
                            {
                                positionDK.y += 10;
                                animDonkey = 0;
                                etape = 2;
                            }
                        if (animDonkey % 75 == 15)
                            {
                                FMOD_System_PlaySound(system, 3, peachaah, 0, NULL);
                                FMOD_Channel_SetVolume(channel3, 0.2);
                            }
                        break;

                case 2:
                        SDL_BlitSurface(donkey[LANCE5], NULL, ecran, &positionDK);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        if (animDonkey==5)
                            {
                                FMOD_System_PlaySound(system, 3, peachdistress, 0, NULL);
                                FMOD_Channel_SetVolume(channel3, 0.2);
                                animDonkey = 0;
                                etape = 3;
                            }
                        break;

                case 3:
                        SDL_BlitSurface(donkey[LANCE4], NULL, ecran, &positionDK);
                        positionPeach.x += animDonkey;
                        positionPeach.y -= animDonkey;
                        positionHelp.x = positionPeach.x + 30;
                        positionHelp.y = positionPeach.y;
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        SDL_BlitSurface(help, NULL, ecran, &positionHelp);
                        if (animDonkey==3)
                            {
                                animDonkey = 0;
                                etape = 4;
                            }
                        break;

                case 4:
                        SDL_BlitSurface(donkey[LANCE5], NULL, ecran, &positionDK);
                        positionPeach.x += animDonkey;
                        positionPeach.y += animDonkey;
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        if (animDonkey==3)
                            {
                                animDonkey = 0;
                                etape = 5;
                            }
                        break;

                case 5:
                        for(j=0;j<=player2;j++)
                            {
                                marioPosture[j] = mario[j][DROITE2];
                                donkeyPosture = ATTEND;
                                if (hauteurSaut[j]==0)
                                    {
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y -= 5;
                                        FMOD_System_PlaySound(system, 2, herewego, 0, NULL);
                                        FMOD_Channel_SetVolume(channel2, 0.3);
                                    }
                                else if (hauteurSaut[j]>0 && hauteurSaut[j]<80)
                                    {
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y -= 5;
                                    }
                                else if (hauteurSaut[j]==80 && tourneDansLeVide[j]!=2)
                                    {
                                        tourneDansLeVide[j] ++;
                                    }
                                else if (hauteurSaut[j]==80 && tourneDansLeVide[j]==2)
                                    {
                                        tourneDansLeVide[j] = 0;
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y += 5;
                                    }
                                else if (hauteurSaut[j]>80 && hauteurSaut[j]<105)
                                    {
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y += 5;
                                    }
                                else if (hauteurSaut[j]==105)
                                    {
                                        hauteurSaut[j] = 0;
                                        marioPosture[j] = mario[j][DROITE1];
                                        donkeyPosture = PREND1;
                                        positionDK.x-=10;
                                        etape = 6;
                                        SDL_Delay(200);
                                        FMOD_System_PlaySound(system, 1, DKroar, 0, NULL);
                                        FMOD_Channel_SetVolume(channel1, 0.2);
                                        animDonkey = 0;
                                    }
                                SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                                SDL_BlitSurface(donkey[donkeyPosture], NULL, ecran, &positionDK);
                                SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                            }
                        break;

                case 6:
                        if (animDonkey % 10 >= 5)
                            donkeyPosture = ATTEND2;
                        else if(animDonkey % 10 < 5)
                            donkeyPosture = PREND1;

                        SDL_BlitSurface(donkey[donkeyPosture], NULL, ecran, &positionDK);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);

                        if (animDonkey==50)
                            {
                                etape = 7;
                                positionDK.x-=30;
                                positionDK.y-=5;
                            }
                        break;

                case 7:
                        if (hauteurSaut[0]==0)
                            {
                                hauteurSaut[0] +=5;
                                positionDK.y -= 5;
                                donkeyPosture = SAUT1;
                            }
                        else if (hauteurSaut[0]>0 && hauteurSaut[0]<50)
                            {
                                hauteurSaut[0] +=5;
                                positionDK.y -= 5;
                                donkeyPosture = SAUT1;
                            }
                        else if (hauteurSaut[0]==50 && tourneDansLeVide[0]!=4)
                            {
                                tourneDansLeVide[0] ++;
                                donkeyPosture = SAUT2;
                            }
                        else if (hauteurSaut[0]==50 && tourneDansLeVide[0]==4)
                            {
                                tourneDansLeVide[0] = 0;
                                hauteurSaut[0] +=5;
                                positionDK.y += 5;
                                donkeyPosture = SAUT2;
                            }
                        else if (hauteurSaut[0]>50 && hauteurSaut[0]<105)
                            {
                                hauteurSaut[0] +=5;
                                positionDK.y += 11;
                                donkeyPosture = SAUT2;
                            }
                        else if (hauteurSaut[0]==105)
                            {
                                hauteurSaut[0] = 0;
                                donkeyPosture = SAUT3;
                                etape = 0;
                                deroulementTransition = 1;
                                FMOD_System_PlaySound(system, 1, crash, 0, NULL);
                                FMOD_Channel_SetVolume(channel1, 0.5);
                            }
                        positionDK.x-=3;
                        SDL_BlitSurface(donkey[donkeyPosture], NULL, ecran, &positionDK);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;
            }

        // TRANSITION
        if (deroulementTransition && deroulementTransition<8)
            {
                SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionStructure);
                deroulementTransition++;
            }
        else if (deroulementTransition==8)
            {
                SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionStructure);
                cinematiqueDebut = 0;
                animDonkey = 0;
                donkeyPosture = 0;
            }

        SDL_Flip(ecran);

        // QUITTER (LA PARTIE/LE JEU) :
        if (in.key[SDLK_ESCAPE])      // ECHAP ou Reduction fenêtre ou arriere plan
            {
                screenshot = SDL_SaveBMP (ecran, "Screenshot.bmp");
                memset(&in,0,sizeof(in));
                exit = pause(ecran, system, musiqueDeFond, screenshot);
                if (exit)
                    return exit;
                in.key[SDLK_ESCAPE] = 0;
            }
        if (in.quit)
            return QUITTER;

        if (in.key[SDLK_p])
            {
                SDL_Delay(500);
            }
    }



    if (level==1)
        {
            positionPeach.x-=55;
            deroulementTransition = 8;
        }
    else if (level==2)
        {
            positionPeach.x-=45;
            vitesseTonneaux = 2;
        }
    else if (level==3)
        {
            positionPeach.x-=45;
            vitesseTonneaux = 2;
        }



///////////////////////
// BOUCLE PRINCIPALE //
///////////////////////

    while(continuer)
    {
        UpdateEvents(&in);                      // Met à jour la structure comportant les booléens indiquants l'appui
                                                // ou non des touches du clavier
        SDL_Delay(35);                          // Effectue une pause de 35 ms permettant de ne pas saturer le processeur
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));      // Efface l'écran (rempli de noir)

        // CADRE : FOND ANIME, STRUCTURE ET HEADER(VIES/SCORE + NIVEAU)
        positionFond.x = deroulementFond;
        SDL_BlitSurface(fond, NULL, ecran, &positionFond);                  // Affiche le fond (décor non animé)
        SDL_BlitSurface(structure, NULL, ecran, &positionStructure);
        SDL_BlitSurface(tonneaux, NULL, ecran, &positionTonneaux);
        SDL_BlitSurface(sacs, NULL, ecran, &positionSacs);
        SDL_BlitSurface(header, NULL, ecran, &positionHeader);
        SDL_BlitSurface(texteVies, NULL, ecran, &positionTexteVies);
        SDL_BlitSurface(texteLevel, NULL, ecran, &positionTexteLevel);

        if (player2)
            {
                for (i=1;i<level;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(gagnantNiveau[i], NULL, ecran, &positionMiniature);
                    }
            }
        if (!player2)
            {
                for (i=0;i<nbVies;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(mario[J1][PETIT], NULL, ecran, &positionMiniature);
                    }
            }

        if (deroulementFond == 0)
            deroulementFond = -1160;
        else
            deroulementFond++;


    // RE-AUTHORISE UN DEPLACEMENT LATERAL OU SAUT APRES LA MONTEE/DESCENTE D'UN ECHELLE SAUF VICTOIRE AU DERNIER NIVEAU:
        for(j=0;j<=player2;j++)
            {
                if (hauteurEchelle[j] == 0)
                    deplacementMarioSurEchelle[j] = 0;
                if (positionMario[j].y <= 97)
                    win[j] = 1;
            }

    // VERIFIE S'IL Y A EU COLLISION ENTRE PERSO ET TONNEAUX :
        for(j=0;j<=player2;j++)
            {
                for(i=0;i<8;i++)
                    {
                        if (!explosionEnCours[i])
                            {
                                if (    (positionTonneau[i].x >= positionMario[j].x - 18) &&
                                        (positionTonneau[i].x <= positionMario[j].x + 18) &&
                                        (positionTonneau[i].y >= positionMario[j].y - 35) &&
                                        (positionTonneau[i].y <= positionMario[j].y + 16)
                                        &&  !clignotement[j]                                       )
                                        {
                                            if (son)
                                                {
                                                    FMOD_System_PlaySound(system, 3, expl, 0, NULL);
                                                    FMOD_Channel_SetVolume(channel3, 0.8);
                                                }
                                            lose[j] = 1;
                                            explosionEnCours[i] = 1;
                                            positionExplosion[i].x = positionTonneau[i].x - 17;
                                            positionExplosion[i].y = positionTonneau[i].y - 13;
                                        }
                                if (positionTonneau[i].x == 102 && positionTonneau[i].y == 547)
                                        {
                                            if (son)
                                                {
                                                    FMOD_System_PlaySound(system, 3, expl, 0, NULL);
                                                    FMOD_Channel_SetVolume(channel3, 0.8);
                                                }
                                            explosionEnCours[i] = 1;
                                            positionExplosion[i].x = positionTonneau[i].x - 17;
                                            positionExplosion[i].y = positionTonneau[i].y - 13;
                                        }
                            }
                        if (explosionEnCours[i])
                            {
                                positionTonneau[i].x = (LIM_BL_4_5+30*i);
                                positionTonneau[i].y = 0;

                                if (statutExplosion[i]<=7)
                                    {
                                        explosionPosture[i] = explosion[statutExplosion[i]];
                                        statutExplosion[i]++;
                                    }
                                else
                                    {
                                        explosionEnCours[i] = 0;
                                        statutExplosion[i] = 0;
                                    }
                            }
                    }
            }

    // LANCEMENTS DE TONNEAUX :
        for(i=0;i<8;i++)
            {
                if (deroulementFond==(-1152+72*i))
                    {
                        animDonkey = 1;
                        lancementTonneau = i;
                    }
            }
        for(i=0;i<8;i++)
            {
                if (deroulementFond==(-576+72*i))
                    {
                        animDonkey = 1;
                        lancementTonneau = i;
                    }
            }

        if (animDonkey)
            {
                animDonkey++;
                if (animDonkey % 4 == 0)
                    {
                        donkeyPosture = animDonkey/4;

                        if (donkeyPosture==PREND2)
                            positionTonneaux.x -= 20;
                        else if (donkeyPosture!=PREND1)
                            positionTonneaux.x += donkeyPosture/2;
                    }
            }

        if (donkeyPosture)
            SDL_BlitSurface(donkey[donkeyPosture], NULL, ecran, &positionDonkey);
        else
            SDL_BlitSurface(donkey[ATTEND], NULL, ecran, &positionDonkey);

        if (donkeyPosture==LANCE4)
            {
                positionTonneau[lancementTonneau].x = LIM_BL_3_4;
                positionTonneau[lancementTonneau].y = 163;
                tonneauPosture[lancementTonneau] = tonneau[TONNEAU2];
            }
        if (animDonkey>=37)
            {
                animDonkey = 0;
                positionTonneaux.x = 45;
            }


    // MOUVEMENT DES TONNEAUX :
        for(j=0;j<vitesseTonneaux;j++)
            {
                for(i=0;i<8;i++)
                    {
                        if (positionTonneau[i].y>0)
                            {
                                if (
                                    !deplacementTonneauSurEchelle[i]
                                    && (random(1,3)==1)
                                    && (verifEchelle(&positionTonneau[i], BAS, TONNEAU)==30 || verifEchelle(&positionTonneau[i], BAS, TONNEAU)==20)
                                    )
                                            deplacementTonneauSurEchelle[i] =1;
                                if (
                                    (verifEchelle(&positionTonneau[i], BAS, TONNEAU)==30 || verifEchelle(&positionTonneau[i], BAS, TONNEAU)==20)
                                    && deplacementTonneauSurEchelle[i]
                                    )
                                        {
                                            positionTonneau[i].y += moveMarioEchelle(&positionTonneau[i], BAS);
                                            varPostureTonneau++;

                                            if (varPostureTonneau%3==0)
                                                tonneauPosture[i] = tonneau[TONNEAU5];
                                            else if (varPostureTonneau%3==1)
                                                tonneauPosture[i] = tonneau[TONNEAU6];
                                            else
                                                tonneauPosture[i] = tonneau[TONNEAU7];
                                        }
                                else if (verifEchelle(&positionTonneau[i], BAS, TONNEAU)==10)
                                        {
                                            positionTonneau[i].y += moveMarioEchelle(&positionTonneau[i], BAS);
                                            varPostureTonneau++;

                                            if (varPostureTonneau%3==0)
                                                tonneauPosture[i] = tonneau[TONNEAU5];
                                            else if (varPostureTonneau%3==1)
                                                tonneauPosture[i] = tonneau[TONNEAU6];
                                            else
                                                tonneauPosture[i] = tonneau[TONNEAU7];
                                        }
                                else
                                        {
                                            deplacementTonneauSurEchelle[i] =0;

                                            if ((positionTonneau[i].y >= ETAGE_1_HAUT) ||
                                                (positionTonneau[i].y <= ETAGE_3_BAS && positionTonneau[i].y >= ETAGE_3_HAUT) ||
                                                (positionTonneau[i].y <= ETAGE_5_BAS && positionTonneau[i].y >= ETAGE_5_HAUT)    )
                                                        {
                                                            positionTonneau[i].y += moveMario(&positionTonneau[i], GAUCHE);
                                                            varPostureTonneau--;
                                                        }
                                            else if ((positionTonneau[i].y <= ETAGE_2_BAS && positionTonneau[i].y >= ETAGE_2_HAUT) ||
                                                     (positionTonneau[i].y <= ETAGE_4_BAS && positionTonneau[i].y >= ETAGE_4_HAUT) ||
                                                     (positionTonneau[i].y <= ETAGE_6_BAS)  )
                                                        {
                                                            positionTonneau[i].y += moveMario(&positionTonneau[i], DROITE);
                                                            varPostureTonneau++;
                                                        }

                                            if (varPostureTonneau%4==0)
                                                tonneauPosture[i] = tonneau[TONNEAU1];
                                            else if (varPostureTonneau%4==1)
                                                tonneauPosture[i] = tonneau[TONNEAU2];
                                            else if (varPostureTonneau%4==2)
                                                tonneauPosture[i] = tonneau[TONNEAU3];
                                            else
                                                tonneauPosture[i] = tonneau[TONNEAU4];
                                        }
                            }
                }
            }


    // SAUT EN COURS :
        for(j=0;j<=player2;j++)
            {
                if (saut[j])
                    {
                        if (sautLeft[j])
                            {
                                diffY[j] = moveMario(&positionMario[j], GAUCHE);
                                diffYtotale[j] += diffY[j];
                            }
                        else if (sautRight[j])
                            {
                                diffY[j] = moveMario(&positionMario[j], DROITE);
                                diffYtotale[j] += diffY[j];
                            }

                        if (saut[j])
                            {
                                if (hauteurSaut[j]==0)
                                    {
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y -= 5;
                                    }
                                else if (hauteurSaut[j]>0 && hauteurSaut[j]<30)
                                    {
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y -= 5;
                                    }
                                else if (hauteurSaut[j]==30 && tourneDansLeVide[j]!=2)
                                    {
                                        tourneDansLeVide[j] ++;
                                    }
                                else if (hauteurSaut[j]==30 && tourneDansLeVide[j]==2)
                                    {
                                        tourneDansLeVide[j] = 0;
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y += 5;
                                    }
                                else if (hauteurSaut[j]>30 && hauteurSaut[j]<55)
                                    {
                                        hauteurSaut[j] +=5;
                                        positionMario[j].y += 5;
                                    }
                                else if (hauteurSaut[j]==55)
                                    {
                                        positionMario[j].y += (60+diffYtotale[j])-55;
                                        saut[j] = 0;
                                        sautHaut[j] = 0;
                                        sautLeft[j] = 0;
                                        sautRight[j] = 0;
                                        hauteurSaut[j] = 0;
                                        diffY[j] = 0;
                                        diffYtotale[j] = 0;
                                    }
                            }
                    }
            }


    // ENCLENCHEMENT ET POSTURES DE SAUT J1 :
        if (in.key[SDLK_SPACE] && !in.key[SDLK_LEFT] && !in.key[SDLK_RIGHT] && !saut[J1] && !deplacementMarioSurEchelle[J1])
            {
                    if (son)
                        {
                            FMOD_System_PlaySound(system, 1, jump, 0, NULL);
                            FMOD_Channel_SetVolume(channel1, 0.1);
                        }
                    saut[J1] = 1;
                    sautHaut[J1] = 1;
                    positionMarioAvantSaut[J1] = positionMario[J1];

                    if (lastPosture[J1] == DROITE)
                        marioPosture[J1] = mario[J1][DROITE2];
                    else if (lastPosture[J1] == GAUCHE)
                        marioPosture[J1] = mario[J1][GAUCHE2];
            }
        if (in.key[SDLK_SPACE] && in.key[SDLK_LEFT] && !in.key[SDLK_RIGHT] && !saut[J1] && !deplacementMarioSurEchelle[J1])
            {
                     if (son)
                        {
                            FMOD_System_PlaySound(system, 1, jump, 0, NULL);
                            FMOD_Channel_SetVolume(channel1, 0.1);
                        }
                    saut[J1] = 1;
                    sautLeft[J1] = 1;
                    positionMarioAvantSaut[J1] = positionMario[J1];
                    marioPosture[J1] = mario[J1][GAUCHE2];
            }
        if (in.key[SDLK_SPACE] && in.key[SDLK_RIGHT] && !in.key[SDLK_LEFT] && !saut[J1] && !deplacementMarioSurEchelle[J1])
            {
                     if (son)
                        {
                            FMOD_System_PlaySound(system, 1, jump, 0, NULL);
                            FMOD_Channel_SetVolume(channel1, 0.1);
                        }
                    saut[J1] = 1;
                    sautRight[J1] = 1;
                    positionMarioAvantSaut[J1] = positionMario[J1];
                    marioPosture[J1] = mario[J1][DROITE2];
            }

        if (player2)
            {
                // ENCLENCHEMENT ET POSTURES DE SAUT J2 :
                    if (in.key[SDLK_TAB] && !in.key[SDLK_a] && !in.key[SDLK_d] && !saut[J2] && !deplacementMarioSurEchelle[J2])
                        {
                                if (son)
                                    {
                                        FMOD_System_PlaySound(system, 1, jump, 0, NULL);
                                        FMOD_Channel_SetVolume(channel1, 0.1);
                                    }
                                saut[J2] = 1;
                                sautHaut[J2] = 1;
                                positionMarioAvantSaut[J2] = positionMario[J2];

                                if (lastPosture[J2] == DROITE)
                                    marioPosture[J2] = mario[J2][DROITE2];
                                else if (lastPosture[J2] == GAUCHE)
                                    marioPosture[J2] = mario[J2][GAUCHE2];
                        }
                    if (in.key[SDLK_TAB] && in.key[SDLK_a] && !in.key[SDLK_d] && !saut[J2] && !deplacementMarioSurEchelle[J2])
                        {
                                 if (son)
                                    {
                                        FMOD_System_PlaySound(system, 1, jump, 0, NULL);
                                        FMOD_Channel_SetVolume(channel1, 0.1);
                                    }
                                saut[J2] = 1;
                                sautLeft[J2] = 1;
                                positionMarioAvantSaut[J2] = positionMario[J2];
                                marioPosture[J2] = mario[J2][GAUCHE2];
                        }
                    if (in.key[SDLK_TAB] && in.key[SDLK_d] && !in.key[SDLK_a] && !saut[J2] && !deplacementMarioSurEchelle[J2])
                        {
                                 if (son)
                                    {
                                        FMOD_System_PlaySound(system, 1, jump, 0, NULL);
                                        FMOD_Channel_SetVolume(channel1, 0.1);
                                    }
                                saut[J2] = 1;
                                sautRight[J2] = 1;
                                positionMarioAvantSaut[J2] = positionMario[J2];
                                marioPosture[J2] = mario[J2][DROITE2];
                        }
            }


    // DEPLACEMENTS SUR ECHELLES J1 :
        if (in.key[SDLK_UP] && !saut[J1])
            {
                hauteurEchelle[J1] = verifEchelle(&positionMario[J1], HAUT, MARIO);
                if (hauteurEchelle[J1]>0)
                    {

                    if (deplacementMarioSurEchelle[J1] == 1)
                        positionMario[J1].y += moveMarioEchelle(&positionMario[J1], HAUT);

                    else
                        {
                            positionMario[J1].y -=4;
                            deplacementMarioSurEchelle[J1] = 1;
                        }

                    varPostureMario[J1]++;
                    if (varPostureMario[J1]%2==0)
                        marioPosture[J1] = mario[J1][HAUT1];
                    else
                        marioPosture[J1] = mario[J1][HAUT2];

                    }
            }
        if (in.key[SDLK_DOWN] && !saut[J1])
            {
                hauteurEchelle[J1] = verifEchelle(&positionMario[J1], BAS, MARIO);
                if (hauteurEchelle[J1]>0)
                    {
                    if (deplacementMarioSurEchelle[J1] == 1)
                        positionMario[J1].y += moveMarioEchelle(&positionMario[J1], BAS);
                    else
                        {
                            positionMario[J1].y +=4;
                            deplacementMarioSurEchelle[J1] = 1;
                        }

                    varPostureMario[J1]++;
                    if (varPostureMario[J1]%2==0)
                        marioPosture[J1] = mario[J1][HAUT1];
                    else
                        marioPosture[J1] = mario[J1][HAUT2];

                    }
            }

        if (player2)
            {
                // DEPLACEMENTS SUR ECHELLES J2 :
                    if (in.key[SDLK_w] && !saut[J2])
                        {
                            hauteurEchelle[J2] = verifEchelle(&positionMario[J2], HAUT, MARIO);
                            if (hauteurEchelle[J2]>0)
                                {

                                if (deplacementMarioSurEchelle[J2] == 1)
                                    positionMario[J2].y += moveMarioEchelle(&positionMario[J2], HAUT);

                                else
                                    {
                                        positionMario[J2].y -=4;
                                        deplacementMarioSurEchelle[J2] = 1;
                                    }

                                varPostureMario[J2]++;
                                if (varPostureMario[J2]%2==0)
                                    marioPosture[J2] = mario[J2][HAUT1];
                                else
                                    marioPosture[J2] = mario[J2][HAUT2];

                                }
                        }
                    if (in.key[SDLK_s] && !saut[J2])
                        {
                            hauteurEchelle[J2] = verifEchelle(&positionMario[J2], BAS, MARIO);
                            if (hauteurEchelle[J2]>0)
                                {
                                if (deplacementMarioSurEchelle[J2] == 1)
                                    positionMario[J2].y += moveMarioEchelle(&positionMario[J2], BAS);
                                else
                                    {
                                        positionMario[J2].y +=4;
                                        deplacementMarioSurEchelle[J2] = 1;
                                    }

                                varPostureMario[J2]++;
                                if (varPostureMario[J2]%2==0)
                                    marioPosture[J2] = mario[J2][HAUT1];
                                else
                                    marioPosture[J2] = mario[J2][HAUT2];

                                }
                        }
            }


    // DEPLACEMENTS LATERAUX J1 :
        if (in.key[SDLK_LEFT] && !in.key[SDLK_RIGHT] && !saut[J1])
            {
                if (deplacementMarioSurEchelle[J1] == 0)
                    {
                        varPostureMario[J1]++;
                        if (varPostureMario[J1]%3==0)
                            marioPosture[J1] = mario[J1][GAUCHE1];
                        else if (varPostureMario[J1]%3==1)
                            marioPosture[J1] = mario[J1][GAUCHE2];
                        else
                            marioPosture[J1] = mario[J1][GAUCHE3];

                        positionMario[J1].y += moveMario(&positionMario[J1], GAUCHE);
                        lastPosture[J1] = GAUCHE;
                    }
            }
        if (in.key[SDLK_RIGHT] && !in.key[SDLK_LEFT] && !saut[J1])
            {
                if (deplacementMarioSurEchelle[J1] == 0)
                    {
                        varPostureMario[J1]++;
                        if (varPostureMario[J1]%3==0)
                            marioPosture[J1] = mario[J1][DROITE1];
                        else if (varPostureMario[J1]%3==1)
                            marioPosture[J1] = mario[J1][DROITE2];
                        else
                            marioPosture[J1] = mario[J1][DROITE3];

                        positionMario[J1].y += moveMario(&positionMario[J1], DROITE);
                        lastPosture[J1] = DROITE;
                    }
            }

        if (player2)
            {
                // DEPLACEMENTS LATERAUX J2 :
                    if (in.key[SDLK_a] && !in.key[SDLK_d] && !saut[J2])
                        {
                            if (deplacementMarioSurEchelle[J2] == 0)
                                {
                                    varPostureMario[J2]++;
                                    if (varPostureMario[J2]%3==0)
                                        marioPosture[J2] = mario[J2][GAUCHE1];
                                    else if (varPostureMario[J2]%3==1)
                                        marioPosture[J2] = mario[J2][GAUCHE2];
                                    else
                                        marioPosture[J2] = mario[J2][GAUCHE3];

                                    positionMario[J2].y += moveMario(&positionMario[J2], GAUCHE);
                                    lastPosture[J2] = GAUCHE;
                                }
                        }
                    if (in.key[SDLK_d] && !in.key[SDLK_a] && !saut[J2])
                        {
                            if (deplacementMarioSurEchelle[J2] == 0)
                                {
                                    varPostureMario[J2]++;
                                    if (varPostureMario[J2]%3==0)
                                        marioPosture[J2] = mario[J2][DROITE1];
                                    else if (varPostureMario[J2]%3==1)
                                        marioPosture[J2] = mario[J2][DROITE2];
                                    else
                                        marioPosture[J2] = mario[J2][DROITE3];

                                    positionMario[J2].y += moveMario(&positionMario[J2], DROITE);
                                    lastPosture[J2] = DROITE;
                                }
                        }
            }


    // CAS DE NON DEPLACEMENT J1 :
        if (!in.key[SDLK_RIGHT] && !in.key[SDLK_LEFT] && !saut[J1] && !deplacementMarioSurEchelle[J1])
            {
                if (lastPosture[J1] == DROITE)
                    marioPosture[J1] = mario[J1][DROITE1];
                else if (lastPosture[J1] == GAUCHE)
                    marioPosture[J1] = mario[J1][GAUCHE1];
            }

        if (player2)
            {
                // CAS DE NON DEPLACEMENT J2 :
                    if (!in.key[SDLK_d] && !in.key[SDLK_a] && !saut[J2] && !deplacementMarioSurEchelle[J2])
                        {
                            if (lastPosture[J2] == DROITE)
                                marioPosture[J2] = mario[J2][DROITE1];
                            else if (lastPosture[J2] == GAUCHE)
                                marioPosture[J2] = mario[J2][GAUCHE1];
                        }
            }


    // DEBUG :
    if (debug)
        {

        // DEBUG VARIABLES/POSITIONS :

            posX = saut[J2];
            posY = diffY[J2];
            posZ = diffYtotale[J2];
            posZbis = deplacementMarioSurEchelle[J2];

            sprintf(positionnementX, "saut2 : %d", posX);
            sprintf(positionnementY, "diff: %d", posY);
            sprintf(positionnementZ, "diffT: %d", posZ);
            sprintf(positionnementZbis, "dEch: %d", posZbis);

            texte = TTF_RenderText_Solid(police, positionnementX, couleurNoire);
            texte2 = TTF_RenderText_Solid(police, positionnementY, couleurNoire);
            texte3 = TTF_RenderText_Solid(police, positionnementZ, couleurNoire);
            texte4 = TTF_RenderText_Solid(police, positionnementZbis, couleurNoire);

            SDL_BlitSurface(texte, NULL, ecran, &positionTexte);
            SDL_BlitSurface(texte2, NULL, ecran, &positionTexte2);
            SDL_BlitSurface(texte3, NULL, ecran, &positionTexte3);
            SDL_BlitSurface(texte4, NULL, ecran, &positionTexte4);




    // DEBUGGING MOVE MARIO

            positionLigneTest.y = 10;

            positionLigneTest.x = LIM_GAUCHE;
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = LIM_DROITE;
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_1_2 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_2_3 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_3_4 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_4_5 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_5_6 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_6_7 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_7_8 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_8_9 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_9_10 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_10_11 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_11_12 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_12_13 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_13_14 -18);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_1_2 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_2_3 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_3_4 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_4_5 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_5_6 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_6_7 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_7_8 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_8_9 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_9_10 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_10_11 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_11_12 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_12_13 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_13_14 -12);
            SDL_BlitSurface(ligneTest, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_1_2 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_2_3 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_3_4 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_4_5 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_5_6 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_6_7 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_7_8 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_8_9 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_9_10 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_10_11 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_11_12 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_12_13 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

            positionLigneTest.x = (LIM_BL_13_14 -6);
            SDL_BlitSurface(ligneTest2, NULL, ecran, &positionLigneTest);

        }


    // COLLAGE DES SURFACES SELON LES NOUVELLES POSITIONS :
        for(j=0;j<=player2;j++)
            {
                positionCadreCollisions[j].x = positionMario[j].x - 18;
                positionCadreCollisions[j].y = positionMario[j].y - 35;

                if (clignotement[j] % 5 == 3 || clignotement[j] % 5 == 4)
                    clignotement[j]--;
                else
                    {
                        if(clignotement[j])
                            clignotement[j]--;
                        SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                    }
            }

        for(i=0;i<8;i++)
            {
                if (positionTonneau[i].y!=0)
                    SDL_BlitSurface(tonneauPosture[i], NULL, ecran, &positionTonneau[i]);
                if (debug)
                    SDL_BlitSurface(marioCadreTest, NULL, ecran, &positionTonneau[i]);
                if (explosionEnCours[i])
                    SDL_BlitSurface(explosionPosture[i], NULL, ecran, &positionExplosion[i]);
            }

        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
        SDL_BlitSurface(pierre, NULL, ecran, &positionPierre);


        if (debug)
            {
                for(j=0;j<=player2;j++)
                    {
                        SDL_BlitSurface(marioCadreTest, NULL, ecran, &positionMario[j]);
                        SDL_BlitSurface(collisionCadreTest, NULL, ecran, &positionCadreCollisions[j]);
                    }
            }

                // EFFET DE FONDU :
                if (deroulementTransition)
                    {
                        SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionStructure);
                        deroulementTransition--;
                    }

        SDL_Flip(ecran);


    // GESTION VICTOIRE / DEFAITE :
        if (!player2 && lose[J1])
            {
                nbVies--;
                if(lose[J1])
                if(nbVies == 0)
                    return GAMEOVER;
            }

        for(j=0;j<=player2;j++)
            {
                if (lose[j])
                    {
                        FMOD_System_PlaySound(system, 4, ooh, 0, NULL);
                        FMOD_Channel_SetVolume(channel4, 0.2);
                        lose[j] = 0;
                        clignotement[j] = 50;
                        marioPosture[j] = mario[j][DROITE1];
                        positionMario[j].x = positionMarioInitiale.x-(30*j);
                        positionMario[j].y = positionMarioInitiale.y;
                        deplacementMarioSurEchelle[j] = 0;
                        saut[j] = 0;
                        sautHaut[j] = 0;
                        sautLeft[j] = 0;
                        sautRight[j] = 0;
                        hauteurSaut[j] = 0;
                        diffY[j] = 0;
                        diffYtotale[j] = 0;
                    }

                if (win[j] && level==1)
                    SDL_SaveBMP(mario[j][PETIT],"Gagnant niveau 1.bmp");

                else if (win[j] && level==2)
                    SDL_SaveBMP(mario[j][PETIT],"Gagnant niveau 2.bmp");

                else if (win[j] && level==3)
                    SDL_SaveBMP(mario[j][PETIT],"Gagnant niveau 3.bmp");

                if (win[j] && level!=3)
                    {
                        continuer = 0;
                        cinematiqueLevelup = 1;
                        animDonkey = 0;
                        etape = 1;
                        gagnant = j;
                        marioPosture[j] = GAUCHE1;
                    }
                if (win[j] && level==3)
                    {
                        continuer = 0;
                        cinematiqueHappyend = 1;
                        animDonkey = 0;
                        etape = 1;
                        gagnant = j;
                        marioPosture[j] = GAUCHE1;
                    }
            }

    // QUITTER (LA PARTIE/LE JEU) :
        if (in.key[SDLK_ESCAPE] || in.focus)      // ECHAP ou Reduction fenêtre ou arriere plan
            {
                screenshot = SDL_SaveBMP (ecran, "Screenshot.bmp");
                memset(&in,0,sizeof(in));
                exit = pause(ecran, system, musiqueDeFond, screenshot);
                if (exit)
                    return exit;
                in.key[SDLK_ESCAPE] = 0;
                in.focus = 0;
            }
        if (in.quit)
            return QUITTER;

        if (in.key[SDLK_l])
            {
                level++;
                in.key[SDLK_l] =0;
            }
        if (in.key[SDLK_p])
            {
                SDL_Delay(500);
            }
        if (in.key[SDLK_o])
            {
                win[J1] = 1;
            }
    }




//////////////////////////
// CINEMATIQUE LEVEL UP //
//////////////////////////

    while (cinematiqueLevelup)
    {
        UpdateEvents(&in);

        SDL_Delay(50);
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

        // CADRE : FOND ANIME, STRUCTURE ET HEADER(VIES/SCORE + NIVEAU)
        positionFond.x = deroulementFond;
        SDL_BlitSurface(fond, NULL, ecran, &positionFond);                  // Affiche le fond (décor non animé)
        SDL_BlitSurface(structure, NULL, ecran, &positionStructure);
        SDL_BlitSurface(tonneaux, NULL, ecran, &positionTonneaux);
        SDL_BlitSurface(sacs, NULL, ecran, &positionSacs);
        SDL_BlitSurface(pierre, NULL, ecran, &positionPierre);
        SDL_BlitSurface(header, NULL, ecran, &positionHeader);
        SDL_BlitSurface(texteVies, NULL, ecran, &positionTexteVies);
        SDL_BlitSurface(texteLevel, NULL, ecran, &positionTexteLevel);

        if (player2)
            {
                for (i=1;i<level;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(gagnantNiveau[i], NULL, ecran, &positionMiniature);
                    }
            }
        if (!player2)
            {
                for (i=0;i<nbVies;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(mario[J1][PETIT], NULL, ecran, &positionMiniature);
                    }
            }

        if (deroulementFond == 0)
            deroulementFond = -1160;
        else
            deroulementFond++;

        animDonkey++;

        // CINEMATIQUE LEVEL UP

        switch(etape)
            {
                case 0:
                        SDL_BlitSurface(donkey[MONTEDROITEPEACH], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(coeurBrise, NULL, ecran, &positionCoeur);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 1:
                        positionMario[gagnant].x-=6;
                        if (animDonkey%3==0)
                            marioPosture[gagnant] = mario[gagnant][GAUCHE1];
                        else if (animDonkey%3==1)
                            marioPosture[gagnant] = mario[gagnant][GAUCHE2];
                        else
                            marioPosture[gagnant] = mario[gagnant][GAUCHE3];
                        if (animDonkey>=3)
                            {
                                animDonkey = 0;
                                donkeyPosture = 0;
                                positionDonkey.y+=5;
                                etape = 2;
                            }
                        SDL_BlitSurface(donkey[ATTEND], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 2:
                        if (animDonkey<11)
                            donkeyPosture = animDonkey;
                        else if (animDonkey>=11 && animDonkey<15)
                            donkeyPosture = animDonkey-10;
                        else if (animDonkey>=15)
                            {
                                animDonkey = 0;
                                donkeyPosture = 0;
                                positionDonkey.y-=10;
                                etape = 3;
                            }
                        positionDonkey.x+=6;
                        SDL_BlitSurface(donkeyMarche[donkeyPosture], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        SDL_BlitSurface(coeur, NULL, ecran, &positionCoeur);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 3:
                        if (animDonkey % 10 >= 5)
                            {
                                positionDonkey.y -= 3;
                                donkeyPosture = MONTEGAUCHE;
                            }
                        else if(animDonkey % 10 < 5)
                            {
                                positionDonkey.y -= 3;
                                donkeyPosture = MONTEDROITE;
                            }
                        if(animDonkey == 20)
                            {
                                animDonkey = 0;
                                donkeyPosture = 0;
                                etape = 4;
                            }
                        SDL_BlitSurface(donkey[donkeyPosture], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        SDL_BlitSurface(coeur, NULL, ecran, &positionCoeur);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 4:
                        if (animDonkey % 10 >= 5)
                            {
                                positionDonkey.y -= 3;
                                SDL_BlitSurface(donkey[MONTEGAUCHEPEACH], NULL, ecran, &positionDonkey);
                                SDL_BlitSurface(coeurBrise, NULL, ecran, &positionCoeur);
                            }
                        else if(animDonkey % 10 < 5)
                            {
                                positionDonkey.y -= 3;
                                positionHelp.x = positionDonkey.x + 55;
                                positionHelp.y = positionDonkey.y + 15;
                                SDL_BlitSurface(donkey[MONTEDROITEPEACH], NULL, ecran, &positionDonkey);
                                SDL_BlitSurface(coeurBrise, NULL, ecran, &positionCoeur);
                                SDL_BlitSurface(help, NULL, ecran, &positionHelp);
                            }
                        if(animDonkey == 20)
                            {
                                animDonkey = 0;
                                etape = 0;
                                deroulementTransition = 1;
                            }
                        if (animDonkey % 20 == 5)
                            {
                                FMOD_System_PlaySound(system, 3, peachaah, 0, NULL);
                                FMOD_Channel_SetVolume(channel3, 0.2);
                            }
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;
            }

        // TRANSITION
        if (deroulementTransition && deroulementTransition<8)
            {
                SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionStructure);
                deroulementTransition++;
                SDL_Flip(ecran);
            }
        else if (deroulementTransition==8)
            {
                SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionStructure);
                animDonkey = 0;
                donkeyPosture = 0;
                SDL_Flip(ecran);
                if (gagnant == J1 && !player2)
                    return WIN;
                if (gagnant == J1 && player2)
                    return WINJ1;
                if (gagnant == J2 && player2)
                    return WINJ2;
            }
        else
            SDL_Flip(ecran);

        // QUITTER (LA PARTIE/LE JEU) :
        if (in.key[SDLK_ESCAPE])      // ECHAP ou Reduction fenêtre ou arriere plan
            {
                screenshot = SDL_SaveBMP (ecran, "Screenshot.bmp");
                memset(&in,0,sizeof(in));
                exit = pause(ecran, system, musiqueDeFond, screenshot);
                if (exit)
                    return exit;
                in.key[SDLK_ESCAPE] = 0;
            }
        if (in.quit)
            return QUITTER;

        if (in.key[SDLK_p])
            {
                SDL_Delay(500);
            }
    }




//////////////////////////////
// CINEMATIQUE HAPPY ENDING //
//////////////////////////////

    while (cinematiqueHappyend)
    {
        UpdateEvents(&in);

        SDL_Delay(50);
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

        // CADRE : FOND ANIME, STRUCTURE ET HEADER(VIES/SCORE + NIVEAU)
        positionFond.x = deroulementFond;
        SDL_BlitSurface(fond, NULL, ecran, &positionFond);                  // Affiche le fond (décor non animé)
        SDL_BlitSurface(structure, NULL, ecran, &positionStructure);
        if (etape==1 || etape==2)
            SDL_BlitSurface(tonneaux, NULL, ecran, &positionTonneaux);
        SDL_BlitSurface(sacs, NULL, ecran, &positionSacs);
        SDL_BlitSurface(header, NULL, ecran, &positionHeader);
        SDL_BlitSurface(texteVies, NULL, ecran, &positionTexteVies);
        SDL_BlitSurface(texteLevel, NULL, ecran, &positionTexteLevel);

        if (player2)
            {
                for (i=1;i<level;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(gagnantNiveau[i], NULL, ecran, &positionMiniature);
                    }
            }
        if (!player2)
            {
                for (i=0;i<nbVies;i++)
                    {
                        positionMiniature.x = positionMiniatureInit.x + 25*i;
                        SDL_BlitSurface(mario[J1][PETIT], NULL, ecran, &positionMiniature);
                    }
            }

        if (deroulementFond == 0)
            deroulementFond = -1160;
        else
            deroulementFond++;

        animDonkey++;

        // CINEMATIQUE HAPPY ENDING

        switch(etape)
            {
                case 0:
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 1:
                        positionMario[gagnant].x-=6;
                        if (animDonkey%3==0)
                            marioPosture[gagnant] = mario[gagnant][GAUCHE1];
                        else if (animDonkey%3==1)
                            marioPosture[gagnant] = mario[gagnant][GAUCHE2];
                        else
                            marioPosture[gagnant] = mario[gagnant][GAUCHE3];
                        if (animDonkey>=14)
                            {
                                animDonkey = 0;
                                donkeyPosture = 0;
                                marioPosture[gagnant] = mario[gagnant][GAUCHE2];
                                etape = 2;
                            }
                        SDL_BlitSurface(donkey[ATTEND], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        SDL_BlitSurface(pierre, NULL, ecran, &positionPierre);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 2:
                        if (hauteurSaut[0]==0)
                            {
                                hauteurSaut[0] +=5;
                                positionPierre.y -= 5;
                            }
                        else if (hauteurSaut[0]>0 && hauteurSaut[0]<40)
                            {
                                hauteurSaut[0] +=5;
                                positionPierre.y -= 5;
                            }
                        else if (hauteurSaut[0]==40 && tourneDansLeVide[0]!=6)
                            {
                                tourneDansLeVide[0] ++;
                            }
                        else if (hauteurSaut[0]==40 && tourneDansLeVide[0]==6)
                            {
                                tourneDansLeVide[0] = 0;
                                hauteurSaut[0] +=5;
                                positionPierre.y += 5;
                            }
                        else if (hauteurSaut[0]>40 && hauteurSaut[0]<85)
                            {
                                hauteurSaut[0] +=5;
                                positionPierre.y += 11;
                            }
                        else if (hauteurSaut[0]>=85)
                            {
                                animDonkey = 0;
                                donkeyPosture = 0;
                                positionDonkey.y+=5;
                                hauteurSaut[0] = 0;
                                etape = 3;
                                positionExplosion[0].x = positionDonkey.x - 60;
                                positionExplosion[0].y = positionDonkey.y;
                                positionExplosion[1].x = positionDonkey.x - 40;
                                positionExplosion[1].y = positionDonkey.y + 10;
                                positionExplosion[2].x = positionDonkey.x - 20;
                                positionExplosion[2].y = positionDonkey.y;
                                for (i=0;i<3;i++)
                                    {
                                        explosionEnCours[i]=1;
                                        statutExplosion[i]=0;
                                        FMOD_System_PlaySound(system, 3, expl, 0, NULL);
                                        FMOD_Channel_SetVolume(channel3, 0.8);
                                    }
                            }
                        positionPierre.x-=7;
                        SDL_BlitSurface(donkey[ATTEND], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        SDL_BlitSurface(pierre, NULL, ecran, &positionPierre);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 3:
                        for (i=0;i<3;i++)
                            {
                                if (explosionEnCours[i])
                                    {
                                        if (statutExplosion[i]<=8)
                                            {
                                                explosionPosture[i] = explosion[statutExplosion[i]];
                                                statutExplosion[i]++;
                                            }
                                        else
                                            {
                                                explosionEnCours[i] = 0;
                                                statutExplosion[i] = 0;
                                            }
                                    }
                            }
                        if (hauteurSaut[0]==0)
                            {
                                hauteurSaut[0] +=5;
                                positionDonkey.y -= 5;
                                donkeyPosture = FIN1;
                                positionDonkey.x+=6;
                            }
                        else if (hauteurSaut[0]>0 && hauteurSaut[0]<50)
                            {
                                hauteurSaut[0] +=5;
                                positionDonkey.y -= 5;
                                donkeyPosture = FIN2;
                                positionDonkey.x+=6;
                            }
                        else if (hauteurSaut[0]==50 && tourneDansLeVide[0]!=2)
                            {
                                tourneDansLeVide[0] ++;
                                donkeyPosture = FIN2;
                                positionDonkey.x+=6;
                            }
                        else if (hauteurSaut[0]==50 && tourneDansLeVide[0]==2)
                            {
                                tourneDansLeVide[0] = 0;
                                hauteurSaut[0] +=5;
                                positionDonkey.y += 5;
                                donkeyPosture = FIN3;
                                positionDonkey.x+=3;
                            }
                        else if (hauteurSaut[0]>50 && hauteurSaut[0]<240)
                            {
                                hauteurSaut[0] +=5;
                                positionDonkey.y += 11;
                                donkeyPosture = FIN4;
                                positionDonkey.x+=3;
                            }
                        else if (hauteurSaut[0]>=240)
                            {
                                animDonkey = 0;
                                hauteurSaut[0] = 0;
                                positionDonkey.y += 6;
                                donkeyPosture = FIN5;
                                etape = 4;
                                FMOD_System_PlaySound(system, 1, crash, 0, NULL);
                                FMOD_Channel_SetVolume(channel1, 0.5);
                                SDL_Delay(300);
                            }
                        SDL_BlitSurface(donkey[donkeyPosture], NULL, ecran, &positionDonkey);
                        for (i=0;i<3;i++)
                            SDL_BlitSurface(explosionPosture[i], NULL, ecran, &positionExplosion[i]);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;


                case 4:
                        positionMario[gagnant].x+=6;
                        if (animDonkey%3==0)
                            marioPosture[gagnant] = mario[gagnant][DROITE1];
                        else if (animDonkey%3==1)
                            marioPosture[gagnant] = mario[gagnant][DROITE2];
                        else
                            marioPosture[gagnant] = mario[gagnant][DROITE3];
                        if (animDonkey>=12)
                            {
                                animDonkey = 0;
                                donkeyPosture = 0;
                                marioPosture[gagnant] = mario[gagnant][GAUCHE1];
                                etape = 5;
                                deroulementTransition = 1;
                            }
                        SDL_BlitSurface(donkey[FIN5], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        break;

                case 5:
                        SDL_BlitSurface(donkey[FIN5], NULL, ecran, &positionDonkey);
                        SDL_BlitSurface(peach, NULL, ecran, &positionPeach);
                        SDL_BlitSurface(coeur, NULL, ecran, &positionCoeur);
                        for(j=0;j<=player2;j++)
                            SDL_BlitSurface(marioPosture[j], NULL, ecran, &positionMario[j]);
                        SDL_Flip(ecran);
                        FMOD_System_PlaySound(system, 3, victoire, 0, NULL);
                        FMOD_Channel_SetVolume(channel3, 0.2);
                        SDL_Delay(3000);
                        etape = 0;
                        deroulementTransition = 1;
                        break;


            }

        // TRANSITION
        if (deroulementTransition && deroulementTransition<8)
            {
                SDL_BlitSurface(transition[deroulementTransition], NULL, ecran, &positionStructure);
                deroulementTransition++;
                SDL_Flip(ecran);
            }
        else if (deroulementTransition==8)
            {
                SDL_Flip(ecran);
                if (gagnant == J1 && !player2)
                    return WIN;
                if (gagnant == J1 && player2)
                    return WINJ1;
                if (gagnant == J2 && player2)
                    return WINJ2;
            }
        else
            SDL_Flip(ecran);

        // QUITTER (LA PARTIE/LE JEU) :
        if (in.key[SDLK_ESCAPE])      // ECHAP ou Reduction fenêtre ou arriere plan
            {
                screenshot = SDL_SaveBMP (ecran, "Screenshot.bmp");
                memset(&in,0,sizeof(in));
                exit = pause(ecran, system, musiqueDeFond, screenshot);
                if (exit)
                    return exit;
                in.key[SDLK_ESCAPE] = 0;
            }
        if (in.quit)
            return QUITTER;

        if (in.key[SDLK_p])
            {
                SDL_Delay(500);
            }
    }

}






int moveMario(SDL_Rect *positionMario, int directionMario)
{
    int diffY = 0;

    switch(directionMario)
    {
        case GAUCHE:

                if (positionMario->y >= ETAGE_1_HAUT)
                    {
                            if (
                                    positionMario->x == (LIM_BL_7_8 -12) ||
                                    positionMario->x == (LIM_BL_7_8 -6) ||
                                    positionMario->x == (LIM_BL_8_9 -12) ||
                                    positionMario->x == (LIM_BL_8_9 -6) ||
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -6) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -6) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -6) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -6) ||
                                    positionMario->x == (LIM_BL_13_14 -12) ||
                                    positionMario->x == (LIM_BL_13_14 -6)
                                )
                                    {
                                        diffY +=1;
                                        positionMario->x -=6;
                                    }
                            else if (positionMario->x - (LIM_BL_2_3 + 5) > 0)
                                positionMario->x -=6;

                            return diffY;
                    }

                else if ( (positionMario->y <= ETAGE_2_BAS && positionMario->y >= ETAGE_2_HAUT) ||
                          (positionMario->y <= ETAGE_4_BAS && positionMario->y >= ETAGE_4_HAUT)    )
                    {
                            if (
                                    positionMario->x == (LIM_BL_1_2 -12) ||
                                    positionMario->x == (LIM_BL_1_2 -6) ||
                                    positionMario->x == (LIM_BL_2_3 -12) ||
                                    positionMario->x == (LIM_BL_2_3 -6) ||
                                    positionMario->x == (LIM_BL_3_4 -12) ||
                                    positionMario->x == (LIM_BL_3_4 -6) ||
                                    positionMario->x == (LIM_BL_4_5 -12) ||
                                    positionMario->x == (LIM_BL_4_5 -6) ||
                                    positionMario->x == (LIM_BL_5_6 -12) ||
                                    positionMario->x == (LIM_BL_5_6 -6) ||
                                    positionMario->x == (LIM_BL_6_7 -12) ||
                                    positionMario->x == (LIM_BL_6_7 -6) ||
                                    positionMario->x == (LIM_BL_7_8 -12) ||
                                    positionMario->x == (LIM_BL_7_8 -6) ||
                                    positionMario->x == (LIM_BL_8_9 -12) ||
                                    positionMario->x == (LIM_BL_8_9 -6) ||
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -6) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -6) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -6) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -6)
                                )
                                    {
                                        diffY -=1;
                                        positionMario->x -=6;
                                    }
                            else if (positionMario->x - LIM_GAUCHE > 0)
                                positionMario->x -=6;

                            return diffY;
                    }

                else if ( (positionMario->y <= ETAGE_3_BAS && positionMario->y >= ETAGE_3_HAUT) ||
                          (positionMario->y <= ETAGE_5_BAS && positionMario->y >= ETAGE_5_HAUT)    )
                    {
                            if (
                                    positionMario->x == (LIM_BL_2_3 -12) ||
                                    positionMario->x == (LIM_BL_2_3 -6) ||
                                    positionMario->x == (LIM_BL_3_4 -12) ||
                                    positionMario->x == (LIM_BL_3_4 -6) ||
                                    positionMario->x == (LIM_BL_4_5 -12) ||
                                    positionMario->x == (LIM_BL_4_5 -6) ||
                                    positionMario->x == (LIM_BL_5_6 -12) ||
                                    positionMario->x == (LIM_BL_5_6 -6) ||
                                    positionMario->x == (LIM_BL_6_7 -12) ||
                                    positionMario->x == (LIM_BL_6_7 -6) ||
                                    positionMario->x == (LIM_BL_7_8 -12) ||
                                    positionMario->x == (LIM_BL_7_8 -6) ||
                                    positionMario->x == (LIM_BL_8_9 -12) ||
                                    positionMario->x == (LIM_BL_8_9 -6) ||
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -6) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -6) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -6) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -6) ||
                                    positionMario->x == (LIM_BL_13_14 -12) ||
                                    positionMario->x == (LIM_BL_13_14 -6)
                                )
                                    {
                                        diffY +=1;
                                        positionMario->x -=6;
                                    }
                            else if (positionMario->x - LIM_BL_1_2 > 0)
                                positionMario->x -=6;

                            return diffY;
                    }

                else if (positionMario->y <= ETAGE_6_BAS && positionMario->y >= ETAGE_6_HAUT)
                    {
                            if (
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -6) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -6) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -6) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -6)
                                )
                                    {
                                        diffY -=1;
                                        positionMario->x -=6;
                                    }
                            else if (positionMario->x - LIM_BL_4_5 > 0)
                                positionMario->x -=6;

                            return diffY;
                    }

                else if (positionMario->y <= ETAGE_7_BAS)
                    {
                            if (positionMario->x - LIM_BL_6_7 > 0)
                                positionMario->x -=6;

                            return diffY;
                    }

        break;

        case DROITE:

                if (positionMario->y >= ETAGE_1_HAUT)
                    {
                            if (
                                    positionMario->x == (LIM_BL_7_8 -12) ||
                                    positionMario->x == (LIM_BL_7_8 -18) ||
                                    positionMario->x == (LIM_BL_8_9 -12) ||
                                    positionMario->x == (LIM_BL_8_9 -18) ||
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -18) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -18) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -18) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -18) ||
                                    positionMario->x == (LIM_BL_13_14 -12) ||
                                    positionMario->x == (LIM_BL_13_14 -18)
                                )
                                    {
                                        diffY -=1;
                                        positionMario->x +=6;
                                    }
                            else if (LIM_DROITE - (positionMario->x +26) > 0)
                                positionMario->x +=6;

                            return diffY;
                    }

                else if ( (positionMario->y <= ETAGE_2_BAS && positionMario->y >= ETAGE_2_HAUT) ||
                          (positionMario->y <= ETAGE_4_BAS && positionMario->y >= ETAGE_4_HAUT)    )
                    {
                            if (
                                    positionMario->x == (LIM_BL_1_2 -12) ||
                                    positionMario->x == (LIM_BL_1_2 -18) ||
                                    positionMario->x == (LIM_BL_2_3 -12) ||
                                    positionMario->x == (LIM_BL_2_3 -18) ||
                                    positionMario->x == (LIM_BL_3_4 -12) ||
                                    positionMario->x == (LIM_BL_3_4 -18) ||
                                    positionMario->x == (LIM_BL_4_5 -12) ||
                                    positionMario->x == (LIM_BL_4_5 -18) ||
                                    positionMario->x == (LIM_BL_5_6 -12) ||
                                    positionMario->x == (LIM_BL_5_6 -18) ||
                                    positionMario->x == (LIM_BL_6_7 -12) ||
                                    positionMario->x == (LIM_BL_6_7 -18) ||
                                    positionMario->x == (LIM_BL_7_8 -12) ||
                                    positionMario->x == (LIM_BL_7_8 -18) ||
                                    positionMario->x == (LIM_BL_8_9 -12) ||
                                    positionMario->x == (LIM_BL_8_9 -18) ||
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -18) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -18) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -18) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -18)
                                )
                                    {
                                        diffY +=1;
                                        positionMario->x +=6;
                                    }
                            else if (LIM_DROITE - (positionMario->x + 62) > 0)
                                positionMario->x +=6;

                            return diffY;
                    }

                else if ( (positionMario->y <= ETAGE_3_BAS && positionMario->y >= ETAGE_3_HAUT) ||
                          (positionMario->y <= ETAGE_5_BAS && positionMario->y >= ETAGE_5_HAUT)    )
                    {
                            if (
                                    positionMario->x == (LIM_BL_2_3 -12) ||
                                    positionMario->x == (LIM_BL_2_3 -18) ||
                                    positionMario->x == (LIM_BL_3_4 -12) ||
                                    positionMario->x == (LIM_BL_3_4 -18) ||
                                    positionMario->x == (LIM_BL_4_5 -12) ||
                                    positionMario->x == (LIM_BL_4_5 -18) ||
                                    positionMario->x == (LIM_BL_5_6 -12) ||
                                    positionMario->x == (LIM_BL_5_6 -18) ||
                                    positionMario->x == (LIM_BL_6_7 -12) ||
                                    positionMario->x == (LIM_BL_6_7 -18) ||
                                    positionMario->x == (LIM_BL_7_8 -12) ||
                                    positionMario->x == (LIM_BL_7_8 -18) ||
                                    positionMario->x == (LIM_BL_8_9 -12) ||
                                    positionMario->x == (LIM_BL_8_9 -18) ||
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -18) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -18) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -18) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -18) ||
                                    positionMario->x == (LIM_BL_13_14 -12) ||
                                    positionMario->x == (LIM_BL_13_14 -18)
                                )
                                    {
                                        diffY -=1;
                                        positionMario->x +=6;
                                    }
                            else if (LIM_DROITE - (positionMario->x +26) > 0)
                                positionMario->x +=6;

                            return diffY;
                    }


                else if (positionMario->y <= ETAGE_6_BAS && positionMario->y >= ETAGE_6_HAUT)
                    {
                            if (
                                    positionMario->x == (LIM_BL_9_10 -12) ||
                                    positionMario->x == (LIM_BL_9_10 -18) ||
                                    positionMario->x == (LIM_BL_10_11 -12) ||
                                    positionMario->x == (LIM_BL_10_11 -18) ||
                                    positionMario->x == (LIM_BL_11_12 -12) ||
                                    positionMario->x == (LIM_BL_11_12 -18) ||
                                    positionMario->x == (LIM_BL_12_13 -12) ||
                                    positionMario->x == (LIM_BL_12_13 -18)
                                )
                                    {
                                        diffY +=1;
                                        positionMario->x +=6;
                                    }
                            else if (LIM_DROITE - (positionMario->x + 62) > 0)
                                positionMario->x +=6;

                            return diffY;
                    }


                else if (positionMario->y <= ETAGE_7_BAS)
                    {
                        if (LIM_DROITE - (positionMario->x + 200) > 0)
                                positionMario->x +=6;

                            return diffY;
                    }

        break;
    }
}


int verifEchelle(SDL_Rect *positionMario, int directionMario, int sujet)
{
    int hauteurEchelle = 0;
    switch(sujet)
    {
            case MARIO:
                switch(directionMario)
                {
                    case HAUT:
                            if (positionMario->x >=450 && positionMario->x <=462 && positionMario->y >=481 && positionMario->y <=537)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 456;
                                }
                            else if (positionMario->x >=96 && positionMario->x <=108 && positionMario->y >=403 && positionMario->y <=461)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 102;
                                }
                            else if (positionMario->x >=450 && positionMario->x <=462 && positionMario->y >=327 && positionMario->y <=385)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 456;
                                }
                            else if (positionMario->x >=96 && positionMario->x <=108 && positionMario->y >=249 && positionMario->y <=307)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 102;
                                }
                            else if (positionMario->x >=450 && positionMario->x <=462 && positionMario->y >=171 && positionMario->y <=229)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 456;
                                }
                            else if (positionMario->x >=246 && positionMario->x <=258 && positionMario->y >=395 && positionMario->y <=469)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 252;
                                }
                            else if (positionMario->x >=294 && positionMario->x <=306 && positionMario->y >=319 && positionMario->y <=391)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 300;
                                }
                            else if (positionMario->x >=186 && positionMario->x <=198 && positionMario->y >=245 && positionMario->y <=311)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 192;
                                }
                            else if (positionMario->x >=318 && positionMario->x <=330 && positionMario->y >=99 && positionMario->y <=163)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 324;
                                }

                    break;

                    case BAS:
                            if (positionMario->x >=450 && positionMario->x <=462 && positionMario->y >=479 && positionMario->y <=535)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 456;
                                }
                            else if (positionMario->x >=96 && positionMario->x <=108 && positionMario->y >=401 && positionMario->y <=459)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 102;
                                }
                            else if (positionMario->x >=450 && positionMario->x <=462 && positionMario->y >=325 && positionMario->y <=381)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 456;
                                }
                            else if (positionMario->x >=96 && positionMario->x <=108 && positionMario->y >=247 && positionMario->y <=305)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 102;
                                }
                            else if (positionMario->x >=450 && positionMario->x <=462 && positionMario->y >=169 && positionMario->y <=227)
                                {
                                    hauteurEchelle = 10;
                                    positionMario->x = 456;
                                }
                            else if (positionMario->x >=246 && positionMario->x <=258 && positionMario->y >=393 && positionMario->y <=467)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 252;
                                }
                            else if (positionMario->x >=294 && positionMario->x <=306 && positionMario->y >=317 && positionMario->y <=389)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 300;
                                }
                            else if (positionMario->x >=186 && positionMario->x <=198 && positionMario->y >=243 && positionMario->y <=309)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 192;
                                }
                            else if (positionMario->x >=318 && positionMario->x <=330 && positionMario->y >=97 && positionMario->y <=161)
                                {
                                    hauteurEchelle = 20;
                                    positionMario->x = 324;
                                }

                    break;
                }

            break;


            case TONNEAU:

                    if (positionMario->x == 456 && positionMario->y >=479 && positionMario->y <=535)
                        {
                            hauteurEchelle = 10;
                        }
                    else if (positionMario->x == 102 && positionMario->y >=401 && positionMario->y <=459)
                        {
                            hauteurEchelle = 10;
                        }
                    else if (positionMario->x == 456 && positionMario->y >=325 && positionMario->y <=381)
                        {
                            hauteurEchelle = 10;
                        }
                    else if (positionMario->x == 102 && positionMario->y >=247 && positionMario->y <=305)
                        {
                            hauteurEchelle = 10;
                        }
                    else if (positionMario->x == 456 && positionMario->y >=169 && positionMario->y <=227)
                        {
                            hauteurEchelle = 10;
                        }
                    else if (positionMario->x == 252 && positionMario->y >=393 && positionMario->y <=467)
                        {
                            hauteurEchelle = 20;
                        }
                    else if (positionMario->x == 300 && positionMario->y >=317 && positionMario->y <=389)
                        {
                            hauteurEchelle = 20;
                        }
                    else if (positionMario->x == 192 && positionMario->y >=243 && positionMario->y <=309)
                        {
                            hauteurEchelle = 20;
                        }
                    else if (positionMario->x == 324 && positionMario->y >=97 && positionMario->y <=161)
                        {
                            hauteurEchelle = 20;
                        }
                    else if (positionMario->x == 174 && positionMario->y >=311 && positionMario->y <=395)
                        {
                            hauteurEchelle = 30;
                        }
                    else if (positionMario->x == 420 && positionMario->y >=231 && positionMario->y <=321)
                        {
                            hauteurEchelle = 30;
                        }
                    else if (positionMario->x == 234 && positionMario->y >=163 && positionMario->y <=239)
                        {
                            hauteurEchelle = 30;
                        }

            break;
    }

    return hauteurEchelle;
}


int moveMarioEchelle(SDL_Rect *positionMario, int directionMario)
{
    int diffY = 0;

    switch(directionMario)
    {
        case HAUT:
            diffY -=2;
            break;

        case BAS:
            diffY +=2;
            break;
    }
    return diffY;
}


int random(int x, int y)
{
    y++;
    return rand()%(y-x) +x;
}


int pause(SDL_Surface *ecran, FMOD_SYSTEM *system, FMOD_CHANNEL *musiqueDeFond, SDL_Surface *screenshot)
{
    SDL_Surface *reprendre = NULL, *retourAuMenu = NULL, *options = NULL, *quitter = NULL, *fond = NULL, *structure = NULL;
    SDL_Surface *debug_debonSonoff = NULL, *debug_debonSonon = NULL, *debug_deboffSonoff = NULL, *debug_deboffSonon = NULL;
    SDL_Surface *son_debonSonoff = NULL, *son_debonSonon = NULL, *son_deboffSonoff = NULL, *son_deboffSonon = NULL;
    SDL_Rect positionReprendre, positionRetourAuMenu, positionOptions, positionQuitter, positionFond, positionStructure;
    SDL_Rect positionDebug_DebonSonoff, positionDebug_DebonSonon, positionDebug_DeboffSonoff, positionDebug_DeboffSonon;
    SDL_Rect positionSon_DebonSonoff, positionSon_DebonSonon, positionSon_DeboffSonoff, positionSon_DeboffSonon;

    fond = SDL_LoadBMP ("Screenshot.bmp");

    positionFond.x = 0;
    positionFond.y = 0;

    structure = IMG_Load("menus et fonds/fond.png");
    positionStructure.x = 0;
    positionStructure.y = 0;

    reprendre = IMG_Load("menus et fonds/menu pause/reprendre.png");
    positionReprendre.x = 0;
    positionReprendre.y = 0;

    retourAuMenu = IMG_Load("menus et fonds/menu pause/retour au menu.png");
    positionRetourAuMenu.x = 0;
    positionRetourAuMenu.y = 0;

    options = IMG_Load("menus et fonds/menu pause/options.png");
    positionOptions.x = 0;
    positionOptions.y = 0;

    quitter = IMG_Load("menus et fonds/menu pause/quitter.png");
    positionQuitter.x = 0;
    positionQuitter.y = 0;

    debug_debonSonon = IMG_Load("menus et fonds/menu pause/debug_debonSonon.png");
    positionDebug_DebonSonon.x = 0;
    positionDebug_DebonSonon.y = 0;

    debug_debonSonoff = IMG_Load("menus et fonds/menu pause/debug_debonSonoff.png");
    positionDebug_DebonSonoff.x = 0;
    positionDebug_DebonSonoff.y = 0;

    debug_deboffSonon = IMG_Load("menus et fonds/menu pause/debug_deboffSonon.png");
    positionDebug_DeboffSonon.x = 0;
    positionDebug_DeboffSonon.y = 0;

    debug_deboffSonoff = IMG_Load("menus et fonds/menu pause/debug_deboffSonoff.png");
    positionDebug_DeboffSonoff.x = 0;
    positionDebug_DeboffSonoff.y = 0;

    son_debonSonon = IMG_Load("menus et fonds/menu pause/son_debonSonon.png");
    positionSon_DebonSonon.x = 0;
    positionSon_DebonSonon.y = 0;

    son_debonSonoff = IMG_Load("menus et fonds/menu pause/son_debonSonoff.png");
    positionSon_DebonSonoff.x = 0;
    positionSon_DebonSonoff.y = 0;

    son_deboffSonon = IMG_Load("menus et fonds/menu pause/son_deboffSonon.png");
    positionSon_DeboffSonon.x = 0;
    positionSon_DeboffSonon.y = 0;

    son_deboffSonoff = IMG_Load("menus et fonds/menu pause/son_deboffSonoff.png");
    positionSon_DeboffSonoff.x = 0;
    positionSon_DeboffSonoff.y = 0;

    Input in;
    memset(&in,0,sizeof(in));

    int varMenu1 = 0;
    int continuer = 1;
    int exit = 1;
    int menu2 = 0;
    int varMenu2 = 0;

    // Son
    FMOD_SOUND *bip;
    FMOD_System_CreateSound(system, "sons/bipmenu.mp3", FMOD_CREATESAMPLE, 0, &bip);

	FMOD_CHANNEL *bipmenu;
	FMOD_System_GetChannel(system, 3, &bipmenu);


    while(continuer)
        {
            UpdateEvents(&in);
            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
            SDL_BlitSurface(fond, NULL, ecran, &positionFond);

            if (in.quit)
                return QUITTER;

            switch(menu2)
            {
                case 0:

                        if (in.key[SDLK_ESCAPE] || (in.key[SDLK_RETURN] && varMenu1 % 4 == 0))
                            return PRECEDENT;

                        if ((in.key[SDLK_RETURN] && (varMenu1 % 4 == 1 || varMenu1 % 4 == -3)) || in.quit)
                            return RETOURMENU;

                        if (in.key[SDLK_RETURN] && (varMenu1 % 4 == 2 || varMenu1 % 4 == -2))
                            {
                                // Options
                                menu2 = 1;
                                in.key[SDLK_RETURN] = 0;
                            }
                        if (in.key[SDLK_RETURN] && (varMenu1 % 4 == 3 || varMenu1 % 4 == -1))
                            return QUITTER;

                        if (in.key[SDLK_UP])
                            {
                                if (son)
                                    {
                                        FMOD_System_PlaySound(system, 3, bip, 0, NULL);
                                        FMOD_Channel_SetVolume(bipmenu, 0.2);
                                    }
                                varMenu1--;
                                in.key[SDLK_UP] = 0;
                            }
                        if (in.key[SDLK_DOWN])
                            {
                                if (son)
                                    {
                                        FMOD_System_PlaySound(system, 3, bip, 0, NULL);
                                        FMOD_Channel_SetVolume(bipmenu, 0.2);
                                    }
                                varMenu1++;
                                in.key[SDLK_DOWN] = 0;
                            }


                        if (varMenu1 % 4 == 0)
                            SDL_BlitSurface(reprendre, NULL, ecran, &positionReprendre);
                        if (varMenu1 % 4 == 1 || varMenu1 % 4 == -3)
                            SDL_BlitSurface(retourAuMenu, NULL, ecran, &positionRetourAuMenu);
                        if (varMenu1 % 4 == 2 || varMenu1 % 4 == -2)
                            SDL_BlitSurface(options, NULL, ecran, &positionOptions);
                        if (varMenu1 % 4 == 3 || varMenu1 % 4 == -1)
                            SDL_BlitSurface(quitter, NULL, ecran, &positionQuitter);

                        break;


                case 1:

                        if (in.key[SDLK_ESCAPE])
                            {
                                menu2 = 0;
                                in.key[SDLK_ESCAPE] = 0;
                            }
                        if (in.key[SDLK_UP] || in.key[SDLK_DOWN])
                            {
                                if (son)
                                    {
                                        FMOD_System_PlaySound(system, 3, bip, 0, NULL);
                                        FMOD_Channel_SetVolume(bipmenu, 0.2);
                                    }
                                varMenu2++;
                                in.key[SDLK_UP] = 0;
                                in.key[SDLK_DOWN] = 0;
                            }
                        if ((in.key[SDLK_RETURN] || in.key[SDLK_LEFT] || in.key[SDLK_RIGHT]) && varMenu2 % 2 == 0)
                            {
                                // Son
                                in.key[SDLK_LEFT] = 0;
                                in.key[SDLK_RIGHT] = 0;
                                in.key[SDLK_RETURN] = 0;
                                if (son)
                                    {
                                        son = 0;
                                        FMOD_Channel_SetPaused(musiqueDeFond, 1);
                                    }
                                else
                                    {
                                        son = 1;
                                        FMOD_Channel_SetPaused(musiqueDeFond, 0);
                                    }
                            }
                        if ((in.key[SDLK_RETURN] || in.key[SDLK_LEFT] || in.key[SDLK_RIGHT]) && varMenu2 % 2 == 1)
                            {
                                // Debug
                                in.key[SDLK_LEFT] = 0;
                                in.key[SDLK_RIGHT] = 0;
                                in.key[SDLK_RETURN] = 0;
                                if (debug)
                                    debug = 0;
                                else
                                    debug = 1;
                            }

                        switch(varMenu2 % 2)
                        {
                            case 0:

                                if (!debug && son)
                                    SDL_BlitSurface(son_deboffSonon, NULL, ecran, &positionSon_DeboffSonon);
                                if (debug && son)
                                    SDL_BlitSurface(son_debonSonon, NULL, ecran, &positionSon_DebonSonon);
                                if (!debug && !son)
                                    SDL_BlitSurface(son_deboffSonoff, NULL, ecran, &positionSon_DeboffSonoff);
                                if (debug && !son)
                                    SDL_BlitSurface(son_debonSonoff, NULL, ecran, &positionSon_DebonSonoff);
                                break;

                            case 1:

                                if (!debug && son)
                                    SDL_BlitSurface(debug_deboffSonon, NULL, ecran, &positionDebug_DeboffSonon);
                                if (debug && son)
                                    SDL_BlitSurface(debug_debonSonon, NULL, ecran, &positionDebug_DebonSonon);
                                if (!debug && !son)
                                    SDL_BlitSurface(debug_deboffSonoff, NULL, ecran, &positionDebug_DeboffSonoff);
                                if (debug && !son)
                                    SDL_BlitSurface(debug_debonSonoff, NULL, ecran, &positionDebug_DebonSonoff);

                                break;
                        }

                break;
            }

            SDL_Delay(35);
            SDL_Flip(ecran);

        }
}
