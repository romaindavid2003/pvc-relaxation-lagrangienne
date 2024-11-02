#include "tsp.h"

// Ordre du graphe
int n;

/*
 La variable nb_noeuds compte le nombre de noeuds de l'arborescence de la recherche par separation et evaluation
*/
int nb_noeuds = 0;

/*
On code le graphe traite par une matrice n * n de structures Arete
*/
Arete ** graphe;

/*
  Pour les nombres d'aretes imposees a partir de chaque sommet du graphe.
*/
int * degre_I;

// Gardera la valeur d'un majorant de la valeur optimum
int borne = INT_MAX;

/*
  Pour conserver le meilleur tour determine depuis le debut d'une methode. 
  Ce tour est code par un tableau de pointeurs vers les structures Arete correspondant aux aretes du tour.
*/
Arete ** meilleure_solution;


// double alpha_initial;

void affiche_solution();
/* 
   Ce parametre doit valoir 0 si on n'utilise pas la relaxation lagrangienne et une valeur non nulle sinon.
*/
int faire_relax;


// int nb_iter;


void meilleur_tour(Arete **, Arete **, int, int);


void init()
{
  int i;

  graphe = (Arete **) malloc(n * sizeof(Arete *));
  for (i = 0; i < n; i++) graphe[i] =  (Arete *) malloc((i + 1) * sizeof(Arete));
  degre_I = (int *) malloc(n * sizeof(int));
  meilleure_solution = (Arete **) malloc(n * sizeof(Arete *));
}

// on ne remplit que la demi-matrice inferieure, sous la premiere diagonale
void lire(char * nom_fichier)
{
  int i, j;
  FILE * fichier;
  int max;

  fichier = fopen(nom_fichier, "r");
  fscanf(fichier, "%d", &n);
  fscanf(fichier, "%d", &max);

  init();
  for (i = 0; i < n; i++)
     for (j = 0; j <= i; j++)
       {
	 fscanf(fichier, "%d", &graphe[i][j].poids);
	 graphe[i][j].x = i;
	 graphe[i][j].y = j; 
	 graphe[i][j].statut = 2;
       }
  borne = 0;
  for (i = 0; i < n-1; i++) 
    meilleure_solution[i] = &graphe[i+1][i];
  meilleure_solution[n-1]=&graphe[n-1][0]; 
  //  affiche_solution();
  for (i = 0; i < n - 1; i++) borne += graphe[i+1][i].poids;
  borne += graphe[n-1][0].poids;
  //printf("borne initiale = %d\n", borne);
}


/*
  Trie les aretes par poids croissants en omettant les aretes exclues.
  Retourne le nombre d'aretes triees.
*/
int trier(Arete ** liste, int nb_liste)
{
  int i, j, k = 0;
  int poids_cle;
  Arete * cle;

  for (i = 0; i < nb_liste; i++)
    {
      if (liste[i] -> statut == 0) continue;
      cle = liste[i];
      poids_cle = cle -> poids;
      j = k;
      while((j > 0) && (poids_cle < liste[j - 1] -> poids)) 
	{
	  liste[j] = liste[j - 1];
	  j--;
	}
      liste[j] = cle;
      k++;
    }
  return k;
}


int poids(Arete ** un_arbre)
{
  int i, somme = 0;

  for (i = 0; i < n; i++) somme += un_arbre[i] -> poids;
  return somme;
}


void sauvegarde_solution(Arete ** un_arbre)
{
  int i;

  for (i = 0; i < n; i++) meilleure_solution[i] = un_arbre[i];
}

