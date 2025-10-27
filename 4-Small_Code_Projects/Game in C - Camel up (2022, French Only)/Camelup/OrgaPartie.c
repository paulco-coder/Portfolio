#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types_structures.h"
#include "programmes_outils.h"
#include "affichage.h"
#include "OrgaPartie.h"
#include "pari.h"
#include "mouvements.h"


void fin_de_manche(int dede[],int money[],int carte_pyra[],int desert[],int nb_jou,s_mise_manche *chameau_mise_)
{

        Sleep(1500);
        system("cls");
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------------------FIN DE MANCHE-------------------------------------------------\n\n\n");
        Sleep(1500);
        system("cls");
        initialiser(dede,5,1);
       for (int v=0;v<nb_jou;v++) //donne 1 £ par carte pyramide et met les cartes pyramide à 0
         {
             money[v]=money[v]+carte_pyra[v];
             carte_pyra[v]=0;
             desert[v]=0;
         }
}


void tour_joueur(int nb_joueur_,int hauteur_camel_[], int case_camel_[], int desert_joueur_[], int carte_pyramide_[], int argent_[], int dede_[],int *numero_joueur_,s_mise_manche *chameau_mise_,s_pari_parti* mise_parti_)//demande au joueur ce qu'il veut faire et appele le sous programme correspondant
{
    int choix_joueur;

    printf("Tour du joueur %d, que voulez vous faire: \nmiser sur un chameau en fin de manche-> entrez 1 \nmiser sur un chameau en fin de partie-> entrez 2 \ntirer un de-> entrez 3 \nplacer/deplacer une carte desert-> entrez 4\n",*numero_joueur_+1);
    scanf("%d",&choix_joueur);
    while(choix_joueur<1||choix_joueur>4)//force le joueur à entrer un nombre entre 3 et 4
    {
        printf("\nVeuillez entrez un nombre entre 1 et 4 : \n");
        scanf("%d",&choix_joueur);
    }
    if(((*mise_parti_).premier[*numero_joueur_]!=-1 && (*mise_parti_).dernier[*numero_joueur_]!=-1) && choix_joueur==2)//empeche le joueur de parier sur la partie si il a deja parie sur le premier et le dernier chameau, pour qu'il ne perde pas sont tour
    {
        while(choix_joueur==2 || (choix_joueur<1||choix_joueur>4))
    {
        printf("\nVous avez deja parie sur le premier et le dernier chameau. Veuillez entrez un nombre entre 1 et 4 (different de 2): \n");
        scanf("%d",&choix_joueur);
    }
    }
    switch (choix_joueur)//les differentes option de jeu possible
    {
    case 1://parier sur le premier chameau en fin de manche
        pari_manche(*numero_joueur_,chameau_mise_);
        break;
    case 2://parier sur le premier ou le dernier chameau en fin de parti
        pari_partie(mise_parti_,*numero_joueur_,nb_joueur_);
        break;
    case 3://tirer un de
        pyramide(dede_,case_camel_,desert_joueur_,hauteur_camel_,carte_pyramide_,*numero_joueur_,argent_,nb_joueur_);
        Sleep(1500);
        break;
    case 4://placer/deplacer une carte oasis ou desert
        printf("Pour placer/deplacer une carte oasis, entrez le numero de la case ou vous souhaitez placez la carte.\nPour placez une carte mirage, entrez \"-\" suivi du numero de la case ou vous souhaitez placez la carte.\nVous avez choisi:");
        scanf("%d",&desert_joueur_[*numero_joueur_]);

        while (valeur_absolue(desert_joueur_[*numero_joueur_])>15||valeur_absolue(desert_joueur_[*numero_joueur_])<2)//force a placer la carte sur le plateau
        {
           printf("La case doit etre comprise entre 2 et 15. \nEntrez une nouvelle case:");
                scanf("%d",&desert_joueur_[*numero_joueur_]);
        }

        for(int i=0;i<5;i++)
        {
            while(valeur_absolue(desert_joueur_[*numero_joueur_])==case_camel_[i])//empeche de poser la carte la ou un chameau est present
            {
                printf("Vous ne pouvez pas poser une carte desert sur une case ou est present un chameau. \nEntrez une nouvelle case:");
                scanf("%d",&desert_joueur_[*numero_joueur_]);
            }
        }
        //empeche de placer une carte desert sur une case ou il y a deja une carte desert/adjacente a une case ou il y a une carte desert
        for(int j=0;j<*numero_joueur_;j++)
        {
            while(valeur_absolue(desert_joueur_[*numero_joueur_])==valeur_absolue(desert_joueur_[j])||valeur_absolue(desert_joueur_[*numero_joueur_])==valeur_absolue(desert_joueur_[j])+1||valeur_absolue(desert_joueur_[*numero_joueur_])==valeur_absolue(desert_joueur_[j]-1))
            {
                printf("Vous ne pouvez pas poser une carte desert sur une case ou il y a une deja carte desert ou adjacente a une case ou il y a une carte desert.\nEntrez une nouvelle case");
                scanf("%d",&desert_joueur_[*numero_joueur_]);
            }
        }
        for(int k=*numero_joueur_+1;k<nb_joueur_;k++)
        {
            while(valeur_absolue(desert_joueur_[*numero_joueur_])==valeur_absolue(desert_joueur_[k])||valeur_absolue(desert_joueur_[*numero_joueur_])==valeur_absolue(desert_joueur_[k])+1||valeur_absolue(desert_joueur_[*numero_joueur_])==valeur_absolue(desert_joueur_[k]-1))
            {
                printf("Vous ne pouvez pas poser une carte desert sur une case ou il y a une deja carte desert ou adjacente a une case ou il y a une carte desert.\nEntrez une nouvelle case: ");
                scanf("%d",&desert_joueur_[*numero_joueur_]);
            }
        }
        break;
    }
    system("cls");
    affichage_plateau(hauteur_camel_,case_camel_,desert_joueur_,dede_,nb_joueur_,carte_pyramide_,argent_,*chameau_mise_,*mise_parti_);
    if (*numero_joueur_!=nb_joueur_-1)//permet de passer au tour du joueur suivant
    {
        *numero_joueur_=*numero_joueur_+1;
    }
    else//permet de premier joueur de jouer apres le dernier joueur
    {
        *numero_joueur_=0;
    }
}


