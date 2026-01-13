#include "Bibliotheque.h"
using namespace std;

Bibliotheque::Bibliotheque(string n, string a, id c){
    nom = n;
    adresse = a;
    code = c;
    listeLivre = Array<Livre>();
    listeAdherent = Array<Adherent>();
}