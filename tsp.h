#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>
#include <sys/resource.h>


typedef struct arete 
{
  int x, y; // on aura en principe x > y
  int poids;

  double poids_relax;
  int statut; 
  /* statut vaut 0 si l'arete est exclue, 
     1 si elle est imposee, 
     2 sinon c'est-a-dire si elle est libre
  */
} Arete;

// n : ordre du graphe considere
extern int n;

/* 
   Les sommets d'un graphe s'appelle 0, 1, 2...
   On code le graphe traite par une matrice n * n de structures Arete

*/
extern Arete ** graphe;


// Plus petite valeur de la longueur d'un tour rencontree depuis le debut d'une methode
extern int borne;

/*
  _alpha_initial : sert a initialiser la variable globale alpha_initial definie dans relaxation.c)
  _nb_iter : sert a initialiser la variable globale nb_iter definie dans relaxation.c
  _borne : sert a initialiser la variable globale borne definie dans relaxation.c
extern void init_relax(double _alpha_initial, int _nb_iter, int _borne);
*/

// libere l'espace-memoire alloue dynamiquement
extern void liberer(Arete **, Arete **, Arete **, int * );

// recopie un_arbre dans la variable globale meilleure_solution definie dans tsp.c
extern void sauvegarde_solution(Arete ** un_arbre);

// retourne la somme des poids des aretes qui se trouvent dans le un_arbre recu en parametre
extern int poids(Arete **);

/* 
liste contient toutes les aretes du graphe (precisement, des pointeurs vers des structures representant ces aretes) sauf celles dont une extremite est le sommet 0 ; les poids des aretes tiennent compte de la modification apportee aux poids initiaux par la relaxation lagrangienne ; nb_liste est le nombre d'aretes de cette liste (ce parametre vaudra toujours (n - 1) * (n - 2) / 2).
liste0 contient toutes les aretes issues du sommet 0 ; les poids des aretes tiennent compte de la modification apportee aux poids initiaux par la relaxation lagrangienne ; nb_liste0 est le nombre d'aretes de cette liste (ce parametre vaudra toujours (n - 1)).
*/
extern int relaxation(Arete ** liste, Arete ** liste0, int nb_liste, int nb_liste0);

extern int nb_iter;

extern double alpha_initial;


extern void init_relax(double _alpha_initial, int _nb_iter, int _borne);

