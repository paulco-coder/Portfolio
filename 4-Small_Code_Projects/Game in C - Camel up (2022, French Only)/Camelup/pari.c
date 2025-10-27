#include <stdio.h>
#include <stdlib.h>

#include "types_structures.h"
#include "programmes_outils.h"
#include "pari.h"


void classement(int classement[],const int case_camel__[],const int hauteur_camel__[])//donne le premier et le deuxieme chameau à la fin de la manche
{
    s_info_camel premier;
    s_info_camel deuxieme;
    int x=0;
    int y=1;
    affectation_info(&premier,case_camel__,hauteur_camel__,x);
    affectation_info(&deuxieme,case_camel__,hauteur_camel__,y);
    for (int i=1;i<5;i++)
    {
        if(premier.case_<case_camel__[i])
        {
           affectation_info(&deuxieme,case_camel__,hauteur_camel__,premier.couleur);
           affectation_info(&premier,case_camel__,hauteur_camel__,i);
        }
        else
        {
            if(premier.case_==case_camel__[i])
            {
                if(hauteur_camel__[i]>premier.hauteur)
                {
                   affectation_info(&deuxieme,case_camel__,hauteur_camel__,premier.couleur);
                   affectation_info(&premier,case_camel__,hauteur_camel__,i);
                }
            }
            else
            {
                if(deuxieme.case_<case_camel__[i])
                {
                    affectation_info(&deuxieme,case_camel__,hauteur_camel__,i);
                }
                else
                {
                    if(deuxieme.case_==case_camel__[i])
                    {
                        if(deuxieme.hauteur<hauteur_camel__[i])
                        {
                            affectation_info(&deuxieme,case_camel__,hauteur_camel__,i);
                        }
                    }
                }
            }
        }
    }
    classement[0]=premier.couleur;
    classement[1]=deuxieme.couleur;
}


