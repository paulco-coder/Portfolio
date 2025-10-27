#ifndef ORGAPARTIE_H_INCLUDED
#define ORGAPARTIE_H_INCLUDED

#include "types_structures.h"

void tour_joueur(int nb_joueur_,int hauteur_camel_[], int case_camel_[], int desert_joueur_[], int carte_pyramide_[], int argent_[], int dede_[],int *numero_joueur_,s_mise_manche *chameau_mise_,s_pari_parti* mise_parti_);
void debut_partie(int *nb_joueur_,int hauteur_camel_[], int case_camel_[], int desert_joueur_[], int carte_pyramide_[], int argent_[], int dede_[],s_mise_manche *chameau_mise_);
void fin_de_manche(int dede[],int money[],int carte_pyra[],int desert[],int nb_jou,s_mise_manche *chameau_mise_);
void partie(int nb_joueur_a,int hauteur_camel_a[], int case_camel_a[], int desert_joueur_a[], int carte_pyramide_a[], int argent_a[], int dede_a[],int *numero_joueur_a,s_mise_manche *chameau_mise_a,s_pari_parti* mise_parti_a,int carte_desert_a[]);
int condition_fin_jeu(const int tab[],int nb);

#endif // ORGAPARTIE_H_INCLUDED
