#ifndef ADHERENT_H
#define ADHERENT_H

#include "Array.h"
#include "Common.h"
#include <string>

class Adherent()
{
private:
    std::string nom;
    std::string prenom;
    std::string adresse;
    id numero;
    Bibliotheque& biblio;
    Array<Livre&> listeLivreEmprunt;
    std::size_t nombreLivreAuto;

public:
    Adherent(std::string nom, std::string prenom, std::string adresse, id numero, Bibliotheque& biblio, std::size_t
    nombreLivreAuto) : nom(nom), prenom(prenom), adresse(adresse), numero(numero), biblio(biblio),
    nombreLivreAuto(nombreLivreAuto) :  {}

    void emprunter(id idLivre)
    {
    }
}




#endif