void debut_partie(int *nb_joueur_,int hauteur_camel_[], int case_camel_[], int desert_joueur_[], int carte_pyramide_[], int argent_[], int dede_[],s_mise_manche *chameau_mise_)
{

    printf("\nVous etes %d joueurs.\n Tirage des des pour le placement initial des chameaux:\n",*nb_joueur_);


    for(int i=0;i<5;i++)//tirage des des et placement des chameaux sur le plateau
    {
       srand(time(NULL));
      int inside_pyramide=0;
      int de_tire;
      while (inside_pyramide==0)    //choisi un dé aléatoirement parmis ceux dispo dans la pyramide
      {
        de_tire= rand()%5; //nbr aleatoire entre 0 et 4
        if (dede_[de_tire]==1)
          {
            inside_pyramide=1;
            dede_[de_tire]=0;
          }

       }
       printf ("\nle de sorti de la pyramide est le :");
       switch (de_tire)
              {
                  case 0: printf (" rouge \n"); break;
                  case 1: printf (" bleu  \n"); break;
                  case 2: printf (" blanc \n"); break;
                  case 3: printf (" jaune \n"); break;
                  case 4: printf (" vert  \n"); break;
              }
       int lance_de_de=(rand()%3)+1; //nbr aleatoire entre 1 et 3
       printf ("\nle resultat du lance est : %d  \n\n",lance_de_de);

       int nb_camel_present=0;//placement des chameaux(Camel en anglais)
       for(int r=0;r<5;r++)
       {
          if (case_camel_[r]==lance_de_de)
          {
              nb_camel_present++;
          }
       }
       case_camel_[de_tire]=lance_de_de;
       hauteur_camel_[de_tire]=nb_camel_present+1;
      Sleep(2000);
    }

    initialiser(dede_,5,1);
    initialiser(desert_joueur_,*nb_joueur_,0);
    initialiser(carte_pyramide_,*nb_joueur_,0);
    system("cls");
}


void partie(int nb_joueur_a,int hauteur_camel_a[], int case_camel_a[], int desert_joueur_a[], int carte_pyramide_a[], int argent_a[], int dede_a[],int *numero_joueur_a,s_mise_manche *chameau_mise_a,s_pari_parti* mise_parti_a, int case_desert_a[])
{
 while (condition_fin_jeu(case_camel_a,nb_joueur_a)==0) //si un chameau est arrivé
{
   while ((dede_a[0]==1 ||  dede_a[1]==1 || dede_a[2]==1 || dede_a[3]==1 || dede_a[4]==1) && condition_fin_jeu(case_camel_a,nb_joueur_a)==0)//condition de fin de manche: tout les des doivent etre en dehors de la pyramide
   {
       tour_joueur(nb_joueur_a,hauteur_camel_a,case_camel_a,desert_joueur_a,carte_pyramide_a,argent_a,dede_a,numero_joueur_a,chameau_mise_a,mise_parti_a);
   }
   fin_de_manche(dede_a,argent_a,carte_pyramide_a,case_desert_a,nb_joueur_a,chameau_mise_a);
   recompense_pari_manche(argent_a,case_camel_a,hauteur_camel_a,*chameau_mise_a,nb_joueur_a);
   init_mise_manche(chameau_mise_a);
   affichage_plateau(hauteur_camel_a,case_camel_a,case_desert_a,dede_a,nb_joueur_a,carte_pyramide_a,argent_a,*chameau_mise_a,*mise_parti_a);
}

}


int condition_fin_jeu(const int tab[],int nb)
{
    // =1 si un chameau est arrivé(si un élément de tab[]>15), =0 sinon
    int indicateur=0;
    for (int x=0;x<5;x++)
    {
        if (tab[x]>15)
            indicateur=1;
    }
    if (indicateur==0)
        return 0;
    if (indicateur!=0)
        return 1;

}


void fin_de_partie(const int hauteur_[],const int case_[],const int desert_[],const int de_[],int nb_player_,const int card_pyra_[],int dinero_[],s_mise_manche cham,s_pari_parti mise_parti_)
{
system("cls");
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------------------FIN DE PARTIE-------------------------------------------------\n\n\n");
Sleep(1500);
system("cls");
recompense_pari_parti(dinero_,case_,hauteur_,nb_player_,mise_parti_);
//affiche une dernière fois le plateau
affichage_plateau(hauteur_,case_,desert_,de_,nb_player_,card_pyra_,dinero_,cham,mise_parti_);
printf(" \n le classement final arrive dans 5 secondes");
Sleep(5000);
system("cls");

//création du classement final

int* classement_final_joueur; // stock dans la case 0 le 1er, la case 1 le 2e etc..
classement_final_joueur= (int*)malloc(sizeof(int)*nb_player_);
Tri(dinero_,nb_player_,classement_final_joueur);
printf("\n\n                   Le gagnant est le joueur %d  \n\n",classement_final_joueur[0]+1);
for (int q=1;q<nb_player_;q++)
    printf("                 Le %de est le joueur %d   \n",q+1,classement_final_joueur[q]+1);

}
