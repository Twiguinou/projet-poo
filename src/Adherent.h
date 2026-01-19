#ifndef PROJET_POO_ADHERENT_H
#define PROJET_POO_ADHERENT_H

#include <cstdint>
#include <string>

struct Adherent
{
    const std::uint64_t numero;
    const std::string nom;
    const std::string prenom;
    const std::string adresse;
    const std::uint64_t bibliotheque;
    const std::uint8_t maxEmprunt;

    Adherent(std::uint64_t numero, std::string nom, std::string prenom, std::string adresse, std::uint64_t bibliotheque, std::uint8_t maxEmprunt)
    : numero(numero), nom(std::move(nom)), prenom(std::move(prenom)), adresse(std::move(adresse)), bibliotheque(bibliotheque), maxEmprunt(maxEmprunt) {}
};

#endif
