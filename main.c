// Lib
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// un joueur
struct joueur {
  int id;
  int score;
};

/* Arbre d'evaluation */
/* Un père peu avoir plusieurs fils. Les fils sont sous forme de liste chainée*/
struct arbre_evaluation {
  int poids;   // poids de la fonction d'évaluation
  int etat;    // est l'état lors que la création de l'arbre
  int coup;    // le coup joué du noeuds 
  int *grille; // grille du jeu
  struct arbre_evaluation *suivant; // noeuds voisins (les fils du père) 
  struct arbre_evaluation *fils;    // noeuds fils
};

/* Liste des fonctions */

/* Affichage du jeu */
void afficher_jeu(int *grille);
void afficher_arbre(struct arbre_evaluation *arbre, int deep);

/* Logique du jeu */
int evaluer_si_fin_de_partie(int *grille, int id);
int choix_du_joueur(int *grille);
int choix_du_robot(int *grille);
void initialiser_grille(int *grille);
int choisir_case(int *grille, struct joueur *joueur,
                 struct arbre_evaluation *arbre_courant);

/* IA */
struct arbre_evaluation *minimax(int *grille, struct arbre_evaluation *arbre,
                                 int etat, int deep);
struct arbre_evaluation *initialiser_arbre(struct arbre_evaluation *arbre,
                                           int *grille);
int evalue_poids_fils(struct arbre_evaluation *arbre, int deep, int joueur_id);
int calcul_min(struct arbre_evaluation *arbre, int deep, int joueur_id);
int calcul_max(struct arbre_evaluation *arbre, int deep, int joueur_id);
struct arbre_evaluation *evaluation(struct arbre_evaluation *arbre,
                                    int joueur_id);
int smart_bot(struct arbre_evaluation *arbre_courant, int joueur_id);
struct arbre_evaluation *update_arbre(struct arbre_evaluation *arbre, int coup,
                                      int id_joueur);

/* free */
void free_arbre(struct arbre_evaluation *arbre);

/* Autres fonctions */
void clone_grille(int grille_source[9], int grille_dest[9]);
int est_feuille(struct arbre_evaluation *arbre);
void afficher_logo();

/* Fonctions */

// evalue si la partie est fini (retourne vrai si le joueur id à gagner, tous 
// les autres cas retourne faux)
int evaluer_si_fin_de_partie(int *grille, int id) {
  int ligne = 0;
  int colonne = 0;
  int diag1 = 0;
  int diag2 = 0;

  // Evaluation de la ligne
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (grille[i * 3 + j] == id) {
        ligne++;
      }
    }
    if (ligne == 3) {
      return 1;
    } else {
      ligne = 0;
    }
  }

  // Evaluation de la colonne
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (grille[j * 3 + i] == id) {
        colonne++;
      }
    }
    if (colonne == 3) {
      return 1;
    } else {
      colonne = 0;
    }
  }

  // Evaluation de la diag1
  for (int i = 0; i < 3; i++) {
    if (grille[i * 3 + i] == id) {
      diag1++;
    }
  }
  if (diag1 == 3) {
    return 1;
  }

  // Evaluation de la diag2
  for (int i = 0; i < 3; i++) {
    if (grille[i * 3 + 2 - i] == id) {
      diag2++;
    }
  }
  if (diag2 == 3) {
    return 1;
  }

  return 0;
}

// clone une grille
void clone_grille(int grille_source[9], int grille_dest[9]) {
  int i;
  for (i = 0; i < 9; i++) {
    grille_dest[i] = grille_source[i];
  }
}

// afficher le jeu
void afficher_jeu(int *grille) {
  printf("[X]");
  for (int i = 0; i < 3; i++) {
    printf("[%d]", i);
  }
  printf("\n[0]");
  int j = 1;
  for (int i = 0; i < 9; i++) {
    printf("|%d|", grille[i]);
    if ((i + 1) % 3 == 0) {
      if (i < 6) {
        printf("\n[%d]", j);
        j++;
      } else {
        printf("\n");
      }
    }
  }
  printf("\n");
}

// afficher un arbre d'évaluation 
void afficher_arbre(struct arbre_evaluation *arbre, int deep) {
  if (arbre == NULL) {
    return;
  }

  if (arbre->grille != NULL) {
    printf("deep = %d\n", deep);
    afficher_jeu(arbre->grille);
  } else {
    printf("root\n");
  }

  afficher_arbre(arbre->fils, deep + 1);
  if (deep != 0) { // n'affiche pas les voisins de la racine
    afficher_arbre(arbre->suivant, deep);
  }
}

