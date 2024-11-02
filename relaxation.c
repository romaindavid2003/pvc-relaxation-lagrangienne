#include "tsp.h"
#include <math.h>

Arete ** liste_relax;
Arete ** liste0_relax; 
Arete ** un_arbre_relax;
int * degres_arbre_relax;

double alpha_initial; // valeur initiale de alpha ; on peut eventuellement ne pas modifier ou bien faire en sorte qu'il varie de 2 a 1

double * lambda; // tableau des multiplicateurs de Lagrange

int nb_iter; // Nombre d'iterations dans le calcul d'un minorant du maximum de la fonction duale


// on aura sans doute interet a ne pas reinitialiser les valeurs des lambda
// entre deux utilisations de la relaxation
void init_relax(double _alpha_initial, int _nb_iter, int _borne)
{
  //printf("alpha = %f\n", _alpha_initial);
  //printf("n iter in relax = %d\n", _nb_iter);
  int i;

  alpha_initial = _alpha_initial;
  nb_iter = _nb_iter;
  borne = _borne;
  lambda = (double *) malloc(n * sizeof(double));
  for (i = 0; i < n; i++) lambda[i] = 0;
  liste_relax =(Arete **) malloc((n - 1) * (n - 2) / 2 * sizeof(Arete *));
  liste0_relax =(Arete **) malloc((n - 1) * sizeof(Arete *));
  un_arbre_relax = (Arete **) malloc(n * sizeof(Arete *));
  degres_arbre_relax = (int *) malloc(n * sizeof(int));
}

void trier_relax(Arete ** liste, int nb_liste)
{
  int i, j;
  double poids_cle;
  Arete * cle;

  for (i = 0; i < nb_liste; i++) 
    liste[i] -> poids_relax =  liste[i] -> poids + 
      lambda[liste[i] -> x] + lambda[liste[i] -> y];
  for (i = 0; i < nb_liste; i++)
    {
      cle = liste[i];
      poids_cle = cle -> poids_relax;
      j = i;
      while((j > 0) && (poids_cle < liste[j - 1] -> poids_relax)) 
	{
	  liste[j] = liste[j - 1];
	  j--;
	}
      liste[j] = cle;
    }
}

int entier_superieur(double valeur){
  int entier = (int)valeur;
  if (valeur-entier > 0){return entier;}
  return entier+1;
}

/*
  Apres le deroulement de cette fonction, les aretes du un _arbre 
  minimisant la fonction de Lagrange
  pour les valeurs de lambda donnees par la variable globale lambda
  sont contenues dans un_arbre_relax (variable globale) ;
  les degres des sommets de cet arbre sont contenus dans degres_arbre_relax (variable globale)..
*/
void meilleur_un_arbre_relax(int nb_liste, int nb_liste0)
{
  int i, j, r = 0;
  int a, b;
  int * comp;

  trier_relax(liste_relax, nb_liste);
  trier_relax(liste0_relax, nb_liste0);
  comp = (int *) malloc(n * sizeof(int));
  for (i = 0; i < n; i++) 
    {
      comp[i] = i;
      degres_arbre_relax[i] = 0;
    }
  i = 0;
  for (r = 0; r < nb_liste; r++) 
    {
      if (liste_relax[r] -> statut == 1)
      {
        un_arbre_relax[i] = liste_relax[r];
        degres_arbre_relax[un_arbre_relax[i] -> x]++;
        degres_arbre_relax[un_arbre_relax[i] -> y]++;
        a = comp[un_arbre_relax[i] -> x];
        b = comp[un_arbre_relax[i] -> y];
        for (j = 1; j < n; j++)
          if (comp[j] == b) comp[j] = a;
        i++;
      }
    }
  r = 0;
  while ((r < nb_liste) && (i < n - 2))
    {
      while((r < nb_liste) && 
	     ((comp[liste_relax[r] -> x] == comp[liste_relax[r] -> y]) || 
	    (liste_relax[r] -> statut != 2))) r++; 
      if (r < nb_liste) 
	{
	  un_arbre_relax[i] = liste_relax[r];
	  degres_arbre_relax[un_arbre_relax[i] -> x]++;
	  degres_arbre_relax[un_arbre_relax[i] -> y]++;
	  a = comp[un_arbre_relax[i] -> x];
	  b = comp[un_arbre_relax[i] -> y];
	  for (j = 1; j < n; j++)
	    if (comp[j] == b) comp[j] = a;
	  r++;
	  i++;
	}
    }
  for (r = 0; r < nb_liste0; r++)  
      if (liste0_relax[r] -> statut == 1) 
	{
	  un_arbre_relax[i] = liste0_relax[r];
	  i++;
	}
  r = 0;
  while ((i < n) && (r < nb_liste0))
    {
      while((r < nb_liste0) && (liste0_relax[r] -> statut != 2)) r++; 
      if (r < nb_liste0)
	{
	  un_arbre_relax[i] = liste0_relax[r];
	  degres_arbre_relax[un_arbre_relax[i] -> x]++;
	  degres_arbre_relax[un_arbre_relax[i] -> y]++;
	  r++; 
	  i++;
	}
    }
  free(comp);
}

double valeur_fonction_duale()
{
  double valeur = 0;
  int i;

  for (i = 0; i < n; i++)  
      valeur += un_arbre_relax[i] -> poids_relax;
  for (i = 0; i < n; i++)   valeur -= 2 * lambda[i];
  return valeur;
}

