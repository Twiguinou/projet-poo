#include "Adherent.h"

Adherent::Adherent(string n, string p, string a, int n, Bibliotheque b){
    nom = n;
    prenom = p;
    adresse = a;
    num = n;
    biblio = b;
}

Adherent::~Adherent(){
    delete listeLivreEmprunt;
}