void recompense_pari_manche(int money_[],const int case_chamcham__[],const int hauteur_chamcham__[],const s_mise_manche chamcham_mise,const int nb_joujou)//distribue argent en fonction des paries de manches
{
    int ordre[2];
    classement(ordre,case_chamcham__,hauteur_chamcham__);//permet de connaitre le premier et le deuxieme chameau
    switch(ordre[0])//cas en fonction du premier chameau
    {
   case 0://si 1er chameau est rouge
       if(chamcham_mise.mise_rouge[0]!=-1)//le premier joueur a avoir bien parié gagne 5
           {
               money_[chamcham_mise.mise_rouge[0]]=money_[chamcham_mise.mise_rouge[0]]+5;
           }
       if(chamcham_mise.mise_rouge[1]!=-1)//le deuxieme joueur a avoir bien parié gagne 3
           {
               money_[chamcham_mise.mise_rouge[1]]=money_[chamcham_mise.mise_rouge[1]]+3;
           }
       if(chamcham_mise.mise_rouge[2]!=-1)//le troisieme joueur a avoir bien parié gagne 2
           {
               money_[chamcham_mise.mise_rouge[2]]=money_[chamcham_mise.mise_rouge[2]]+2;
           }
        switch(ordre[1])//case en fonction du deuxieme chameau
        {
        case 1://si deuxieme chameau est bleu
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_bleu[i]!=-1)//si le chameau parié est premier, le joueur gagne 1
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)//si le chameau parié n'est ni premier ni deuxieme, le joueur perd 1
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)//si le chameau parié n'est ni premier ni deuxieme, le joueur perd 1
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)//si le chameau parié n'est ni premier ni deuxieme, le joueur perd 1
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 2:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 3:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 4:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+1;
                }
            }
            break;
        }
       break;
   case 1://si 1er chameau est bleu
       if(chamcham_mise.mise_bleu[0]!=-1)//le premier joueur a avoir bien parié gagne 5
           {
               money_[chamcham_mise.mise_bleu[0]]=money_[chamcham_mise.mise_bleu[0]]+5;
           }
       if(chamcham_mise.mise_bleu[1]!=-1)//le deuxieme joueur a avoir bien parié gagne 3
           {
               money_[chamcham_mise.mise_bleu[1]]=money_[chamcham_mise.mise_bleu[1]]+3;
           }
       if(chamcham_mise.mise_bleu[2]!=-1)//le troisieme joueur a avoir bien parié gagne 2
           {
               money_[chamcham_mise.mise_bleu[2]]=money_[chamcham_mise.mise_bleu[2]]+2;
           }
        switch(ordre[1])//case en fonction du deuxieme chameau
        {
        case 0:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 2:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 3:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 4:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+1;
                }
            }
            break;
        }
    break;
   case 2://si 1er chameau est blanc
       if(chamcham_mise.mise_blanc[0]!=-1)//le premier joueur a avoir bien parié gagne 5
           {
               money_[chamcham_mise.mise_blanc[0]]=money_[chamcham_mise.mise_blanc[0]]+5;
           }
       if(chamcham_mise.mise_blanc[1]!=-1)//le deuxieme joueur a avoir bien parié gagne 3
           {
               money_[chamcham_mise.mise_blanc[1]]=money_[chamcham_mise.mise_blanc[1]]+3;
           }
       if(chamcham_mise.mise_blanc[2]!=-1)//le troisieme joueur a avoir bien parié gagne 2
           {
               money_[chamcham_mise.mise_blanc[2]]=money_[chamcham_mise.mise_blanc[2]]+2;
           }
        switch(ordre[1])//case en fonction du deuxieme chameau
        {
        case 0:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 1:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 3:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 4:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+1;
                }
            }
        }
    break;
   case 3://si 1er chameau est jaune
       if(chamcham_mise.mise_jaune[0]!=-1)//le premier joueur a avoir bien parié gagne 5
           {
               money_[chamcham_mise.mise_jaune[0]]=money_[chamcham_mise.mise_jaune[0]]+5;
           }
       if(chamcham_mise.mise_jaune[1]!=-1)//le deuxieme joueur a avoir bien parié gagne 3
           {
               money_[chamcham_mise.mise_jaune[1]]=money_[chamcham_mise.mise_jaune[1]]+3;
           }
       if(chamcham_mise.mise_jaune[2]!=-1)//le troisieme joueur a avoir bien parié gagne 2
           {
               money_[chamcham_mise.mise_jaune[2]]=money_[chamcham_mise.mise_jaune[2]]+2;
           }
        switch(ordre[1])//case en fonction du deuxieme chameau
        {
        case 0:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 1:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 2:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+-1;
                }
            }
            break;
        case 4:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_vert[i]!=-1)
                {
                    money_[chamcham_mise.mise_vert[i]]=money_[chamcham_mise.mise_vert[i]]+1;
                }
            }
        }
    break;
   case 4://si 1er chameau est vert
       if(chamcham_mise.mise_vert[0]!=-1)//le premier joueur a avoir bien parié gagne 5
           {
               money_[chamcham_mise.mise_vert[0]]=money_[chamcham_mise.mise_vert[0]]+5;
           }
       if(chamcham_mise.mise_vert[1]!=-1)//le deuxieme joueur a avoir bien parié gagne 3
       {
               money_[chamcham_mise.mise_vert[1]]=money_[chamcham_mise.mise_vert[1]]+3;
           }
       if(chamcham_mise.mise_vert[2]!=-1)//le troisieme joueur a avoir bien parié gagne 2
           {
               money_[chamcham_mise.mise_vert[2]]=money_[chamcham_mise.mise_vert[2]]+2;
           }
        switch(ordre[1])//case en fonction du deuxieme chameau
        {
        case 0:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
            }
            break;
        case 1:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
            }
            break;
        case 2:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+-1;
                }
            }
            break;
        case 3:
            for(int i=0;i<3;i++)
            {
                if(chamcham_mise.mise_rouge[i]!=-1)
                {
                    money_[chamcham_mise.mise_rouge[i]]=money_[chamcham_mise.mise_rouge[i]]+-1;
                }
                if(chamcham_mise.mise_bleu[i]!=-1)
                {
                    money_[chamcham_mise.mise_bleu[i]]=money_[chamcham_mise.mise_bleu[i]]+-1;
                }
                if(chamcham_mise.mise_blanc[i]!=-1)
                {
                    money_[chamcham_mise.mise_blanc[i]]=money_[chamcham_mise.mise_blanc[i]]+-1;
                }
                if(chamcham_mise.mise_jaune[i]!=-1)
                {
                    money_[chamcham_mise.mise_jaune[i]]=money_[chamcham_mise.mise_jaune[i]]+1;
                }
            }
        }
    break;
    }
    for(int j=0;j<nb_joujou;j++)//empeche joueur d'avoir de l'argent negatif
    {
        if(money_[j]<0)
        {
            money_[j]=0;
        }
    }
}