/*
  On utilise un algorithme issu de l'algorithme de Kruskal pour deteminer un un_arbre de poids minimum 
  parmi les un_arbres contenant les aretes imposees.
  liste et liste0 donnent l'ensemble des aretes du sous-graphe considere et les cardinaux 
  de ces listes sont respectivement nb_liste et nb_liste0. 
  Les aretes issues du sommet 0 sont dans liste0, les autre sont dns liste.
  L'ensemble des pointeurs vers les aretes de l'arbre construit est dans le tableau un_arbre.
  La suite des degres des sommets 0, 1, , n - 1 dans le un_arbre est codee dans le tableau  degres_arbre.
*/
int meilleur_un_arbre(Arete ** liste, Arete ** liste0, int nb_liste, int nb_liste0, Arete ** un_arbre, int * degres_arbre)
{
  int i, j, r = 0;
  int a, b;
  int * comp;

  comp = (int *) malloc(n * sizeof(int));
  for (i = 0; i < n; i++) 
    {
      comp[i] = i;
      degres_arbre[i] = 0;
    }
  i = 0;

  // On met les aretes imposees de liste
  for (r = 0; r < nb_liste; r++) 
    {
      if (liste[r] -> statut == 1)
	{
	  un_arbre[i] = liste[r];
	  degres_arbre[un_arbre[i] -> x]++;
	  degres_arbre[un_arbre[i] -> y]++;
	  a = comp[un_arbre[i] -> x];
	  b = comp[un_arbre[i] -> y];
	  for (j = 1; j < n; j++)
	    if (comp[j] == b) comp[j] = a;
	  i++;
	}
    }

  // On complete l'arbre sur les sommets autres que 0.
  r = 0;
  while ((r < nb_liste) && (i < n - 2))
    {
      while((r < nb_liste) && 
	     ((comp[liste[r] -> x] == comp[liste[r] -> y]) || 
	    (liste[r] -> statut != 2))) r++; 
      if (r < nb_liste) 
	{
	  un_arbre[i] = liste[r];
	  degres_arbre[un_arbre[i] -> x]++;
	  degres_arbre[un_arbre[i] -> y]++;
	  a = comp[un_arbre[i] -> x];
	  b = comp[un_arbre[i] -> y];
	  for (j = 1; j < n; j++)
	    if (comp[j] == b) comp[j] = a;
	  r++;
	  i++;
	}
    }
  if (i != n - 2) // l'arbre n'a pas pu etre construit
    {
      free(comp);
      return 0;
    }

  // On ajoute les aretes imposees issue de 0 (il doit y en avoir au plus 2)
  for (r = 0; r < nb_liste0; r++)  
      if (liste0[r] -> statut == 1) 
	{
	  un_arbre[i] = liste0[r];
	  i++;
	}
  r = 0;

  // On ajoute les aretes les plus legeres issues de 0 jusqu'a en avoir 2 en tout issues de 0
  while ((i < n) && (r < nb_liste0))
    {
      while((r < nb_liste0) && (liste0[r] -> statut != 2)) r++; 
      if (r < nb_liste0)
	{
	  un_arbre[i] = liste0[r];
	  degres_arbre[un_arbre[i] -> x]++;
	  degres_arbre[un_arbre[i] -> y]++;
	  r++; 
	  i++;
	}
    }
  free(comp);
  if (i != n ) return 0;
  else return 1;
}


/*
  Exclut toutes les aretes non imposees issues du sommet s et rajoute ces aretes au tableau exclues.
  nb_exclues est le nombre d'aretes dans le tableau exclues avant l'execution de la fonction
  Renvoie le nombre d'aretes dans le tableau exclues apres l'execution de la fonction.
*/
int exclure(int s, Arete ** exclues, int nb_exclues)
{
  int i;
  
  for (i = 0; i < s; i++)
    if (graphe[s][i].statut == 2) 
      {
	graphe[s][i].statut = 0;
	exclues[nb_exclues++] = &graphe[s][i];
	}
  for (i = s + 1 ; i < n; i++)
    if (graphe[i][s].statut == 2) 
      {
	graphe[i][s].statut = 0;
	exclues[nb_exclues++] = &graphe[i][s];
      }
  return nb_exclues;
}

int deux_exclues(int s, Arete * a1, Arete * a2, Arete ** exclues)
{
  int i;
  int nb_exclues = 0;
  
  a1 -> statut = 1;
  a2 -> statut = 1;
  degre_I[a1 -> x]++;
  degre_I[a1 -> y]++;
  degre_I[a2 -> x]++;
  degre_I[a2 -> y]++;
  nb_exclues = exclure(s, exclues, nb_exclues);
  if( a1 -> x != s)
    {
      if (degre_I[a1 -> x] == 2) 
	nb_exclues = exclure(a1 -> x, exclues, nb_exclues);
    }
  else
    {
      if (degre_I[a1 -> y] == 2) 
	nb_exclues = exclure(a1 -> y, exclues, nb_exclues);
    }
  if( a2 -> x != s)
    {
      if (degre_I[a2 -> x] == 2) 
	nb_exclues = exclure(a2 -> x, exclues, nb_exclues);
    }
  else
    {
      if (degre_I[a2 -> y] == 2) 
	nb_exclues = exclure(a2 -> y, exclues, nb_exclues);
    }
  return nb_exclues;
}