// algo minimax
// créer une arbre avec toutes les possibilités de jeu
struct arbre_evaluation *minimax(int *grille, struct arbre_evaluation *arbre,
                                 int etat, int deep) {
  struct arbre_evaluation *head_fils = NULL;

  if (evaluer_si_fin_de_partie(grille, 1)) {
    return NULL;
  }
  if (evaluer_si_fin_de_partie(grille, 2)) {
    return NULL;
  }

  for (int i = 0; i < 9; i++) {
    if (grille[i] == 0) {
      // création du noeuds 
      struct arbre_evaluation *arbre_fils = NULL;
      arbre_fils = malloc(sizeof(struct arbre_evaluation));
      arbre_fils->fils = NULL;
      arbre_fils->suivant = NULL;
      arbre_fils->grille = NULL;

      int *grille_clone = NULL;
      grille_clone = malloc(sizeof(int) * 9);

      clone_grille(grille, grille_clone);
      grille_clone[i] = etat + 1;
      arbre_fils->coup = i;
      arbre_fils->grille = grille_clone;

      // ajout du fils dans la liste de noeuds
      struct arbre_evaluation *arbre_fils_suivant = head_fils;

      if (arbre_fils_suivant == NULL) {
        head_fils = arbre_fils;
        head_fils->fils =
            minimax(grille_clone, head_fils->fils, (etat + 1) % 2, deep + 1);
      } else {
        while (arbre_fils_suivant->suivant != NULL) {
          arbre_fils_suivant = arbre_fils_suivant->suivant;
        }
        arbre_fils_suivant->suivant = arbre_fils;
        arbre_fils_suivant = arbre_fils_suivant->suivant;
        arbre_fils_suivant->fils = minimax(
            grille_clone, arbre_fils_suivant->fils, (etat + 1) % 2, deep + 1);
      }
    }
  }
  return head_fils;
}

// initalise l'arbre d'évaluation de tous les coups possibles 
struct arbre_evaluation *initialiser_arbre(struct arbre_evaluation *arbre,
                                           int *grille) {
  int grille_clone[9];
  clone_grille(grille, grille_clone);

  arbre = malloc(sizeof(struct arbre_evaluation));
  arbre->fils = NULL;
  arbre->suivant = NULL;
  arbre->grille = NULL;

  arbre->fils = minimax(grille, arbre->fils, 0, 0);
  return arbre;
}

// Permet au joueur de jouer
int choix_du_joueur(int *grille) {
  int n = 0;

  scanf("%d", &n);
  // Get input value
  while (n > 0 && n < 10 && grille[n] != 0) {
    printf("Entrez un nombre entre 0 et 8 : ");
    scanf("%d", &n);
  }
  return n;
}

// Permet à un robot de jouer au hasard 
int choix_du_robot(int *grille) {
  int n = (rand() % 9);
  while (grille[n] != 0) {
    n = (rand() % 9);
  }
  return n;
}

// initalise la grille a 0
void initialiser_grille(int *grille) {
  for (int i = 0; i < 9; i++) {
    grille[i] = 0;
  }
}

// free arbre
void free_arbre(struct arbre_evaluation *arbre) {
  if (arbre == NULL) {
    return;
  }
  if (arbre->grille != NULL) {
    free(arbre->grille);
  }

  free_arbre(arbre->fils);
  free_arbre(arbre->suivant);
  free(arbre);
}

// permet de savoir si un arbre est une feuille
int est_feuille(struct arbre_evaluation *arbre) {
  if (arbre == NULL) {
    return -1;
  }

  if (arbre->fils == NULL) {
    return 1;
  }
  return 0;
}

// fonction d'évaluation d'une feuille 
int evalue_poids_fils(struct arbre_evaluation *arbre, int deep, int joueur_id) {
  int *grille = arbre->grille;
  if (evaluer_si_fin_de_partie(grille, joueur_id) == 1) {
    return 10 - deep;
  }
  if (evaluer_si_fin_de_partie(grille, 1 + (joueur_id % 2)) == 1) {
    return -10 + deep;
  }
  return 0;
}

// calcul le minimum des fils
int calcul_min(struct arbre_evaluation *arbre, int deep, int joueur_id) {
  if (arbre == NULL) {
    printf("arbre null");
    return -1;
  }

  struct arbre_evaluation *head = arbre;
  struct arbre_evaluation *arbre_fils = arbre->fils;

  while (arbre_fils != NULL) {
    if (est_feuille(arbre_fils)) {
      arbre_fils->poids = evalue_poids_fils(arbre_fils, deep, joueur_id);
    } else {
      arbre_fils->poids = calcul_max(arbre_fils, deep + 1, joueur_id);
    }
    arbre_fils = arbre_fils->suivant;
  }

  arbre = head;
  arbre_fils = arbre->fils;

  int min = arbre_fils->poids;

  while (arbre_fils != NULL) {
    if (arbre_fils->poids < min) {
      min = arbre_fils->poids;
    }
    arbre_fils = arbre_fils->suivant;
  }
  return min;
}

