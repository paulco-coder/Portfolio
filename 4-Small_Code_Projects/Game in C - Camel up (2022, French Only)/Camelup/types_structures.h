#ifndef TYPES_STRUCTURES_H_INCLUDED
#define TYPES_STRUCTURES_H_INCLUDED


struct mise_manche{
        int mise_rouge[3];
        int mise_bleu[3];
        int mise_blanc[3];
        int mise_jaune[3];
        int mise_vert[3];
        };
  typedef struct mise_manche s_mise_manche;



struct info_camel{
   int hauteur;
   int case_;
   int couleur;
};
 typedef struct info_camel s_info_camel;



 struct pari_parti{
 int* premier;//premier[0]=3>>> le joueur 1 a miser sur le chameau 3(le jaune) qui arrive en premier
 int* ordre_premier;//ordre_premier[0]=6>>>le joueur 7 est le premier a avoir parier sur le chameau qui finira premier
 int* dernier;
 int* ordre_dernier;
 };
 typedef struct pari_parti s_pari_parti;


#endif // TYPES_STRUCTURES_H_INCLUDED


