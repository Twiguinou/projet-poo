#include "MainWindow.h"
#include <QDockWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <fstream>              // Pour std::ifstream
#include <nlohmann/json.hpp>    // Pour manipuler le JSON

MainWindow::MainWindow() : QMainWindow(nullptr), m_db(std::make_unique<Database>())
{
    setObjectName("projet-poo");
    resize(1400, 800);
    setWindowTitle("Gestionnaire de Bibliothèque");

    // --- 1. Zone Centrale (Tableau) ---
    m_tableauDonnees = new QTableWidget(this);
    m_tableauDonnees->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setCentralWidget(m_tableauDonnees);

    // --- 2. Dock Navigation (Liste des tables brutes) ---
    auto* dockTables = new QDockWidget("Base de Données", this);
    m_listeTables = new QListWidget(dockTables);
    dockTables->setWidget(m_listeTables);
    addDockWidget(Qt::LeftDockWidgetArea, dockTables);

    // --- 3. Dock Actions (Les boutons demandés) ---
    auto* dockActions = new QDockWidget("Panneau de Contrôle", this);
    auto* widgetActions = new QWidget();
    auto* layoutActions = new QVBoxLayout(widgetActions);

    // Section Adhérent
    layoutActions->addWidget(new QLabel("<b>Gestion Adhérents</b>"));
    auto* btnEmprunter = new QPushButton("Emprunter un livre");
    auto* btnRendre = new QPushButton("Rendre un livre");
    layoutActions->addWidget(btnEmprunter);
    layoutActions->addWidget(btnRendre);

    layoutActions->addSpacing(10);

    // Section Bibliothèque
    layoutActions->addWidget(new QLabel("<b>Gestion Bibliothèque</b>"));
    auto* btnAcheter = new QPushButton("Acheter un livre (Lier)");
    auto* btnSupprimer = new QPushButton("Supprimer un livre");
    auto* btnPretBiblio = new QPushButton("Demander prêt Inter-Biblio");
    layoutActions->addWidget(btnAcheter);
    layoutActions->addWidget(btnSupprimer);
    layoutActions->addWidget(btnPretBiblio);

    layoutActions->addSpacing(10);

    // Section Filtres
    layoutActions->addWidget(new QLabel("<b>Filtres (Biblio 0)</b>"));
    auto* btnVoirRomans = new QPushButton("Voir les Romans");
    auto* btnVoirBD = new QPushButton("Voir les BDs");
    layoutActions->addWidget(btnVoirRomans);
    layoutActions->addWidget(btnVoirBD);

    layoutActions->addStretch();
    dockActions->setWidget(widgetActions);
    addDockWidget(Qt::RightDockWidgetArea, dockActions);

    // --- 4. Connexions ---
    connect(m_listeTables, &QListWidget::itemClicked, this, &MainWindow::onTableSelectionnee);
    
    // Connexion aux méthodes de Database.cpp
    connect(btnEmprunter, &QPushButton::clicked, this, &MainWindow::onBtnEmprunter);
    connect(btnRendre, &QPushButton::clicked, this, &MainWindow::onBtnRendre);
    connect(btnSupprimer, &QPushButton::clicked, this, &MainWindow::onBtnSupprimerLivre);
    connect(btnAcheter, &QPushButton::clicked, this, &MainWindow::onBtnAcheterLivre);
    connect(btnPretBiblio, &QPushButton::clicked, this, &MainWindow::onBtnPreterInterBiblio);
    
    connect(btnVoirRomans, &QPushButton::clicked, this, &MainWindow::onBtnVoirRomans);
    connect(btnVoirBD, &QPushButton::clicked, this, &MainWindow::onBtnVoirBD);

    // --- 5. Données ---
    chargerDonneesExemple();
}

// ---------------------------------------------------------
// IMPLEMENTATION DES REQUÊTES (BOUTONS)
// ---------------------------------------------------------

