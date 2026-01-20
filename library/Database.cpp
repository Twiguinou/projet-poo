#include "Database.h"

std::unique_ptr<Database> Database::read(const nlohmann::json& json)
{
    auto database = std::make_unique<Database>();

    for (const auto& element : json.at("bibliotheques").items())
    {
        const nlohmann::json& desc = element.value();
        database->bibliotheque.append(
            desc.at("code").get<std::uint64_t>(),
            desc.at("nom").get<std::string>(),
            desc.at("adresse").get<std::string>()
        );
    }

    for (const auto& element : json.at("livres").items())
    {
        const nlohmann::json& desc = element.value();
        std::uint64_t code = desc.at("code").get<std::uint64_t>();

        database->livre.append(
            code,
            desc.at("auteur").get<std::string>(),
            desc.at("titre").get<std::string>(),
            desc.at("editeur").get<std::string>(),
            desc.at("isbn").get<std::string>(),
            desc.at("public").get<Livre::Public>()
        );

        const auto& dessinateur = desc.find("dessinateur");
        if (dessinateur != desc.end())
        {
            database->bd.append(code, dessinateur->get<std::string>());
        }

        const auto& style = desc.find("style");
        if (style != desc.end())
        {
            database->recueilDePoesie.append(code, style->get<RecueilDePoesie::Style>());
        }

        const auto& genre = desc.find("genre");
        if (genre != desc.end())
        {
            database->roman.append(code, genre->get<Roman::Genre>());
        }

        const auto& siecle = desc.find("siecle");
        if (siecle != desc.end())
        {
            database->pieceDeTheatre.append(code, siecle->get<std::uint8_t>());
        }

        const auto& type = desc.find("type");
        if (type != desc.end())
        {
            database->album.append(code, type->get<Album::Type>());
        }

        const auto& bibliotheque = desc.find("bibliotheque");
        if (bibliotheque != desc.end())
        {
            database->detient.append(code, bibliotheque->get<std::uint64_t>());
        }
    }

    for (const auto& element : json.at("prete").items())
    {
        const nlohmann::json& desc = element.value();
        database->preterLivre(
            desc.at("livre").get<std::uint64_t>(),
            desc.at("bibliotheque").get<std::uint64_t>()
        );
    }

    for (const auto& element : json.at("adherents").items())
    {
        const nlohmann::json& desc = element.value();
        std::uint64_t numero = desc.at("numero").get<std::uint64_t>();

        database->adherent.append(
            numero,
            desc.at("nom").get<std::string>(),
            desc.at("prenom").get<std::string>(),
            desc.at("adresse").get<std::string>(),
            desc.at("bibliotheque").get<std::uint64_t>(),
            desc.at("maxEmprunt").get<std::uint8_t>()
        );

        for (const auto& empruntElement : desc.at("emprunts").items())
        {
            const nlohmann::json& empruntDesc = empruntElement.value();
            database->emprunter(numero, empruntDesc.get<std::uint64_t>());
        }
    }

    return database;
}

void Database::ajouterBibliotheque(const Bibliotheque& data)
{
    bibliotheque.append(data.code, data.nom, data.adresse);
}

void Database::ajouterLivre(const Livre& data)
{
    livre.append(data.code, data.auteur, data.titre, data.editeur, data.isbn, data._public);
    try
    {
        auto data_bd = dynamic_cast<const BandeDessinee&>(data);
        bd.append(data_bd.code, data_bd.dessinateur);
        return;
    }
    catch (const std::bad_cast&) {}

    try
    {
        auto data_poesie = dynamic_cast<const RecueilDePoesie&>(data);
        recueilDePoesie.append(data_poesie.code, data_poesie.style);
        return;
    }
    catch (const std::bad_cast&) {}

    try
    {
        auto data_roman = dynamic_cast<const Roman&>(data);
        roman.append(data_roman.code, data_roman.genre);
        return;
    }
    catch (const std::bad_cast&) {}

    try
    {
        auto data_piece = dynamic_cast<const PieceDeTheatre&>(data);
        pieceDeTheatre.append(data_piece.code, data_piece.siecle);
        return;
    }
    catch (const std::bad_cast&) {}

    try
    {
        auto data_album = dynamic_cast<const Album&>(data);
        album.append(data_album.code, data_album.type);
    }
    catch (const std::bad_cast&) {}
}

void Database::acheterLivre(std::uint64_t codeLivre, std::uint64_t codeBiblio)
{
    if (!livre.contains(codeLivre))
    {
        throw std::runtime_error("Ce livre n'existe pas.");
    }

    if (!bibliotheque.contains(codeBiblio))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    detient.append(codeLivre, codeBiblio);
}

