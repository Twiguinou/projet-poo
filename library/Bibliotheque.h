#ifndef PROJET_POO_BIBLIOTHEQUE_H
#define PROJET_POO_BIBLIOTHEQUE_H

#include <cstdint>
#include <string>

struct Bibliotheque
{
    const std::uint64_t code;
    const std::string nom;
    const std::string adresse;

    Bibliotheque(std::uint64_t code, std::string nom, std::string adresse)
    : code(code), nom(std::move(nom)), adresse(std::move(adresse)) {}
};

struct Emprunte
{
    const std::uint64_t livre;
    const std::uint64_t adherent;
};

struct Detient
{
    const std::uint64_t livre;
    const std::uint64_t bibliotheque;
};

struct Prete
{
    const std::uint64_t livre;
    const std::uint64_t bibliotheque;
};

#endif
