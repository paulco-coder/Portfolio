#include <stdio.h>
#include <stdlib.h>

#include "types_structures.h"
#include "programmes_outils.h"
#include "affichage.h"


void affichage_plateau(const int hauteur_chameau[],const int case_chameau[],const int desert[],const int de[],int nb_player,const int card_pyramide[],const int dinero[],const s_mise_manche cham,const s_pari_parti mise_parti_)
      {

//------------------------------------------------------------LIGNE DU HAUT DU PLATEAU (CASES 0 1 2 14 15)----------------------------------------------------------------------------

          printf("----14-------15----------------1--------2-----  joueur 1:                joueur2:                 ");
          if (nb_player>=3)
            printf("joueur 3:                ");
          if (nb_player>=4)
            printf("joueur 4:");
          printf("\n|");

          int limite=4;   // on en a besoin quand on affiche les infos de chaque joueur car on affiche les 4 éventuels premiers joueurs sur la même ligne
          if (nb_player<4)
            limite=nb_player;

          int l; //l pour ligne
          for (l=5;l>=1;l--)
          {
          int c=14; //c pour case
          while (c!=-1) //je défini c à -1 au switch à la fin quand je veux casser la boucle et donc changer de ligne
          {
              //si il y a un chameau à la ligne et à la case donnée par les boucles alors il l'affiche
              int espace=1;
              if (hauteur_chameau[0]==l && case_chameau[0]== c)
              {
                  printf("{rouge} |");
                  espace=0;
              }
              if (hauteur_chameau[1]==l && case_chameau[1]== c)
              {
                  printf("{bleu}  |");
                  espace=0;
              }
              if (hauteur_chameau[2]==l && case_chameau[2]== c)
                {
                  printf("{blanc} |");
                  espace=0;
              }
              if (hauteur_chameau[3]==l && case_chameau[3]== c)
                {
                  printf("{jaune} |");
                  espace=0;
              }
              if (hauteur_chameau[4]==l && case_chameau[4]== c)
                {
                  printf("{vert}  |");
                  espace=0;
              }

              if (l==3)
              {
              for (int m=0;m<nb_player; m++) //recherche dans toutes les cartes deserts
              {
                  if (desert[m]== c)
                  {
                  printf("oasis +1|");
                  espace=0;
                  }
                  if (desert[m]== -c)
                  {
                  printf("mirage-1|");
                  espace=0;
                  }
              }
              }

              if (c==100 && l==4) // je note la case de depart 100 pour pas avoir de problèmes
              {
                  printf(" depart |");
                  espace=0;
              }
              if (c==100 && l==3)
              {
                  printf("    /   |");
                  espace=0;
              }
              if (c==100 && l==2)
              {
                  printf(" arrive |");
                  espace=0;
              }
              if (espace==1)
                printf("        |");

              if (c==2 && l==4)
              {
              for (int x=0;x<limite;x++) // affiche la case desert correspondant aux 4 premiers joueurs
              {
                if (x==0)
                    printf("  ");
                if (nb_player>x)
                {
                if (desert[x]==0)
                 printf("case desert: %c          ",155); // caractère endemble vide
                if (desert[x]!=0)
                 printf("case desert: %d          ",valeur_absolue(desert[x])); //valeur absolue pour éviter les "-" sur les carte mirage
                if (valeur_absolue(desert[x])<10)
                    printf(" ");
                }
              }
              }
              if (c==2 && l==3)
              {
                for (int y=0;y<limite;y++)// affiche le nombre de carte pyramides correspondant aux 4 premiers joueurs
              {
                if (y==0)
                    printf("  ");
                if (nb_player>y)
                 printf("carte pyramide: %d        ",card_pyramide[y]);
              }
              }
              if (c==2 && l==2)
              {
                for (int z=0;z<limite;z++) // affiche l'argent des 4 premiers joueurs
              {
                 printf("  argent %c: %d           ",156,dinero[z]); //156 -> £
                if (dinero[z]<10)
                    printf(" ");
              }
              }
              if (c==2 && l==1)
              {
                printf("  ");
                for (int g=0;g<limite;g++)
              {
                 printf("pari partie 1er:         ");
              }
              }
              switch (c)
              {
                  case 14: c=15; break;
                  case 15: c=100; break;
                  case 100: c=1; break;
                  case 1: c=2; break;
                  case 2: c=-1; break;
              }

          }
          if (l!=1)
          printf("\n|");//changement de ligne
          }
          printf("\n----13----------------------------------3-----");
          int rang;
          for (int j=0;j<limite;j++) //affiche si les 4 premiers joueurs ont parié (partie) et en quel position
            {
                rang=0;
                 for (int e=0;e<nb_player;e++)
                 {
                  if (mise_parti_.ordre_premier[e]==j) //recherche dans tout le tableau ordre_premier
                        rang=e+1;
                 }
              if (rang==0)
              printf("  n'a pas parier         ");
              if (rang==1)
              printf("  a parier en 1er        ");
              if (rang>1)
                printf("  a parier en %de         ",rang);
              }
         printf("\n|");

//------------------------------------------------------------3 LIGNES DU MILIEU DU PLATEAU (CASES 3 4 5 11 12 13)---------------------------------------------------------------

          int t;
          for (t=1;t<=3;t++) //il y a 3 lignes au milieu de notre plateau
          {
          for (l=5;l>=1;l--)
          {
              int c;
              switch (t)
              {
                  case 1: c=13; break;
                  case 2: c=12; break;
                  case 3: c=11; break;
              }


              while (c!=-1) //même fonctionnement qu'au dessus
          {
              int espace=1;
              if (hauteur_chameau[0]==l && case_chameau[0]== c)
              {
                  printf("{rouge} |");
                  espace=0;
              }
              if (hauteur_chameau[1]==l && case_chameau[1]== c)
              {
                  printf("{bleu}  |");
                  espace=0;
              }
              if (hauteur_chameau[2]==l && case_chameau[2]== c)
                {
                  printf("{blanc} |");
                  espace=0;
              }
              if (hauteur_chameau[3]==l && case_chameau[3]== c)
                {
                  printf("{jaune} |");
                  espace=0;
              }
              if (hauteur_chameau[4]==l && case_chameau[4]== c)
                {
                  printf("{vert}  |");
                  espace=0;
              }

              if (l==3)
              {
              for (int m=0;m<nb_player; m++)
              {
                  if (desert[m]== c)
                  {
                  printf("oasis +1|");
                  espace=0;
                  }
                  if (desert[m]== -c)
                  {
                  printf("mirage-1|");
                  espace=0;
                  }
              }
              }
              if (espace==1)
                printf("        |");
              if (c==13 || c==12 || c==11)
              {
              int space=0;

               if (c==13 && l==3 && t==1 && (de[0]==0||de[1]==0||de[2]==0||de[3]==0||de[4]==0)) //si au moins un de est sorti
               {
                  space=1;
                  printf("les des deja sortis sont :|");
               }
               if (c==13 && l==2 && t==1 && de[0]==0) //si le de rouge est sorti
               {
                  space=1;
                  printf("          rouge           |");
               }
               if (c==13 && l==1 && t==1 && de[1]==0)
               {
                  space=1;
                  printf("          bleu            |");
               }
               if (c==12 && l==5 && t==2 && de[3]==0)
               {
                  space=1;
                  printf("          jaune           |");
               }
               if (c==12 && l==4 && t==2 && de[4]==0)
               {
                  space=1;
                  printf("          vert            |");
               }
              if (space==0)
                printf("                          |");
              }

              if(c==3 && l==5)
              {
                  printf("  ");
                  for (int q=0;q<limite;q++)
                  printf("pari partie dernier:     "); //même fonctionnement que pour parti premier
              }
              if(c==3 && l==4)
              {
                  for (int j=0;j<limite;j++)
            {
                rang=0;
                 for (int e=0;e<nb_player;e++) //recherche dans tout le tableau ordre_dernier
                 {
                  if (mise_parti_.ordre_dernier[e]==j)
                        rang=e+1;
                 }
              if (rang==0)
              printf("  n'a pas parier         ");
              if (rang==1)
              printf("  a parier en 1er        ");
              if (rang>1)
                printf("  a parier en %de         ",rang);
              }
              }

              if (c==3 && l==1)
          {
            if (nb_player>=5)
            printf("  joueur 5:                ");
            if (nb_player>=6)
            printf("joueur 6:                ");
            if (nb_player>=7)
            printf("joueur 7:                ");
            if (nb_player>=8)
            printf("joueur 8:");
          }




              if (c==4 && l==5)
              {
              for (int x=4;x< nb_player;x++)
              {
                if (x==4)
                    printf("  ");
                if (nb_player>x)
                {
                if (desert[x]==0)
                 printf("case desert: %c          ",155);
                if (desert[x]!=0)
                 printf("case desert: %d          ",valeur_absolue(desert[x]));
                if (valeur_absolue(desert[x])<10)
                    printf(" ");
                }
              }
              }
              if (c==4 && l==4)
              {
                for (int y=4;y<nb_player;y++)
              {
                if (y==4)
                    printf("  ");
                 printf("carte pyramide: %d        ",card_pyramide[y]);
              }
              }
              if (c==4 && l==3)
              {
                for (int z=4;z<nb_player;z++)
              {
                 printf("  argent %c: %d           ",156,dinero[z]);
                if (dinero[z]<10)
                    printf(" ");
              }
              }
              if (c==4 && l==2)
              {
                  printf("  ");
                for (int g=4;g<nb_player;g++)
              {
                 printf("pari partie 1er:         ");
              }
              }
              if (c==4 && l==1)
              {
              for (int j=4;j<nb_player;j++)
            {
                rang=0;
                 for (int e=0;e<nb_player;e++)
                 {
                  if (mise_parti_.ordre_premier[e]==j)
                        rang=e+1;
                 }
              if (rang==0)
              printf("  n'a pas parier         ");
              if (rang==1)
              printf("  a parier en 1er        ");
              if (rang>1)
                printf("  a parier en %de         ",rang);
              }
              }

              if (c==5 && l==5)
              {
                  for (int j=4;j<nb_player;j++)
            {
                rang=0;
                 for (int e=0;e<nb_player;e++)
                 {
                  if (mise_parti_.ordre_dernier[e]==j)
                        rang=e+1;
                 }
              if (rang==0)
              printf("  n'a pas parier         ");
              if (rang==1)
              printf("  a parier en 1er        ");
              if (rang>1)
                printf("  a parier en %de         ",rang);
              }
              }

              if (c==5 && l==2)
                printf ("Les paris manche des joueurs avec le gain associe si le chameau choisi arrive a la position donnee:");
              if (c==5 && l==1)
                printf ("        rouge               bleu               blanc              jaune              vert");


              switch (c)
              {
                  case 13: c=3; break;
                  case 3: c=-1; break;
                  case 12: c=4; break;
                  case 4: c=-1; break;
                  case 11: c=5; break;
                  case 5: c=-1; break;
              }

          }
          if (l!=1)
          printf("\n|");//changement de ligne
          }
          if (t==1 && de[2]==0)
            printf ("\n----12----          blanc           ----4-----");
          if (t==1 && de[2]==1)
            printf ("\n----12----                          ----4-----");
          if (t==1)
          {
          printf("\n|");
          }
          if (t==2)
          {
            printf ("\n----11----                          ----5-----");

            printf("  ");
                for (int g=4;g<nb_player;g++)
              {
                 printf("pari partie dernier:     ");
              }
            printf("\n|");
          }

          }
          printf ("\n----10-------9--------8--------7--------6-----     -------------      -------------      -------------      -------------      -------------\n|");

//---------------------------------------------------------------DERNIERE LIGNE DU PLATEAU (CASES 6 a 10)------------------------------------------------------------------

          for (l=5;l>=1;l--)
          {
          int c; //c pour case
          for (c=10;c>=6;c--)
          {
              int espace=1;
              if (hauteur_chameau[0]==l && case_chameau[0]== c)
              {
                  printf("{rouge} |");
                  espace=0;
              }
              if (hauteur_chameau[1]==l && case_chameau[1]== c)
              {
                  printf("{bleu}  |");
                  espace=0;
              }
              if (hauteur_chameau[2]==l && case_chameau[2]== c)
                {
                  printf("{blanc} |");
                  espace=0;
              }
              if (hauteur_chameau[3]==l && case_chameau[3]== c)
                {
                  printf("{jaune} |");
                  espace=0;
              }
              if (hauteur_chameau[4]==l && case_chameau[4]== c)
                {
                  printf("{vert}  |");
                  espace=0;
              }

              if (l==3)
              {
              for (int m=0;m<nb_player; m++)
              {
                  if (desert[m]== c)
                  {
                  printf("oasis +1|");
                  espace=0;
                  }
                  if (desert[m]== -c)
                  {
                  printf("mirage-1|");
                  espace=0;
                  }
              }
              }

              if (espace==1)
                printf("        |");

              if (c==6 && l==5)
              {
                  printf("   ");
                printf("  | %c | %c | %c |    ",conv(cham.mise_rouge[0]),conv(cham.mise_rouge[1]),conv(cham.mise_rouge[2])); //voir le fonctionnement de conv() pour comprendre
                printf("  | %c | %c | %c |    ",conv(cham.mise_bleu[0]),conv(cham.mise_bleu[1]),conv(cham.mise_bleu[2]));
                printf("  | %c | %c | %c |    ",conv(cham.mise_blanc[0]),conv(cham.mise_blanc[1]),conv(cham.mise_blanc[2]));
                printf("  | %c | %c | %c |    ",conv(cham.mise_jaune[0]),conv(cham.mise_jaune[1]),conv(cham.mise_jaune[2]));
                printf("  | %c | %c | %c |",conv(cham.mise_vert[0]),conv(cham.mise_vert[1]),conv(cham.mise_vert[2]));
              }
              if (c==6 && l==4)
              {
                  printf("1er:  ");
                  for (int w=0;w<5;w++)
                    printf("+5%c +3%c +2%c        ",156,156,156); // 156 -> £
              }
              if (c==6 && l==3)
              {
                  printf("2e:   ");
                  for (int w=0;w<5;w++)
                    printf("+1%c +1%c +1%c        ",156,156,156);
              }
              if (c==6 && l==2)
              {
                  printf("autre:");
                  for (int w=0;w<5;w++)
                    printf("-1%c -1%c -1%c        ",156,156,156);
              }


          }
          if (l!=1)
          printf("\n|");//changement de ligne
          }
          printf("\n----------------------------------------------\n");


}