void Database::supprimerLivre(std::uint64_t codeLivre)
{
    if (emprunte.contains(codeLivre))
    {
        throw std::runtime_error("Ce livre est toujours emprunté.");
    }

    if (prete.contains(codeLivre))
    {
        throw std::runtime_error("Ce livre est toujours prêté à une autre bibliothèque.");
    }

    detient.remove(codeLivre);
}

void Database::ajouterAdherent(const Adherent& data)
{
    if (!bibliotheque.contains(data.bibliotheque))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    adherent.append(data.numero, data.nom, data.prenom, data.adresse, data.bibliotheque, data.maxEmprunt);
}

Livre Database::trouverLivre(std::uint64_t code) const
{
    auto data = livre.findOrThrow<std::uint64_t, std::string, std::string, std::string, std::string, Livre::Public>(code);
    return { std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data), std::get<4>(data), std::get<5>(data) };
}

Adherent Database::trouverAdherent(std::uint64_t numero) const
{
    auto data = adherent.findOrThrow<std::uint64_t, std::string, std::string, std::string, std::uint64_t, std::uint8_t>(numero);
    return { std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data), std::get<4>(data), std::get<5>(data) };
}

Bibliotheque Database::trouverBibliotheque(std::uint64_t code) const
{
    auto data = bibliotheque.findOrThrow<std::uint64_t, std::string, std::string>(code);
    return { std::get<0>(data), std::get<1>(data), std::get<2>(data) };
}

std::optional<Emprunte> Database::trouverEmprunt(std::uint64_t codeLivre) const
{
    auto data_opt = emprunte.find<std::uint64_t, std::uint64_t>(codeLivre);
    if (data_opt.has_value())
    {
        return Emprunte(std::get<0>(*data_opt), std::get<1>(*data_opt));
    }

    return std::nullopt;
}

std::optional<Detient> Database::trouverDetient(std::uint64_t codeLivre) const
{
    auto data_opt = detient.find<std::uint64_t, std::uint64_t>(codeLivre);
    if (data_opt.has_value())
    {
        return Detient(std::get<0>(*data_opt), std::get<1>(*data_opt));
    }

    return std::nullopt;
}

std::optional<Prete> Database::trouverPret(std::uint64_t codeLivre) const
{
    auto data_opt = prete.find<std::uint64_t, std::uint64_t>(codeLivre);
    if (data_opt.has_value())
    {
        return Prete(std::get<0>(*data_opt), std::get<1>(*data_opt));
    }

    return std::nullopt;
}

std::vector<BandeDessinee> Database::bandeDessinees(std::uint64_t codeBiblio) const
{
    if (!bibliotheque.contains(codeBiblio))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    std::vector<BandeDessinee> result;
    std::function func = [&](std::uint64_t detientCodeLivre, std::uint64_t detientCodeBiblio)
    {
        if (detientCodeBiblio != codeBiblio || !bd.contains(detientCodeLivre))
        {
            return;
        }

        auto dessinateur = std::get<1>(bd.findOrThrow<std::uint64_t, std::string>(detientCodeLivre));
        result.emplace_back(trouverLivre(detientCodeLivre), dessinateur);
    };
    detient.forEach(func);
    return result;
}

std::vector<RecueilDePoesie> Database::recueilsDePoesie(std::uint64_t codeBiblio) const
{
    if (!bibliotheque.contains(codeBiblio))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    std::vector<RecueilDePoesie> result;
    std::function func = [&](std::uint64_t detientCodeLivre, std::uint64_t detientCodeBiblio)
    {
        if (detientCodeBiblio != codeBiblio || !recueilDePoesie.contains(detientCodeLivre))
        {
            return;
        }

        auto style = std::get<1>(recueilDePoesie.findOrThrow<std::uint64_t, RecueilDePoesie::Style>(detientCodeLivre));
        result.emplace_back(trouverLivre(detientCodeLivre), style);
    };
    detient.forEach(func);
    return result;
}

std::vector<Roman> Database::romans(std::uint64_t codeBiblio) const
{
    if (!bibliotheque.contains(codeBiblio))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    std::vector<Roman> result;
    std::function func = [&](std::uint64_t detientCodeLivre, std::uint64_t detientCodeBiblio)
    {
        if (detientCodeBiblio != codeBiblio || !roman.contains(detientCodeLivre))
        {
            return;
        }

        auto genre = std::get<1>(roman.findOrThrow<std::uint64_t, Roman::Genre>(detientCodeLivre));
        result.emplace_back(trouverLivre(detientCodeLivre), genre);
    };
    detient.forEach(func);
    return result;
}

