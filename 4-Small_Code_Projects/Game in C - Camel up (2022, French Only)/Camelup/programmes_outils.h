#ifndef PROGRAMMES_OUTILS_H_INCLUDED
#define PROGRAMMES_OUTILS_H_INCLUDED
#include "types_structures.h"


int conv(int pc);
int valeur_absolue(int st);

void initialiser (int tab[],int nb,int i);
void demander_nb_joueur(int *nb_joueur_);
void affectation_info(s_info_camel* info_camel_,const int case_camel___[],const int hauteur_camel___[],int f);
void init_mise_manche(s_mise_manche* camel_miser_);


#endif // PROGRAMMES_OUTILS_H_INCLUDED