void pari_manche(const int numero_joueur__,s_mise_manche *camel_miser)//demande et stock les paries de joueurs sur la fin de manche
{
    int choix;
    int v=0;
    while (v==0)//force le joueur a entrer une valeur
    {
    printf("Sur quel chameau voulez vous miser?\nRouge-->Entrez 0\nBleu-->Entrez 1\nBlanc-->Entrez 2\nJaune-->Entrez 3\nVert-->Entrez 4\nVous avez choisi:");
    scanf("%d",&choix);
    while(choix<0||choix>4)//force le joueur à entrer un nombre entre 0 et 4
    {
        printf("veuilez entrez un nombre entre 0 et 4\n");
        scanf("%d",&choix);
    }

    switch(choix)//en fonction du chameau sur le quel le joueur veut miser
    {
        case 0:
             if((*camel_miser).mise_rouge[0]==-1)//si personne n'a encore misé sur le chameau rouge
                {
                    (*camel_miser).mise_rouge[0]=numero_joueur__;//stock le numero du joueur qui parie dans la première case de mise_rouge
                    v=1;//permet de sortir de la boucle
                }
             else
             {
                 if((*camel_miser).mise_rouge[1]==-1)//si une personne à déjà misé sur le chameau rouge
                {
                    (*camel_miser).mise_rouge[1]=numero_joueur__;//stock le numero du joueur qui parie dans la deuxieme case de mise_rouge
                    v=1;
                }
                else
                {
                    if((*camel_miser).mise_rouge[2]==-1)//si deux personnes ont déjà misé sur le chameau rouge
                    {
                      (*camel_miser).mise_rouge[2]=numero_joueur__;//stock le numero du joueur qui parie dans la deuxieme case de mise_rouge
                      v=1;
                    }
                    else
                    {
                        printf("3 joueurs on deja mise sur ce chameau, vous ne pouvez plus mise dessus. Veuillez choisir un autre chameau\n");
                    }
                }
             }
            break;

        case 1:
            if((*camel_miser).mise_bleu[0]==-1)
                {
                    (*camel_miser).mise_bleu[0]=numero_joueur__;
                    v=1;
                }
             else
             {
                 if((*camel_miser).mise_bleu[1]==-1)
                {
                    (*camel_miser).mise_bleu[1]=numero_joueur__;
                    v=1;
                }
                else
                {
                    if((*camel_miser).mise_bleu[2]==-1)
                    {
                      (*camel_miser).mise_bleu[2]=numero_joueur__;
                      v=1;
                    }
                    else
                    {
                        printf("3 joueurs on deja mise sur ce chameau, vous ne pouvez plus mise dessus. Veuillez choisir un autre chameau\n");
                    }
                }
             }
            break;

        case 2:
            if((*camel_miser).mise_blanc[0]==-1)
                {
                    (*camel_miser).mise_blanc[0]=numero_joueur__;
                    v=1;
                }
             else
             {
                 if((*camel_miser).mise_blanc[1]==-1)
                {
                    (*camel_miser).mise_blanc[1]=numero_joueur__;
                    v=1;
                }
                else
                {
                    if((*camel_miser).mise_blanc[2]==-1)
                    {
                      (*camel_miser).mise_blanc[2]=numero_joueur__;
                      v=1;
                    }
                    else
                    {
                        printf("3 joueurs on deja mise sur ce chameau, vous ne pouvez plus mise dessus. Veuillez choisir un autre chameau\n");
                    }
                }
             }
            break;
        case 3:
            if((*camel_miser).mise_jaune[0]==-1)
                {
                    (*camel_miser).mise_jaune[0]=numero_joueur__;
                    v=1;
                }
             else
             {
                 if((*camel_miser).mise_jaune[1]==-1)
                {
                    (*camel_miser).mise_jaune[1]=numero_joueur__;
                    v=1;
                }
                else
                {
                    if((*camel_miser).mise_jaune[2]==-1)
                    {
                      (*camel_miser).mise_jaune[2]=numero_joueur__;
                      v=1;
                    }
                    else
                    {
                        printf("3 joueurs on deja mise sur ce chameau, vous ne pouvez plus mise dessus. Veuillez choisir un autre chameau\n");
                    }
                }
             }
             break;
        case 4:
            if((*camel_miser).mise_vert[0]==-1)
                {
                    (*camel_miser).mise_vert[0]=numero_joueur__;
                    v=1;
                }
             else
             {
                 if((*camel_miser).mise_vert[1]==-1)
                {
                    (*camel_miser).mise_vert[1]=numero_joueur__;
                    v=1;
                }
                else
                {
                    if((*camel_miser).mise_vert[2]==-1)
                    {
                      (*camel_miser).mise_vert[2]=numero_joueur__;
                      v=1;
                    }
                    else
                    {
                        printf("3 joueurs on deja mise sur ce chameau, vous ne pouvez plus mise dessus. Veuillez choisir un autre chameau\n");
                    }
                }
             }
            break;
    }
    }
}