int is_hamil(){
  for( int i = 0 ; i < n ; i++ )
  {
      if (degres_arbre_relax[i] != 2) return 0;
  }
  return 1;
}



void update_lambda(float valeur){
  float norme = 0;
  for( int i = 0 ; i < n ; i++ )
  {
    norme += (degres_arbre_relax[i]-2)*(degres_arbre_relax[i]-2);
  }
  float step_size = alpha_initial*((float)borne-valeur)/norme;
  for( int i = 0 ; i < n ; i++ )
  {
    lambda[i] = lambda[i] + step_size*((float)degres_arbre_relax[i]-2);
  }

}

/* 
   L'ordre du graphe est note n.
   La fonction doit retourner 1 si on peut couper le noeud 
   correspondant de l'arbre de recherche, 
   c'est-a-dire si celui-ci ne contient pas mieux que la meilleure solution deja rencontree. 
   La fonction retourne 0 si la relaxation n'a pas permis de couper ce noeud. 
   Une variable globale nommee borne est declaree dans tsp.h
   et definie dans tsp.c. Elle contient la plus petite longueur 
   d'un tour rencontre depuis le debut de la methode BB. 
   Une fonction poids definie dans tsp.c renvoie le poids d'un un_arbre passe en parametre.
   Une fonction sauvegarde_solution permet de sauvegarder un un_arbre
   passe en parametre dans la variable globale meilleure_solution.
   liste contient les aretes non exclues et non incidentes au sommet 0, 
   le nombre de ces aretes vaut nb_liste.
   liste0 contient les aretes non exclues et incidentes au sommet 0, 
   le nombre de ces aretes vaut nb_liste0.
   Les variables liste et liste0 n'auront probablement pas besoin 
   d'etre manipulees en completant le programme.
   nb_iter donne le nombre maximum d'iterations dans la recherche du maximum de la fonction duale
   Les multiplicateurs de Lagrange sont dans un tableau global nomme lambda.

*/
int relaxation(Arete ** liste, Arete ** liste0, int nb_liste, int nb_liste0)
{
  //stc::cerr<<1<<std::endl;
  int sommet;
  int num_iter = 1, i, k;
  double valeur;
  double alpha = alpha_initial;
  double T;
  int P, D;

  for (i = 0; i < nb_liste; i++) liste_relax[i] = liste[i];
  for (i = 0; i < nb_liste0; i++) liste0_relax[i] = liste0[i];

   // debut de la  recherche du maximum de la fonstion duale
  for (num_iter = 1; num_iter <= nb_iter; num_iter++)
    {
      /* calcul de l'arbre qui minimise la fonction de Lagrange 
	 pour les valeurs actuelles des lambdas ; 
	 la liste des aretes de cet arbre 
	 est dans un_arbre_relax (variable globale). */
      meilleur_un_arbre_relax(nb_liste, nb_liste0); 

      // valeur de la fonction duale pour les valeurs actuelles des lambdas 
      valeur = valeur_fonction_duale();

      /* Regarder si on peut couper en comparant la valeur de la 
	 fonction duale a la borne ; si oui, sortir ; 
	 on raffinera la comparaison en utilisant le fait que 
	 le probleme primal admet une solution entiere, alors que 
	 les valeurs de la fonction duale sont reelles (si la borne vaut 40 
	 et que valeur vaut 39.001, on ne peut pas faire moins que 40
	 et on peut couper). */

      // A COMPLETER
      if (ceil(valeur)>=borne){return 1;}

      /* Regarder si un_arbre_relax est un tour en utilisant 
	 le tableau global degres_arbre_relax
	 qui contient les degres des sommets de un_arbre_relax. */

      if (is_hamil()){
        int tree_weight = poids(un_arbre_relax);
        if (tree_weight < borne){
          borne = tree_weight;
          sauvegarde_solution(un_arbre_relax);
          //printf("borne = %d\n", borne);
          
        }
        return 1;
      }

      /* S'il s'agit d'un tour :
	 calculer le poids de ce tour ; 
	 l'appel de poids(un_arbre_relax) renvoie ce poids.
	 S'il le faut, mettre ce poids dans borne et sauvegarder 
	 le tour correspondant par l'instruction 
	 sauvegarde_solution(un_arbre_relax).
	 Quitter la fonction relaxation en renvoyant la valeur appropriee.
	 utiliser eventuellement :
	      printf("borne = %d\n", borne);
	 pour voir ou en est la borne.
      */
    
      // A COMPLETER 


      if (num_iter == nb_iter) break;
      /* Si les remarques precedentes n'ont pas permis de conclure,
	 calculer de nouvelles valeurs des lambdas (voir le cours).
	  */
 
      // A COMPLETER

      update_lambda(valeur);

      /* Ne pas trop se preoccuper de cette boucle qui sert a eviter  une divergence 
	 de la recherche de l'optimum, 
      . */
      for (i = 0; i < n; i++) 
      {
        if ((lambda[i] > borne) || (lambda[i] < -borne)) lambda[i] = 0;
      }

      /* Faire decroitre la valeur de alpha (on peut eventuellement laisser
	 toujours ce parametre a la valeur alpha_initial) */
	// A COMPLETER EVENTUELLEMENT
      alpha_initial *= 0.9;


    }
  return 0;
}
