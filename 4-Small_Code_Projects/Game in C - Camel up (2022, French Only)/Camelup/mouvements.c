#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mouvements.h"

void pyramide(int de[], int case_chameau[],int desert_[], int hauteur_chameau[], int carte_pyr[],int numero_joueur,int money[], int nb_joueur_)
{
    srand(time(NULL)); // nécessaire pour le tirage au hasard
    int inside_pyramide=0;
    int de_tire;
    carte_pyr[numero_joueur]++;
    while (inside_pyramide==0)    //choisi un dé aléatoirement parmis ceux dispo dans la pyramide
    {
        de_tire= rand()%5; //nbr aleatoire entre 0 et 4
        if (de[de_tire]==1)
        {
            inside_pyramide=1;
            de[de_tire]=0;
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
    avance_grp(de_tire,lance_de_de,case_chameau,desert_,hauteur_chameau,money,nb_joueur_);

}



void avance_grp(int de_sorti,int lance,int case_chamcham[],int desert[],int hauteur_chamcham[], int pessos[],int nb_jou)
{
    int grp[5]={0,0,0,0,0};   //1 si le chameau associé fait partie du groupe
    int hauteur_relative[5]={0,0,0,0,0};   //le plus bas du grp a une hauteur relative de 1 et plus le chameau est haut dans le grp plus sa hauteur relative est eleve
    int taille_grp=0;
    int nb_chameau_present=0; // donne le nombre de chameau present sur la case d'arrive
    int case_de_base=case_chamcham[de_sorti];
    int hauteur_de_base=hauteur_chamcham[de_sorti];

    // avance le grp, donne la taille du groupe, donne les chameaux presents dans le groupe, leur hauteur relative et le nbr de chameau sur la case d'arrive
    for (int r=0;r<5;r++)
    {
        if (case_chamcham[r]== case_de_base+lance)  // si il y a le chameau r sur la case d'arrive
            {nb_chameau_present++;
            }

        // si le chameau r est sur la meme case de depart que le chameau designe par le de et qu'il se trouve au dessus ou que c'est le chameau designe
        if (case_chamcham[r]==case_de_base && hauteur_chamcham[r]>=hauteur_de_base)
        {
            case_chamcham[r]=case_de_base+lance;
            hauteur_relative[r]= hauteur_chamcham[r]-hauteur_de_base+1;
            grp[r]=1;
            taille_grp=taille_grp+1;
        }
    }
       //recherche dans toutes les cases desert--------------------------------------------------------------------
           int case_desert=0;
           for (int t=0;t<nb_jou;t++)
           {

               //si la case d'arrivé est une case mirage------------------------------------
               if (desert[t]== -case_chamcham[de_sorti])
               {
                   pessos[t]++;
                   case_desert=1;
                   for (int s=0;s<5;s++)
                        {
                            if (case_chamcham[s]== case_chamcham[de_sorti]-1)    //si le chameau s est sur la case precedent la case mirage
                            {
                                nb_chameau_present++;
                                hauteur_chamcham[s]=hauteur_chamcham[s]+taille_grp;
                            }
                        }


                   for (int r=0;r<5;r++)
                        {
                          if (grp[r]==1)      // si le chameau r fait parti du grp
                          {
                          case_chamcham[r]=case_chamcham[r]-1;
                          hauteur_chamcham[r]=hauteur_relative[r];
                          }
                        }

               }
               //si la case d'arrivé est une case oasis-----------------------------------------
               if (desert[t]== case_chamcham[de_sorti])
               {
                   case_desert=1;
                   pessos[t]++;

                   for (int s=0;s<5;s++)
                        {
                            if (case_chamcham[s]== case_chamcham[de_sorti]+1)  //si le chameau s est sur la case apres la case oasis
                                nb_chameau_present++;
                        }

                   for (int r=0;r<5;r++)
                        {
                          if (grp[r]==1)      // si le chameau r fait parti du grp
                          {
                          case_chamcham[r]=case_chamcham[r]+1;
                          hauteur_chamcham[r]= hauteur_relative[r]+nb_chameau_present;
                          }
                        }

               }
               // si la case d'arrive n'est ni une case oasis ni mirage------------------------------------
             if (case_desert==0)
               {
                    for (int r=0;r<5;r++)
                        {
                          if (grp[r]==1)      // si le chameau r fait parti du grp
                          hauteur_chamcham[r]= hauteur_relative[r]+nb_chameau_present;
                        }
               }
    }
}