void pari_partie(s_pari_parti* mise_parti__,const int joueur_jouant,const int nombre_joueurs)//demande et stock les paries des joueurs sur la fin de partie
{
    int choix=-1;
    int w=0;
    int p=0;

    if((*mise_parti__).premier[joueur_jouant]!=-1)//Force les joueurs qui ont deja parie sur un chameau (premier ou dernier) a parier sur l'autre position
    {
        printf("Vous avez deja parie sur le chameau qui finira premier, vous ne pouvez plus parier que sur le chameau qui finira dernier\n");
        choix=1;
    }
    else
    {
        if((*mise_parti__).dernier[joueur_jouant]!=-1)//Force les joueurs qui ont deja parie sur un chameau (premier ou dernier) a parier sur l'autre position
        {
            printf("Vous avez deja parie sur le chameau qui finira dernier, vous ne pouvez plus parier que sur le chameau qui finira premier\n");
            choix=0;
        }
        else//laise le joueur choisir de miser sur premier ou dernier
        {
            printf("Pour miser sur le chameau qui finira premier, Entrez--->0\nPour miser sur le chameau qui finira dernier, Entrez--->1\n");
            scanf("%d",&choix);
        }
    }

    while(choix<0||choix>1)//force le joueur à entrer 0 ou 1
    {
        printf("veuilez entrez un nombre entre 0 et 1\n");
        scanf("%d",&choix);
    }
    printf("Sur quel chameau voulez vous miser?\nRouge-->Entrez 0\nBleu-->Entrez 1\nBlanc-->Entrez 2\nJaune-->Entrez 3\nVert-->Entrez 4\nVous avez choisi:");
    switch(choix)
    {
    case 0://si le joueur parie sur le premier

        scanf("%d",&(*mise_parti__).premier[joueur_jouant]);//permet de stocker dans le tableau premier le chameau parier par le joueur
        while((*mise_parti__).premier[joueur_jouant]<0||(*mise_parti__).premier[joueur_jouant]>4)//force le joueur a entrer un nombre entre 0 et 4
    {
        printf("veuilez entrez un nombre entre 0 et 4\n");
        scanf("%d",&(*mise_parti__).premier[joueur_jouant]);
    }
        while((*mise_parti__).premier[joueur_jouant]==(*mise_parti__).dernier[joueur_jouant] || ((*mise_parti__).premier[joueur_jouant]<0||(*mise_parti__).premier[joueur_jouant]>4))//empeche le joueur de parier sur un chameau sur lequel il a deja parie
        {
            printf("Vous avez deja parie que ce chameau serait dernier, choisissez un autre chameaux.\nEntrez un nombre entre 0 et 4 (autre que %d): ",(*mise_parti__).premier[joueur_jouant]);
            scanf("%d",&(*mise_parti__).premier[joueur_jouant]);
        }
        while(w<nombre_joueurs && p==0)//boucle pour que la premiere case égale à -1 du tableau ordre_premier prenne la valeur du numero du joueur entrain de jouer
        {                              //double condition dans le while permet de sortir de la boucle des que la condition est rempli
         if((*mise_parti__).ordre_premier[w]==-1)
         {
             (*mise_parti__).ordre_premier[w]=joueur_jouant;
             p=1;
         }
         w++;
        }
        break;
    case 1://si le joueur parie sur le dernier
        scanf("%d",&(*mise_parti__).dernier[joueur_jouant]);//permet de stocker dans le tableau dernier le chameau parier par le joueur
        while((*mise_parti__).dernier[joueur_jouant]<0||(*mise_parti__).dernier[joueur_jouant]>4)
    {
        printf("veuilez entrez un nombre entre 0 et 4\n");
        scanf("%d",&(*mise_parti__).dernier[joueur_jouant]);
    }
        while((*mise_parti__).premier[joueur_jouant]==(*mise_parti__).dernier[joueur_jouant] || ((*mise_parti__).dernier[joueur_jouant]<0||(*mise_parti__).dernier[joueur_jouant]>4))//empeche le joueur de parier sur un chameau sur lequel il a deja parie
        {
            printf("Vous avez deja parie que ce chameau serait premier, choisissez un autre chameaux.\nEntrez un nombre entre 0 et 4 (autre que %d): ",(*mise_parti__).dernier[joueur_jouant]);
            scanf("%d",&(*mise_parti__).dernier[joueur_jouant]);
        }
        while(w<nombre_joueurs && p==0)//boucle pour que la premiere case égale à -1 du tableau ordre_dernier prenne la valeur du numero du joueur entrain de jouer
        {
         if((*mise_parti__).ordre_dernier[w]==-1)
         {
             (*mise_parti__).ordre_dernier[w]=joueur_jouant;
             p=1;
         }
         w++;
        }
        break;
    }


}