// calcul le maximum des fils
int calcul_max(struct arbre_evaluation *arbre, int deep, int joueur_id) {
  if (arbre == NULL) {
    printf("arbre null");
    return -1;
  }
  struct arbre_evaluation *head = arbre;
  struct arbre_evaluation *arbre_fils = arbre->fils;

  while (arbre_fils != NULL) {
    if (est_feuille(arbre_fils)) {
      arbre_fils->poids = evalue_poids_fils(arbre_fils, deep, joueur_id);
    } else {
      arbre_fils->poids = calcul_min(arbre_fils, deep + 1, joueur_id);
    }
    arbre_fils = arbre_fils->suivant;
  }

  arbre = head;
  arbre_fils = arbre->fils;

  int max = arbre_fils->poids;

  while (arbre_fils != NULL) {
    if (arbre_fils->poids > max) {
      max = arbre_fils->poids;
    }
    arbre_fils = arbre_fils->suivant;
  }

  return max;
}

// décoration des poids sur l'arbre d'evaluation
struct arbre_evaluation *evaluation(struct arbre_evaluation *arbre,
                                    int joueur_id) {
  if (est_feuille(arbre)) {
    arbre->poids = 0;
    return arbre;
  }
  arbre->poids = calcul_max(arbre, 0, joueur_id);
  return arbre;
}

// permet à l'IA de jouer, elle établi une stratégie sur l'arbre courant
int smart_bot(struct arbre_evaluation *arbre_courant, int joueur_id) {

  // decorer l'arbre
  arbre_courant = evaluation(arbre_courant, joueur_id);

  // recherche de la branche max (= établir stratégie)
  struct arbre_evaluation *branche = arbre_courant->fils;
  struct arbre_evaluation *arbre_max = branche;

  int max = arbre_courant->poids;

  while (branche != NULL) {
    if (branche->poids == max) {
      break;
    }
    branche = branche->suivant;
  }

  return branche->coup;
}

// met à jour l'arbre en fonction des coups joué
struct arbre_evaluation *update_arbre(struct arbre_evaluation *arbre, int coup,
                                      int id_joueur) {
  arbre = arbre->fils;
  while (arbre->suivant != NULL) {
    if (arbre->grille[coup] == id_joueur) {
      return arbre;
    }
    arbre = arbre->suivant;
  }
  return arbre;
}

// choisir une case en fonction du joueur qui joue.
int choisir_case(int *grille, struct joueur *joueur,
                 struct arbre_evaluation *arbre_courant) {
  if (joueur->id == 1) {
    // return choix_du_robot(grille);
    // return choix_du_joueur(grille);
    return smart_bot(arbre_courant, joueur->id);
  } else {
    // return choix_du_robot(grille);
    return choix_du_joueur(grille);
    // return smart_bot(arbre_courant, joueur->id);
  }
  fprintf(stderr, "Erreur : le type du joueur id non reconnu");
  return -1;
}

// affiche le logo
void afficher_logo() {
  printf(" __  __  ___  ____  ____ ___ ___  _   _ \n");
  printf("|  \\/  |/ _ \\|  _ \\|  _ \\_ _/ _ \\| \\ | |\n");
  printf("| |\\/| | | | | |_) | |_) | | | | |  \\| |\n");
  printf("| |  | | |_| |  _ <|  __/| | |_| | |\\  |\n");
  printf("|_|  |_|\\___/|_| \\_\\_|  |___\\___/|_| \\_|\n");
  printf("\n");
}

// Function main
int main() {
  /* Init joueurs */
  struct joueur *joueur1 = NULL;
  struct joueur *joueur2 = NULL;

  joueur1 = malloc(sizeof(struct joueur *));
  joueur2 = malloc(sizeof(struct joueur *));

  joueur1->id = 1;
  joueur2->id = 2;

  joueur1->score = 0;
  joueur2->score = 0;

  /* Init random */
  srand(time(NULL));
  int tour = 0;

  /* Init variables */
  struct joueur *joueur;

  struct arbre_evaluation *racine = NULL;
  struct arbre_evaluation *arbre_courant =
      NULL; // racine courant selon le coups choisis

  int grille[9];
  initialiser_grille(grille);

  racine = initialiser_arbre(racine, grille);
  arbre_courant = racine;

  afficher_logo();

  // début  du jeu 
  for (int i = 0; i < 9; i++) {
    // choix du joueur en fonction du tour
    if (tour == 0) {
      joueur = joueur1;
    } else {
      joueur = joueur2;
    }

    tour = 1 - tour;

    afficher_jeu(grille);
    printf("joueur %d choisi un emplacement\n", joueur->id);

    int choix = choisir_case(grille, joueur, arbre_courant);
    grille[choix] = joueur->id;

    // mise a jour de l'abre pour l'ia (met à jour la racine)
    arbre_courant = update_arbre(arbre_courant, choix, joueur->id);

    // évalue si c'est la fin de la partie 
    if (evaluer_si_fin_de_partie(grille, joueur->id) == 1) {
      printf("Joueur %d à gagné\n", joueur->id);
      joueur->score++;
      break;
    }
    if (i == 8) {
      printf("Match nul\n");
    }

    printf("joueur suivant\n");
  }

  // affiche le résultat final 
  afficher_jeu(grille);
  printf("Score joueur 1 : %d\n", joueur1->score);
  printf("Score joueur 2 : %d\n", joueur2->score);

  // free
  free_arbre(racine);
  free(joueur1);
  free(joueur2);
  return 0;
}