void retour_deux_exclues(Arete * a1, Arete * a2, Arete ** exclues, int nb_exclues)
{
  int i, a, b;

  a1 -> statut = 2;
  a2 -> statut = 2;
  degre_I[a1 -> x]--;
  degre_I[a1 -> y]--;
  degre_I[a2 -> x]--;
  degre_I[a2 -> y]--;
  for (i = 0; i < nb_exclues; i++)
  {
    a = exclues[i] -> x;
    b =  exclues[i] -> y;
    if (a > b) graphe[a][b].statut = 2;
    else graphe[b][a].statut = 2;
  }   
}

int imposee_exclue(Arete * a1, Arete * a2, Arete ** exclues)
{
  int nb_exclues = 0;
  int i = 0;

  a1 -> statut = 1;
  degre_I[a1 -> x]++;
  degre_I[a1 -> y]++;
  exclues[nb_exclues++] = a2;

  if (degre_I[a1 -> x] == 2) 	
    nb_exclues = exclure(a1 -> x, exclues, nb_exclues);
  if (degre_I[a1 -> y] == 2) 	
    nb_exclues = exclure(a1 -> y, exclues, nb_exclues);

  return nb_exclues;
}

void retour_imposee_exclue(Arete * a1, Arete ** exclues, int nb_exclues)
{
  int i, a, b;

  a1 -> statut = 2;
  degre_I[a1 -> x]--;
  degre_I[a1 -> y]--;
  for (i = 0; i < nb_exclues; i++)
  {
    a = exclues[i] -> x;
    b =  exclues[i] -> y;
    if (a > b) graphe[a][b].statut = 2;
    else graphe[b][a].statut = 2;
  }   
}

/*
  Fonction auxiliaire utilisee dans la fonction meilleure_tour pour effectuer le branchement
  lorsque cela est necessaire.Cette fonctio appelle meilleure_tour rendant ainsi la methode recursive.
*/
void branchement(int sommet, Arete ** liste, Arete ** liste0, int nb_liste, int nb_liste0, Arete ** un_arbre)
{
  int nb_exclues;
  Arete ** exclues;
  Arete * a, * a1 = NULL, * a2 = NULL;
  int i = 0;

  exclues = (Arete **) malloc(nb_liste * sizeof(Arete));
  if (exclues == NULL)
    {
      printf("pb memoire\n");
      exit(1);
    }
  while (a1 == NULL) 
    {
      a = un_arbre[i];
      if (((a -> x == sommet) || (a -> y == sommet))
	  && (a -> statut == 2)) a1 = a;
      i++;
    }
  while (a2 == NULL) 
    {
      a = un_arbre[i];
      if (((a -> x == sommet) || (a -> y == sommet))
	  && (a -> statut == 2)) a2 = a;
      i++;
    }
  if (degre_I[sommet] == 0)
    {
      nb_exclues = deux_exclues(sommet, a1, a2, exclues);
      meilleur_tour(liste, liste0, nb_liste, nb_liste0);
      retour_deux_exclues(a1, a2, exclues, nb_exclues);
    }
  nb_exclues = imposee_exclue(a1, a2, exclues);
  meilleur_tour(liste, liste0, nb_liste, nb_liste0);
  retour_imposee_exclue(a1,  exclues, nb_exclues); 

  graphe[a1 -> x][a1 -> y].statut = 0;
  meilleur_tour(liste, liste0, nb_liste, nb_liste0);
  graphe[a1 -> x][a1 -> y].statut = 2;
  free(exclues);
}

void liberer(Arete ** liste, Arete ** liste0, Arete ** un_arbre, int * degres_arbre)
{
  free(liste);
  free(liste0);
  free(un_arbre);
  free(degres_arbre);
}