void classement_parti(int classement_parti_[],const int case_chameau___[],const int hauteur_chameau___[])//donne le premier et le dernier chameau en fin de partie
 {
     classement_parti_[0]=0;//stock le numero du premier chameau
     classement_parti_[1]=0;//stock le numero du dernier chameau
     for(int i=1;i<5;i++)//boucle pour passez sur tout les cases des tableaux case_chameau et hauteur_chameau
     {
         if(case_chameau___[i]>case_chameau___[classement_parti_[0]])//determine le premier chameau en fonction de sa case et sa hauteur
         {
             classement_parti_[0]=i;
         }
         else
         {
             if(case_chameau___[i]==case_chameau___[classement_parti_[0]])
             {
                 if(hauteur_chameau___[i]>hauteur_chameau___[classement_parti_[0]])
                 {
                     classement_parti_[0]=i;
                 }
             }
         }



         if(case_chameau___[i]<case_chameau___[classement_parti_[1]])//determine le dernier chameau en fonction de sa case et sa hauteur
         {
             classement_parti_[1]=i;
         }
         else
         {
             if(case_chameau___[i]==case_chameau___[classement_parti_[1]])
             {
                 if(hauteur_chameau___[i]<hauteur_chameau___[classement_parti_[1]])
                 {
                     classement_parti_[1]=i;
                 }
             }
         }
      }
 }


