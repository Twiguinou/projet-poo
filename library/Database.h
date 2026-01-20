#ifndef PROJET_POO_LIBS_DATABASE_H
#define PROJET_POO_LIBS_DATABASE_H

#include <nlohmann/json.hpp>

#include "Adherent.h"
#include "Bibliotheque.h"
#include "Livre.h"
#include "Table.h"

struct Database
{
    Table bibliotheque {
        { "code", false, true, DT_UINT64 },
        { "nom", DT_STRING },
        { "adresse", DT_STRING }
    };
    Table adherent {
        { "numero", false, true, DT_UINT64 },
        { "nom", DT_STRING },
        { "prenom", DT_STRING },
        { "adresse", DT_STRING },
        { "bibliotheque", true, false, DT_UINT64 },
        { "maxEmprunt", DT_UINT8 }
    };
    Table livre {
        { "code", false, true, DT_UINT64 },
        { "auteur", DT_STRING },
        { "titre", DT_STRING },
        { "editeur", DT_STRING },
        { "isbn", DT_STRING },
        { "public", DT_PUBLIC }
    };
    Table bd {
        { "code", true, true, DT_UINT64 },
        { "dessinateur", DT_STRING }
    };
    Table recueilDePoesie {
        { "code", true, true, DT_UINT64 },
        { "style", DT_STYLE_POESIE }
    };
    Table roman {
        { "code", true, true, DT_UINT64 },
        { "genre", DT_GENRE }
    };
    Table pieceDeTheatre {
        { "code", true, true, DT_UINT64 },
        { "siecle", DT_UINT8 }
    };
    Table album {
        { "code", true, true, DT_UINT64 },
        { "type", DT_TYPE_ILLUSTRATIONS }
    };
    Table emprunte {
        { "livre", true, true, DT_UINT64 },
        { "adherent", true, false, DT_UINT64 }
    };
    Table detient {
        { "livre", true, true, DT_UINT64 },
        { "bibliotheque", true, false, DT_UINT64 }
    };
    Table prete {
        { "livre", true, true, DT_UINT64 },
        { "bibliotheque", true, false, DT_UINT64 }
    };

    Livre trouverLivre(std::uint64_t code) const;

    Adherent trouverAdherent(std::uint64_t numero) const;

    Bibliotheque trouverBibliotheque(std::uint64_t code) const;

    std::optional<Emprunte> trouverEmprunt(std::uint64_t codeLivre) const;

    std::optional<Detient> trouverDetient(std::uint64_t codeLivre) const;

    std::optional<Prete> trouverPret(std::uint64_t codeLivre) const;

    static std::unique_ptr<Database> read(const nlohmann::json& json);

    void ajouterBibliotheque(const Bibliotheque& data);

    void ajouterLivre(const Livre& data);

    void acheterLivre(std::uint64_t codeLivre, std::uint64_t codeBiblio);

    void supprimerLivre(std::uint64_t codeLivre);

    void ajouterAdherent(const Adherent& data);

    std::vector<BandeDessinee> bandeDessinees(std::uint64_t codeBiblio) const;

    std::vector<RecueilDePoesie> recueilsDePoesie(std::uint64_t codeBiblio) const;

    std::vector<Roman> romans(std::uint64_t codeBiblio) const;

    std::vector<PieceDeTheatre> piecesDeTheatre(std::uint64_t codeBiblio) const;

    std::vector<Album> albums(std::uint64_t codeBiblio) const;

    void emprunter(std::uint64_t numeroAdherent, std::uint64_t codeLivre);

    void retournerAdherent(std::uint64_t codeLivre);

    void preterLivre(std::uint64_t codeLivre, std::uint64_t codeBiblio);

    void retournerBiblio(std::uint64_t codeBiblio);
};

#endif
