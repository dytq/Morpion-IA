# Morpion-IA
Implémentation de l'algorithme minimax pour un morpion en C
# Compilation 
gcc main.c
# Execution 
./a.out
# Hack
Possible de changer le type de joueurs, soit un robot qui joue aléatoirement, soit un smart_bot qui joue intelligement ou bien un joueur qui attend en entrée des coordonnées entre 0 et 8
Ligne 414
```c
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
```
# auteur 
Dedarally Taariq
