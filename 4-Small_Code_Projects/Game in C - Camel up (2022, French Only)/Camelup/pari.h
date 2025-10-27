#ifndef PARI_H_INCLUDED
#define PARI_H_INCLUDED
#include "types_structures.h"

void classement(int classement[],const int case_camel__[],const int hauteur_camel__[]);
void recompense_pari_manche(int money_[],const int case_chamcham__[],const int hauteur_chamcham__[],const s_mise_manche chamcham_mise,const int nb_joujou);
void pari_manche(const int numero_joueur__,s_mise_manche *camel_miser);
void pari_partie(s_pari_parti* mise_parti__,const int joueur_jouant,const int nombre_joueurs);
void classement_parti(int classement_parti_[],const int case_chameau___[],const int hauteur_chameau___[]);
void recompense_pari_parti(int Yen[],const int case_chameau__[],const int hauteur_chameau__[],const int nombre_de_jugador,const s_pari_parti mise_parti___);

#endif // PARI_H_INCLUDED