/* 
   Determine le meilleure tour lorsque les aretes non exclues sur les sommets 1, 2, n - 1 
   et triees sont dans liste_pere et en nombre de nb_liste_pere, 
   les aretes non exclues issues du sommet 0 et triees sont dans liste0_pere et en nombre de nb_liste0_pere.
*/
void meilleur_tour(Arete ** liste_pere, Arete ** liste0_pere, 
	    int nb_liste_pere, int nb_liste0_pere)
{
  int i = 0, j, nb_E, P;
  int sommet_gros_degre = 0;
  Arete ** liste;
  Arete ** liste0;
  int nb_liste, nb_liste0;
  Arete ** un_arbre;
  int * degres_arbre;

  nb_noeuds++;
  
  liste =(Arete **) malloc(nb_liste_pere * sizeof(Arete *));
  if (liste == NULL)
    {
      printf("pb memoire\n");
      exit(1);
    }
  for (i = 0; i < nb_liste_pere; i++) liste[i] = liste_pere[i];
  nb_liste = trier(liste, nb_liste_pere);
  liste0 =(Arete **) malloc(nb_liste0_pere * sizeof(Arete *));
  if (liste0 == NULL)
    {
      printf("pb memoire\n");
      exit(1);
    }
  for (i = 0; i < nb_liste0_pere; i++) liste0[i] = liste0_pere[i];
  nb_liste0 = trier(liste0, nb_liste0_pere);

  un_arbre = (Arete **) malloc(n * sizeof(Arete *));
  if (un_arbre == NULL)
    {
      printf("pb memoire\n");
      exit(1);
    }
  degres_arbre = (int *) malloc(n * sizeof(int));
  if (degres_arbre == NULL)
    {
      printf("pb memoire\n");
      exit(1);
    }
  if (!meilleur_un_arbre(liste, liste0, nb_liste, nb_liste0, un_arbre, degres_arbre)) return;
  P = poids(un_arbre);
  if (P >= borne) 
    {
      liberer(liste, liste0, un_arbre, degres_arbre);
      return;
    }
  if ((faire_relax) && (relaxation(liste, liste0, nb_liste, nb_liste0)))
    {
      liberer(liste, liste0, un_arbre, degres_arbre);
      return;
    }
  while ((degres_arbre[sommet_gros_degre] <= 2) && (sommet_gros_degre < n)) 
    sommet_gros_degre++; 
  if (sommet_gros_degre == n) 
    {
      if (P < borne)
	{
	  borne = P;
	  //printf("borne = %d\n", borne);
	  sauvegarde_solution(un_arbre);	
	}
      liberer(liste, liste0, un_arbre, degres_arbre);
      return;
    }
  branchement(sommet_gros_degre, liste, liste0, nb_liste, nb_liste0, un_arbre);
  liberer(liste, liste0, un_arbre, degres_arbre);
}

void affiche_solution()
{
  int i, j, s;
  int * pris;

  pris = (int *) malloc(n * sizeof(int));
  for (i = 0; i < n; i++) pris[i] = 0;
  //printf("La solution : ");
  //printf("%d, %d", meilleure_solution[0] -> x, meilleure_solution[0] -> y);
  s = meilleure_solution[0] -> y;
  for (i = 1; i < n - 1; i++)
    {
      for (j = 1; j < n; j++)
	if (!pris[j])
	  {
	    if (meilleure_solution[j] -> x == s)
	      {
		s =  meilleure_solution[j] -> y;
		//printf(", %d", s);
		pris[j] = 1;
		j = n;
	      }
	    else if (meilleure_solution[j] -> y  == s)
	      {
		s =  meilleure_solution[j] -> x;
		//printf(", %d", s);
		pris[j] = 1;
		j = n;
	      }
	  }
    }
  //printf("\nPoids : %d\n", borne);
  //printf("Nombre de noeuds %d\n", nb_noeuds);
}

int main(int nb, char ** arg)
{
  int i, j, k;
  Arete ** liste;
  Arete ** liste0;
  int nb_liste, nb_liste0;	
  struct rusage buftime;
  long  start_sec, start_misec, stop_sec, stop_misec ;
  double duree;	
  double start, stop;
  
  getrusage(RUSAGE_SELF, &buftime);
  start_sec = buftime.ru_utime.tv_sec;
  start_misec = buftime.ru_utime.tv_usec;
  start =(double)start_sec + (double)start_misec/1000000;
  faire_relax = atoi(arg[2]);
  lire(arg[1]);
  if (faire_relax) init_relax(atof(arg[3]), atoi(arg[4]), borne);
  nb_liste = (n - 1) * (n - 2) / 2;
  liste =(Arete **) malloc(nb_liste * sizeof(Arete *));
  k = 0;
  for (i = 2; i < n; i++)
    for (j = 1; j < i; j++) liste[k++] = &graphe[i][j];
  nb_liste0 = n - 1;
  liste0 = (Arete **) malloc(nb_liste0 * sizeof(Arete *));
  k = 0;
  for (i = 1; i < n; i++) liste0[k++] = &graphe[i][0];


  meilleur_tour(liste, liste0, nb_liste, nb_liste0);

  affiche_solution();

  getrusage(RUSAGE_SELF, &buftime);
  stop_sec = buftime.ru_utime.tv_sec;
  stop_misec = buftime.ru_utime.tv_usec;
  stop =(double)stop_sec + (double)stop_misec/1000000;
  duree = stop - start;
  if (duree >0.06){
  printf("duree = %f secondes\n", duree);
  printf("alpha = %f\n", alpha_initial);
  printf("n iter in relax = %d\n", nb_iter);  }
  return 0;
}
