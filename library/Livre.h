#ifndef PROJET_POO_LIVRE_H
#define PROJET_POO_LIVRE_H

#include <string>
#include <nlohmann/json.hpp>

#include "Table.h"

struct Livre
{
    enum class Public
    {
        ADULTES,
        ADOS,
        JEUNESSES,
        TOUT_PUBLIC
    };

    const std::uint64_t code;
    const std::string auteur;
    const std::string titre;
    const std::string editeur;
    const std::string isbn;
    const Public _public;

    Livre(std::uint64_t code, std::string auteur, std::string titre, std::string editeur, std::string isbn, Public _public)
    : code(code), auteur(std::move(auteur)), titre(std::move(titre)), editeur(std::move(editeur)), isbn(std::move(isbn)), _public(_public) {}

    Livre(const Livre& livre) : Livre(livre.code, livre.auteur, livre.titre, livre.editeur, livre.isbn, livre._public) {}

    virtual ~Livre() = default;
};
static const BuiltInDataType<Livre::Public> DT_PUBLIC;

struct BandeDessinee : Livre
{
    const std::string dessinateur;

    BandeDessinee(const Livre& livre, std::string dessinateur) : Livre(livre), dessinateur(std::move(dessinateur)) {}
};

struct RecueilDePoesie : Livre
{
    enum class Style
    {
        VERS,
        PROSE,
        VERS_ET_PROSE
    };

    const Style style;

    RecueilDePoesie(const Livre& livre, Style style) : Livre(livre), style(style) {}
};
static const BuiltInDataType<RecueilDePoesie::Style> DT_STYLE_POESIE;

struct Roman : Livre
{
    enum class Genre
    {
        LITTERATURE,
        ROMAN_NOIR,
        ROMAN_POLICIER,
        ROMAN_ANIMALIER,
        ROMAN_DAMOUR,
        ROMAN_DE_MOEURS,
        ROMAN_GOTHIQUE,
        ROMAN_COURTOIS,
        ROMAN_EPISTOLAIRE,
        ROMAN_FEUILLETON,
        ROMAN_GRAPHIQUE,
        ROMAN_HISTORIQUE,
        ROMAN_PHOTO,
        ROMAN_PICARESQUE,
        ROMAN_MEMOIRES,
        ROMAN_POPULAIRE,
        ROMAN_DAVENTURES,
        ROMAN_DANTICIPATION,
        ROMAN_DESPIONNAGE,
        ROMAN_DAPPRENTISSAGE,
        ROMAN_DE_CHEVALERIE,
        ROMAN_AUTOBIOGRAPHIQUE,
        NOUVEAU_ROMAN,
        ROMAN_CHEVALERESQUE,
        CONTE,
        NOUVELLE,
        AUTRE
    };

    const Genre genre;

    Roman(const Livre& livre, Genre genre) : Livre(livre), genre(genre) {}
};
static const BuiltInDataType<Roman::Genre> DT_GENRE;

struct PieceDeTheatre : Livre
{
    const std::uint8_t siecle;

    PieceDeTheatre(const Livre& livre, std::uint8_t siecle) : Livre(livre), siecle(siecle) {}
};

struct Album : Livre
{
    enum class Type
    {
        PHOTOS,
        DESSINS,
        PHOTOS_ET_DESSINS
    };

    const Type type;

    Album(const Livre& livre, Type type) : Livre(livre), type(type) {}
};
static const BuiltInDataType<Album::Type> DT_TYPE_ILLUSTRATIONS;

NLOHMANN_JSON_SERIALIZE_ENUM(Livre::Public, {
    { Livre::Public::ADULTES, "ADULTES" },
    { Livre::Public::ADOS, "ADOS" },
    { Livre::Public::JEUNESSES, "JEUNESSES" },
    { Livre::Public::TOUT_PUBLIC, "TOUT_PUBLIC" }
})
NLOHMANN_JSON_SERIALIZE_ENUM(RecueilDePoesie::Style, {
    { RecueilDePoesie::Style::VERS, "VERS" },
    { RecueilDePoesie::Style::PROSE, "PROSE" },
    { RecueilDePoesie::Style::VERS_ET_PROSE, "VERS_ET_PROSE" }
})
NLOHMANN_JSON_SERIALIZE_ENUM(Roman::Genre, {
    { Roman::Genre::LITTERATURE, "LITTERATURE" },
    { Roman::Genre::ROMAN_NOIR, "ROMAN_NOIR" },
    { Roman::Genre::ROMAN_POLICIER, "ROMAN_POLICIER" },
    { Roman::Genre::ROMAN_ANIMALIER, "ROMAN_ANIMALIER" },
    { Roman::Genre::ROMAN_DAMOUR, "ROMAN_DAMOUR" },
    { Roman::Genre::ROMAN_DE_MOEURS, "ROMAN_DE_MOEURS" },
    { Roman::Genre::ROMAN_GOTHIQUE, "ROMAN_GOTHIQUE" },
    { Roman::Genre::ROMAN_COURTOIS, "ROMAN_COURTOIS" },
    { Roman::Genre::ROMAN_EPISTOLAIRE, "ROMAN_EPISTOLAIRE" },
    { Roman::Genre::ROMAN_FEUILLETON, "ROMAN_FEUILLETON" },
    { Roman::Genre::ROMAN_GRAPHIQUE, "ROMAN_GRAPHIQUE" },
    { Roman::Genre::ROMAN_HISTORIQUE, "ROMAN_HISTORIQUE" },
    { Roman::Genre::ROMAN_PHOTO, "ROMAN_PHOTO" },
    { Roman::Genre::ROMAN_PICARESQUE, "ROMAN_PICARESQUE" },
    { Roman::Genre::ROMAN_MEMOIRES, "ROMAN_MEMOIRES" },
    { Roman::Genre::ROMAN_POPULAIRE, "ROMAN_POPULAIRE" },
    { Roman::Genre::ROMAN_DAVENTURES, "ROMAN_DAVENTURES" },
    { Roman::Genre::ROMAN_DANTICIPATION, "ROMAN_DANTICIPATION" },
    { Roman::Genre::ROMAN_DESPIONNAGE, "ROMAN_DESPIONNAGE" },
    { Roman::Genre::ROMAN_DAPPRENTISSAGE, "ROMAN_DAPPRENTISSAGE" },
    { Roman::Genre::ROMAN_DE_CHEVALERIE, "ROMAN_DE_CHEVALERIE" },
    { Roman::Genre::ROMAN_AUTOBIOGRAPHIQUE, "ROMAN_AUTOBIOGRAPHIQUE" },
    { Roman::Genre::NOUVEAU_ROMAN, "NOUVEAU_ROMAN" },
    { Roman::Genre::ROMAN_CHEVALERESQUE, "ROMAN_CHEVALERESQUE" },
    { Roman::Genre::CONTE, "CONTE" },
    { Roman::Genre::NOUVELLE, "NOUVELLE" },
    { Roman::Genre::AUTRE, "AUTRE" }
})
NLOHMANN_JSON_SERIALIZE_ENUM(Album::Type, {
    { Album::Type::PHOTOS, "PHOTOS" },
    { Album::Type::DESSINS, "DESSINS" },
    { Album::Type::PHOTOS_ET_DESSINS, "PHOTOS_ET_DESSINS" }
})

#endif
