#include <windows.h>
#include "OrgaPartie.h"


int main()
{
    ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); //plein écran

//-------------------------------------------------INITIALISATOINS----------------------------------------------------

    int nb_joueur;
    demander_nb_joueur(&nb_joueur);

    int joueur_venant_de_jouer=0;
    // attention de 0 à 7 pas 1 à 8

    int hauteur_camel[5]={0,0,0,0,0};
    int case_camel[5]={0,0,0,0,0};
    int dede[5]={1,1,1,1,1};
    //1 signifie dans la pyramide, 0 en dehors

    int* argent;
    int* desert_joueur;
    int* carte_pyramide;
    argent= (int*)malloc(sizeof(int)*nb_joueur);//dimensionnement des tableaux a nb_joueur
    desert_joueur= (int*)malloc(sizeof(int)*nb_joueur);
    carte_pyramide= (int*)malloc(sizeof(int)*nb_joueur);

    s_pari_parti mise_parti;
    mise_parti.premier= (int*)malloc(sizeof(int)*nb_joueur);//dimensionnement des tableaux a nb_joueur
    mise_parti.dernier= (int*)malloc(sizeof(int)*nb_joueur);
    mise_parti.ordre_premier= (int*)malloc(sizeof(int)*nb_joueur);
    mise_parti.ordre_dernier= (int*)malloc(sizeof(int)*nb_joueur);

    s_mise_manche chameau_mise;
    //initialisation de toute les variables
    init_mise_manche(&chameau_mise);
    initialiser(argent,nb_joueur,0);
    initialiser(desert_joueur,nb_joueur,0);
    initialiser(carte_pyramide,nb_joueur,0);
    initialiser(mise_parti.premier,nb_joueur,-1);
    initialiser(mise_parti.dernier,nb_joueur,-1);
    initialiser(mise_parti.ordre_premier,nb_joueur,-1);
    initialiser(mise_parti.ordre_dernier,nb_joueur,-1);

//------------------------------------------------------PROGRAMME---------------------------------------------------------------------

    debut_partie(&nb_joueur,hauteur_camel,case_camel,desert_joueur,carte_pyramide,argent,dede,&chameau_mise);
    affichage_plateau(hauteur_camel,case_camel,desert_joueur,dede,nb_joueur,carte_pyramide,argent,chameau_mise,mise_parti);
    partie(nb_joueur,hauteur_camel,case_camel,desert_joueur,carte_pyramide,argent,dede,&joueur_venant_de_jouer,&chameau_mise,&mise_parti,desert_joueur);
    fin_de_partie(hauteur_camel,case_camel,desert_joueur,dede,nb_joueur,carte_pyramide,argent,chameau_mise,mise_parti);
}