void MainWindow::onBtnEmprunter() {
    bool ok1, ok2;
    // On demande les IDs via des boites de dialogue simples
    int idAdherent = QInputDialog::getInt(this, "Emprunt", "Numéro Adhérent:", 0, 0, 10000, 1, &ok1);
    if(!ok1) return;
    int idLivre = QInputDialog::getInt(this, "Emprunt", "Code Livre:", 0, 0, 10000, 1, &ok2);
    if(!ok2) return;

    try {
        // APPEL DATABASE : void emprunter(numeroAdherent, codeLivre)
        m_db->emprunter(idAdherent, idLivre);
        QMessageBox::information(this, "Succès", "Livre emprunté !");
        // Rafraichir la vue table Emprunte
        afficherTable(m_db->emprunte);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

void MainWindow::onBtnRendre() {
    bool ok;
    int idLivre = QInputDialog::getInt(this, "Retour", "Code Livre à rendre:", 0, 0, 10000, 1, &ok);
    if(!ok) return;

    try {
        // APPEL DATABASE : void retournerAdherent(codeLivre)
        m_db->retournerAdherent(idLivre);
        QMessageBox::information(this, "Succès", "Livre rendu !");
        afficherTable(m_db->emprunte);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

void MainWindow::onBtnSupprimerLivre() {
    bool ok;
    int idLivre = QInputDialog::getInt(this, "Suppression", "Code Livre à pilonner:", 0, 0, 10000, 1, &ok);
    if(!ok) return;

    try {
        // APPEL DATABASE : void supprimerLivre(codeLivre)
        // Vérifie déjà s'il est emprunté ou prêté
        m_db->supprimerLivre(idLivre); 
        QMessageBox::information(this, "Succès", "Livre supprimé des stocks.");
        afficherTable(m_db->detient);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

void MainWindow::onBtnPreterInterBiblio() {
    // "Une bibliothèque demande un livre à une autre"
    bool ok1, ok2;
    int idLivre = QInputDialog::getInt(this, "Prêt Inter-Biblio", "Code Livre:", 0, 0, 10000, 1, &ok1);
    if(!ok1) return;
    int idBiblioDemandeur = QInputDialog::getInt(this, "Prêt Inter-Biblio", "Code Biblio Demandeur:", 0, 0, 10000, 1, &ok2);
    if(!ok2) return;

    try {
        // APPEL DATABASE : void preterLivre(codeLivre, codeBiblio)
        m_db->preterLivre(idLivre, idBiblioDemandeur);
        QMessageBox::information(this, "Succès", "Livre transféré/prêté.");
        afficherTable(m_db->prete);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

void MainWindow::onBtnAcheterLivre() {
    // Note : Pour simplifier l'UI, on suppose que le livre existe déjà dans la base générale
    // et qu'on veut l'ajouter au stock d'une bibliothèque (table Detient).
    bool ok1, ok2;
    int idLivre = QInputDialog::getInt(this, "Achat", "Code Livre existant:", 0, 0, 10000, 1, &ok1);
    if(!ok1) return;
    int idBiblio = QInputDialog::getInt(this, "Achat", "Pour la bibliothèque:", 0, 0, 10000, 1, &ok2);
    if(!ok2) return;

    try {
        // APPEL DATABASE : void acheterLivre(codeLivre, codeBiblio)
        m_db->acheterLivre(idLivre, idBiblio);
        QMessageBox::information(this, "Succès", "Livre acheté et ajouté au stock.");
        afficherTable(m_db->detient);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

void MainWindow::onBtnVoirRomans() {
    try {
        // APPEL DATABASE : vector<Roman> romans(codeBiblio)
        // On fixe la biblio à 0 pour l'exemple
        auto resultat = m_db->romans(0); 
        
        // On construit une table temporaire pour l'afficher
    Table tDisplay({
    { "Code", DT_UINT64 },
    { "Titre", DT_STRING },
    { "Auteur", DT_STRING },
    { "Genre", DT_STRING }
});
        for(const auto& r : resultat) {
            // Conversion Enum Genre -> String pour affichage (simplifié)
            tDisplay.append((uint64_t)r.code, r.titre, r.auteur, std::string("Genre ID: ") + std::to_string((int)r.genre));
        }
        afficherTable(tDisplay);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

void MainWindow::onBtnVoirBD() {
    try {
        // APPEL DATABASE : vector<BandeDessinee> bandeDessinees(codeBiblio)
        auto resultat = m_db->bandeDessinees(0);

        Table tDisplay({
    { "Code", DT_UINT64 },
    { "Titre", DT_STRING },
    { "Dessinateur", DT_STRING }
});
        for(const auto& bd : resultat) {
            tDisplay.append((uint64_t)bd.code, bd.titre, bd.dessinateur);
        }
        afficherTable(tDisplay);
    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

// ---------------------------------------------------------
// FONCTIONS UTILITAIRES
// ---------------------------------------------------------

void MainWindow::afficherMessageErreur(const std::exception& e) {
    QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
}

void MainWindow::onTableSelectionnee(QListWidgetItem* item) {
    if (!item) return;
    QString nom = item->text();

    // Mapping manuel car les tables sont des membres publics, pas dans une map
    if (nom == "Livres") afficherTable(m_db->livre);
    else if (nom == "Adhérents") afficherTable(m_db->adherent);
    else if (nom == "Emprunts") afficherTable(m_db->emprunte);
    else if (nom == "Bibliothèques") afficherTable(m_db->bibliotheque);
    else if (nom == "Stock (Détient)") afficherTable(m_db->detient);
    else if (nom == "Prêts Inter-Biblio") afficherTable(m_db->prete);
}

void MainWindow::chargerDonneesExemple() {
    // 1. Définition du nom du fichier
    // Le fichier doit se trouver dans le même dossier que l'exécutable (.exe)
    // ou dans le dossier de travail courant.
    std::string nomFichier = "exemple.json";
    std::ifstream fichier(nomFichier);

    if (!fichier.is_open()) {
        QMessageBox::warning(this, "Erreur de chargement", 
                             QString("Impossible d'ouvrir le fichier '%1'.\n"
                                     "Vérifiez qu'il est bien situé à côté de l'exécutable.")
                             .arg(QString::fromStdString(nomFichier)));
        return;
    }

    try {
        // 2. Parsing du fichier JSON
        nlohmann::json jsonData;
        fichier >> jsonData;

        // 3. Construction de la base de données via la méthode statique existante
        // Database::read retourne un std::unique_ptr<Database>
        m_db = Database::read(jsonData);

        // 4. Mise à jour de l'interface (Liste des tables à gauche)
        m_listeTables->clear();
        m_listeTables->addItem("Livres");
        m_listeTables->addItem("Adhérents");
        m_listeTables->addItem("Bibliothèques");
        m_listeTables->addItem("Emprunts");
        m_listeTables->addItem("Stock (Détient)");
        m_listeTables->addItem("Prêts Inter-Biblio");

        // Optionnel : Afficher un message de succès ou charger une vue par défaut
        // afficherTable(m_db->livre); 

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur de parsing", 
                              QString("Erreur lors de la lecture du JSON :\n%1")
                              .arg(e.what()));
    }
}

void MainWindow::afficherTable(const Table& table) {
    auto colonnes = table.getColumnNames();
    m_tableauDonnees->clear();
    m_tableauDonnees->setColumnCount(colonnes.size());
    m_tableauDonnees->setRowCount(table.size());

    QStringList headers;
    for(const auto& name : colonnes) headers << QString::fromStdString(name);
    m_tableauDonnees->setHorizontalHeaderLabels(headers);

    for (size_t col = 0; col < colonnes.size(); ++col) {
        const auto& dataVector = table.getColumnData(col);
        for (size_t row = 0; row < dataVector.size(); ++row) {
             QString texte = anyToString(dataVector[row]);
             auto* cell = new QTableWidgetItem(texte);
             cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
             m_tableauDonnees->setItem(row, col, cell);
        }
    }
}

QString MainWindow::anyToString(const std::any& val) const {
    if (!val.has_value()) return "NULL";
    // Conversion basique des types connus
    if (val.type() == typeid(std::string)) return QString::fromStdString(std::any_cast<std::string>(val));
    if (val.type() == typeid(std::uint64_t)) return QString::number(std::any_cast<std::uint64_t>(val));
    if (val.type() == typeid(std::uint8_t)) return QString::number(std::any_cast<std::uint8_t>(val));
    // Pour les Enums, idéalement il faudrait une fonction de conversion stringify,
    // ici on retourne un placeholder si le type n'est pas géré directement.
    return "[Donnée]";
}