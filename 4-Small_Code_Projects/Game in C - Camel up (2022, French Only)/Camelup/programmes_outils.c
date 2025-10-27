#include <stdio.h>
#include <stdlib.h>

#include "types_structures.h"


int conv(int pc) //renvoie un char correspondant au joueur (si le nombre envoyé est -1 alors il renvoi le charactère ensemble vide
{
    if (pc== -1)
        return 155;
    else
        return 49+pc;
}


int valeur_absolue(int st)
{
    if (st<0)
        return -st;
        else
            return st;
}


void initialiser (int tab[], int nb,int i) //affecte aux nb premières cases du tableau la valeure i
{
    for (int a=0;a<nb;a++)
            {
            tab[a]=i;
            }
}


void affectation_info(s_info_camel* info_camel_,const int case_camel___[],const int hauteur_camel___[],int f) //collecte et stocke des infos
{
    (*info_camel_).case_=case_camel___[f];
    (*info_camel_).hauteur=hauteur_camel___[f];
    (*info_camel_).couleur=f;
}


void init_mise_manche(s_mise_manche* camel_miser_)//remet tout les paries à 0
{
    for(int i=0;i<3;i++)
    {
        (*camel_miser_).mise_rouge[i]=-1;
        (*camel_miser_).mise_bleu[i]=-1;
        (*camel_miser_).mise_blanc[i]=-1;
        (*camel_miser_).mise_jaune[i]=-1;
        (*camel_miser_).mise_vert[i]=-1;

    }
}


void demander_nb_joueur(int *nb_joueur_)
{
    printf("Bienvenue dans CamelUP,un jeu de 2 a 8 joueurs, combien de joueurs etes vous? ");
    scanf("%d",nb_joueur_);
    while (*nb_joueur_<2||*nb_joueur_>8)//condition nombre de joueur
    {
        printf("Vous ne pouvez pas jouez a CamelUp a %d, veuillez choisir un nombre de joueurs entre 2 et 8.",*nb_joueur_);
        scanf("%d",nb_joueur_);
    }
}


void Tri(const int flouz[],int nb,int classement_j[]) //regarde dans le tableau flouz(argent) et renvoie dans le tableau classement_j dans l'ordre les numéros des joueurs du plus riche au plus pauvre
{
    int* tab;
    tab= (int*)malloc(sizeof(int)*nb);

    for (int f=0;f<nb;f++) //recopie flouz(argent) dans un tableau temporaire
        tab[f]=flouz[f];

    int stock;
    int joueur;
    for (int r=0;r<nb;r++) //boucle de tri
    {
        stock=0;
        joueur=0;
        for (int i=0;i<nb;i++)
        {
        if (tab[i]>=stock)
        {
            stock=tab[i];
            joueur=i;
        }
        }
        classement_j[r]=joueur;
        tab[joueur]=-1; // donne à la case r la valeur -1 pour ne pas être dérangé par cette valeur à la prochaine réitération de la boucle
    }
}
