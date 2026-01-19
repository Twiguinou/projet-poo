#include <iostream>

#include "Database.h"

int main()
{
    Database db;

    db.ajouterBibliotheque({
        0,
        "François Mitterand",
        "Sevran"
    });
    db.ajouterBibliotheque({
        1,
        "François Mitron",
        "Annecy"
    });

    db.ajouterAdherent({
        0,
        "Lacisaille",
        "Karim",
        "42 rue de la zonz",
        0,
        1
    });

    db.ajouterLivre(Roman {
        {
            3,
           "Pierre Palmade",
           "Le droit de vote des femmes ou la décadence d'une nation",
           "Fayard",
           "1-2345-6789-1",
           Livre::Public::ADULTES
        },
        Roman::Genre::ROMAN_NOIR
    });

    db.acheterLivre(3, 0);
    db.emprunter(0, 3);

    auto romans = db.romans(0);
    for (const auto& roman : romans)
    {
        std::cout << roman.titre << std::endl;
    }

    return 0;
}