std::vector<PieceDeTheatre> Database::piecesDeTheatre(std::uint64_t codeBiblio) const
{
    if (!bibliotheque.contains(codeBiblio))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    std::vector<PieceDeTheatre> result;
    std::function func = [&](std::uint64_t detientCodeLivre, std::uint64_t detientCodeBiblio)
    {
        if (detientCodeBiblio != codeBiblio || !pieceDeTheatre.contains(detientCodeLivre))
        {
            return;
        }

        auto siecle = std::get<1>(pieceDeTheatre.findOrThrow<std::uint64_t, std::uint8_t>(detientCodeLivre));
        result.emplace_back(trouverLivre(detientCodeLivre), siecle);
    };
    detient.forEach(func);
    return result;
}

std::vector<Album> Database::albums(std::uint64_t codeBiblio) const
{
    if (!bibliotheque.contains(codeBiblio))
    {
        throw std::runtime_error("Cette bibliothèque n'existe pas.");
    }

    std::vector<Album> result;
    std::function func = [&](std::uint64_t detientCodeLivre, std::uint64_t detientCodeBiblio)
    {
        if (detientCodeBiblio != codeBiblio || !album.contains(detientCodeLivre))
        {
            return;
        }

        auto type = std::get<1>(album.findOrThrow<std::uint64_t, Album::Type>(detientCodeLivre));
        result.emplace_back(trouverLivre(detientCodeLivre), type);
    };
    detient.forEach(func);
    return result;
}

void Database::emprunter(std::uint64_t numeroAdherent, std::uint64_t codeLivre)
{
    Adherent a = trouverAdherent(numeroAdherent);
    std::function getEmpruntsFunc = [numeroAdherent](std::uint64_t _, std::uint64_t empruntAdherent)
    {
        return numeroAdherent == empruntAdherent;
    };
    if (emprunte.find_all(getEmpruntsFunc).size() == a.maxEmprunt)
    {
        throw std::runtime_error("Cet adhérent ne peut plus emprunter.");
    }

    auto empruntActuel = trouverEmprunt(codeLivre);
    if (empruntActuel.has_value())
    {
        if (empruntActuel->adherent == numeroAdherent)
        {
            return;
        }

        throw std::runtime_error("Ce livre est déjà emprunté.");
    }

    auto d = trouverDetient(codeLivre);
    if (!d.has_value())
    {
        throw std::runtime_error("Ce livre n'est pas détenu par une bibliothèque.");
    }

    auto pret = trouverPret(codeLivre);
    if (d->bibliotheque == a.bibliotheque)
    {
        if (pret.has_value())
        {
            throw std::runtime_error("Ce livre est prêté à une autre bibliothèque.");
        }
    }
    else if (!pret.has_value() || pret->bibliotheque != a.bibliotheque)
    {
        throw std::runtime_error("Ce livre n'est pas disponible dans la bibliothèque de l'adhérent.");
    }

    emprunte.append(codeLivre, numeroAdherent);
}

void Database::retournerAdherent(std::uint64_t codeLivre)
{
    emprunte.remove(codeLivre);
}

void Database::preterLivre(std::uint64_t codeLivre, std::uint64_t codeBiblio)
{
    if (trouverEmprunt(codeLivre).has_value())
    {
        throw std::runtime_error("Ce livre est prêté à un adhérent.");
    }

    auto d = trouverDetient(codeLivre);
    if (!d.has_value())
    {
        throw std::runtime_error("Ce livre n'est pas détenu par une bibliothèque.");
    }

    auto pret = trouverPret(codeLivre);
    if (pret.has_value())
    {
        if (pret->bibliotheque == codeBiblio)
        {
            return;
        }

        throw std::runtime_error("Ce livre est déjà prêté à une autre bibliothèque.");
    }

    if (d->bibliotheque == codeBiblio)
    {
        return;
    }

    prete.append(codeLivre, codeBiblio);
}

void Database::retournerBiblio(std::uint64_t codeBiblio)
{
    std::function getPretsFunc = [this,codeBiblio](std::uint64_t codeLivre, std::uint64_t destination)
    {
        return destination == codeBiblio && !trouverEmprunt(codeLivre).has_value();
    };
    auto prets = prete.find_all(getPretsFunc);
    for (const auto& pret : prets)
    {
        prete.remove(std::get<0>(pret));
    }
}
