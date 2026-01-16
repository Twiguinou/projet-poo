#include "Adherent.h"

Adherent::Adherent(std::string n, std::string p, std::string a, id n, Bibliotheque& b, std::size_t nbLvAuto) :
nom(n), prenom(p), adresse(a), numero(n), biblio(b), nombreLivreAuto(nbLvAuto)
{}

void Adherent::emprunter(id idLivre)
{
    if (listeLivreEmprunt.getSize() == nbLvAuto)
    {
        throw idLivre;
    }
}