void recompense_pari_parti(int Yen[],const int case_chameau__[],const int hauteur_chameau__[],const int nombre_de_jugador,const s_pari_parti mise_manche___)//distribue argent en fonction des paries de partie
 {
     int ordre[2];//stock le num du premier chameau dans ordre[0] et le num du dernier dans ordre[1]
     int compteur_premier=0;//stock le nb de personnes qui ont deja recus de l'argent pour avoir bien parie sur le premier chameau
     int compteur_dernier=0;//stock le nb de personnes qui ont deja recus de l'argent pour avoir bien parie sur le dernier chameau
     classement_parti(ordre,case_chameau__,hauteur_chameau__);//determination premier et dernier chameau

     for(int i=0;i<nombre_de_jugador;i++)
     {
         if(mise_manche___.ordre_premier[i]!=-1)//on va parcourir le tableau ordre_premier de la premiere a la derniere case (si on allait de 0 a nombre_de_jugador, "mise_manche___.ordre_premier[i]" serait remplacer par i
         {
         if(mise_manche___.premier[mise_manche___.ordre_premier[i]]!=-1 && mise_manche___.premier[mise_manche___.ordre_premier[i]]!=ordre[0])//si joueur c'est tromper dans pari, il perd de l'argent
         {
             Yen[mise_manche___.ordre_premier[i]]=Yen[mise_manche___.ordre_premier[i]]-1;//tout les joueurs qui ce sont trompé perdent 1
         }
         else
         {
             if(mise_manche___.premier[mise_manche___.ordre_premier[i]]==ordre[0])//si le joueur "mise_manche___.ordre_premier[i]" a bien parier, on regarde si il a parier en premier/deuxieme ect pour savoir combien il va gagner
             {
               for(int a=0;a<nombre_de_jugador;a++)//permet de parcourir le tableau ordre_premier
               {
                   if(mise_manche___.ordre_premier[a]==mise_manche___.ordre_premier[i])//donne un certain nombre d'argent au joueur en fonction de sa position de parieur
                   {
                       switch(compteur_premier)
                       {
                           case 0://le premier joueur a avoir misé juste gagne 8
                               Yen[mise_manche___.ordre_premier[i]]=Yen[mise_manche___.ordre_premier[i]]+8;
                              break;
                           case 1://le deuxieme joueur a avoir misé juste gagne 5
                            Yen[mise_manche___.ordre_premier[i]]=Yen[mise_manche___.ordre_premier[i]]+5;
                            break;
                           case 2://le troisieme joueur a avoir misé juste gagne 3
                            Yen[mise_manche___.ordre_premier[i]]=Yen[mise_manche___.ordre_premier[i]]+3;
                            break;
                           case 3://le quatrième joueur a avoir misé juste gagne 2
                            Yen[mise_manche___.ordre_premier[i]]=Yen[mise_manche___.ordre_premier[i]]+2;
                            break;
                           default://tout les autres joueurs qui ont misé juste gagne 1
                            Yen[mise_manche___.ordre_premier[i]]=Yen[mise_manche___.ordre_premier[i]]+1;
                       }
                    compteur_premier=compteur_premier+1;
                   }
               }
             }
         }
         }
         if(mise_manche___.ordre_dernier[i]!=-1)//on va parcourir le tableau ordre_dernier de la premiere a la derniere case (si on allait de 0 a nombre_de_jugador, "mise_manche___.ordre_dernier[i]" serait remplacer par i
         {
         if(mise_manche___.dernier[mise_manche___.ordre_dernier[i]]!=-1 && mise_manche___.dernier[mise_manche___.ordre_dernier[i]]!=ordre[1])//si joueur c'est tromper dans pari, il perd de l'argent
         {
             Yen[mise_manche___.ordre_dernier[i]]=Yen[mise_manche___.ordre_dernier[i]]-1;
         }
         else
         {
             if(mise_manche___.dernier[mise_manche___.ordre_dernier[i]]==ordre[1])//si le joueur "mise_manche___.ordre_premier[i]" a bien parier, on regarde si il a parier en premier/deuxieme ect pour savoir combien il va gagner
             {
                for(int b=0;b<nombre_de_jugador;b++)//permet de parcourir le tableau ordre_dernier
               {
                   if(mise_manche___.ordre_dernier[b]==mise_manche___.ordre_dernier[i])//donne un certain nombre d'argent au joueur en fonction de sa position de parieur
                   {
                       switch(compteur_dernier)
                       {
                           case 0:
                               Yen[mise_manche___.ordre_dernier[i]]=Yen[mise_manche___.ordre_dernier[i]]+8;
                              break;
                           case 1:
                            Yen[mise_manche___.ordre_dernier[i]]=Yen[mise_manche___.ordre_dernier[i]]+5;
                            break;
                           case 2:
                            Yen[mise_manche___.ordre_dernier[i]]=Yen[mise_manche___.ordre_dernier[i]]+3;
                            break;
                           case 3:
                            Yen[mise_manche___.ordre_dernier[i]]=Yen[mise_manche___.ordre_dernier[i]]+2;
                            break;
                           default:
                            Yen[mise_manche___.ordre_dernier[i]]=Yen[mise_manche___.ordre_dernier[i]]+1;
                       }
                    compteur_dernier=compteur_dernier+1;
                   }
               }
             }
         }
        }
     }
     for (int g=0; g<nombre_de_jugador;g++)//empêche le joueur d'avoir de l'argent négatif
     {
         if (Yen[g]<0)
            Yen[g]=0;
     }
 }
