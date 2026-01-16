#ifndef ADHERENT_H
#define ADHERENT_H

#include "Array.h"
using namespace std;

class Adherent(){
    private :
    string nom;
    string prenom;
    string adresse;
    id numero;
    Bibliotheque biblio;
    Array<Livre> listeLivreEmprunt;
    int nombreLivreAuto;
    public:
    Adherent(string nom, string prenom, string adresse, id numero, Bibliotheque biblio);
    ~Adherent();
    void emprunter(id);
}




#endif
