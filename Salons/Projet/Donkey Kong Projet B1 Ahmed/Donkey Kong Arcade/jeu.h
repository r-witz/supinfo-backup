#ifndef JEU_H_INCLUDED
#define JEU_H_INCLUDED


int game(SDL_Surface *ecran, FMOD_SYSTEM *system, FMOD_CHANNEL *musiqueDeFond, int player2, int persoChoisi, int perso2Choisi, int level);
int moveMario(SDL_Rect *positionMario, int directionMario);
int verifEchelle(SDL_Rect *positionMario, int directionMario, int sujet);
int moveMarioEchelle(SDL_Rect *positionMario, int directionMario);
int random(int x, int y);
int pause(SDL_Surface *ecran, FMOD_SYSTEM *system, FMOD_CHANNEL *musiqueDeFond, SDL_Surface *screenshot);


#endif // JEU_H_INCLUDED
