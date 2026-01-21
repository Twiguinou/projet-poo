#include "MainWindow.h"

// Includes Qt
#include <QDockWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QFile>

// Includes Standard & Json
#include <fstream>
#include <nlohmann/json.hpp>

// =========================================================================
// CONSTRUCTEUR
// =========================================================================
MainWindow::MainWindow() : QMainWindow(nullptr), m_db(std::make_unique<Database>())
{
    setObjectName("projet-poo");
    resize(1400, 900);
    setWindowTitle("Gestionnaire de Bibliothèque v3.0");

    // 1. STYLE GLOBAL (CSS)
    this->setStyleSheet(R"(
        QMainWindow { background-color: #2b2b2b; color: #f0f0f0; }
        QDockWidget { color: #f0f0f0; font-weight: bold; }
        QDockWidget::title { background: #3c3f41; padding-left: 10px; padding-top: 4px; }
        QTableWidget { background-color: #2b2b2b; color: #f0f0f0; gridline-color: #3c3f41; selection-background-color: #3daee9; border: none; }
        QHeaderView::section { background-color: #3c3f41; color: #f0f0f0; padding: 5px; border: 1px solid #2b2b2b; font-weight: bold; }
        QListWidget { background-color: #323232; color: #f0f0f0; border: none; font-size: 14px; }
        QListWidget::item { padding: 12px; border-bottom: 1px solid #3c3f41; }
        QListWidget::item:selected { background-color: #3daee9; color: white; }
        QPushButton { background-color: #3c3f41; color: white; border-radius: 4px; padding: 8px 16px; border: 1px solid #555; text-align: left; }
        QPushButton:hover { background-color: #4e5254; border: 1px solid #3daee9; }
        QGroupBox { border: 1px solid #555; border-radius: 6px; margin-top: 24px; font-weight: bold; color: #3daee9; font-size: 14px; }
        QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; left: 10px; }
        QScrollArea { border: none; background-color: transparent; }
    )");

    // 2. ZONE CENTRALE (TABLEAU)
    m_tableauDonnees = new QTableWidget(this);
    m_tableauDonnees->setAlternatingRowColors(true);
    m_tableauDonnees->setShowGrid(false);
    m_tableauDonnees->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableauDonnees->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableauDonnees->verticalHeader()->setVisible(false);
    m_tableauDonnees->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setCentralWidget(m_tableauDonnees);

    // 3. DOCK GAUCHE (NAVIGATION)
    auto* dockTables = new QDockWidget("Base de Données", this);
    dockTables->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_listeTables = new QListWidget(dockTables);
    dockTables->setWidget(m_listeTables);
    addDockWidget(Qt::LeftDockWidgetArea, dockTables);

    // 4. DOCK DROITE (PANNEAU DE CONTRÔLE) - Correction de l'affichage
    auto* dockActions = new QDockWidget("Panneau de Contrôle", this);
    dockActions->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    // Le widget qui contiendra tout le layout vertical
    auto* widgetContenu = new QWidget();
    auto* layoutPrincipal = new QVBoxLayout(widgetContenu);
    layoutPrincipal->setSpacing(15);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);

    // --- GROUPE ADHÉRENTS ---
    auto* groupAdh = new QGroupBox("Gestion Adhérents");
    auto* layoutAdh = new QVBoxLayout(groupAdh);
    auto* btnEmprunter = new QPushButton("Emprunter Livre");
    auto* btnRendre = new QPushButton("Rendre Livre");
    layoutAdh->addWidget(btnEmprunter);
    layoutAdh->addWidget(btnRendre);
    layoutPrincipal->addWidget(groupAdh);

    // --- GROUPE BIBLIOTHÈQUE ---
    auto* groupBib = new QGroupBox("Gestion Bibliothèque");
    auto* layoutBib = new QVBoxLayout(groupBib);
    auto* btnAcheter = new QPushButton("Acheter / Lier au stock");
    auto* btnSupprimer = new QPushButton("Supprimer / Pilonner");
    auto* btnPretBiblio = new QPushButton("Prêt Inter-Biblio");
    layoutBib->addWidget(btnAcheter);
    layoutBib->addWidget(btnSupprimer);
    layoutBib->addWidget(btnPretBiblio);
    layoutPrincipal->addWidget(groupBib);

    // --- GROUPE FILTRES ---
    auto* groupFiltre = new QGroupBox("Filtres par Bibliothèque");
    auto* layoutFiltre = new QVBoxLayout(groupFiltre);
    
    auto* btnVoirRomans = new QPushButton("Afficher les Romans...");
    auto* btnVoirBD = new QPushButton("Afficher les BDs...");
    auto* btnVoirPoesie = new QPushButton("Afficher la Poésie...");
    auto* btnVoirTheatre = new QPushButton("Afficher le Théâtre...");
    auto* btnVoirAlbums = new QPushButton("Afficher les Albums...");

    layoutFiltre->addWidget(btnVoirRomans);
    layoutFiltre->addWidget(btnVoirBD);
    layoutFiltre->addWidget(btnVoirPoesie);
    layoutFiltre->addWidget(btnVoirTheatre);
    layoutFiltre->addWidget(btnVoirAlbums);
    layoutPrincipal->addWidget(groupFiltre);

    // Espace vide à la fin pour pousser les boutons vers le haut
    layoutPrincipal->addStretch();

    // Configuration de la ScrollArea (Indispensable pour voir le contenu)
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true); // CRUCIAL : Permet au widget de s'étendre
    scrollArea->setWidget(widgetContenu); // On met le widget rempli dans la scroll area
    
    dockActions->setWidget(scrollArea); // On met la scroll area dans le dock
    addDockWidget(Qt::RightDockWidgetArea, dockActions);

    // 5. CONNEXIONS
    connect(m_listeTables, &QListWidget::itemClicked, this, &MainWindow::onTableSelectionnee);

    // Actions
    connect(btnEmprunter, &QPushButton::clicked, this, &MainWindow::onBtnEmprunter);
    connect(btnRendre, &QPushButton::clicked, this, &MainWindow::onBtnRendre);
    connect(btnAcheter, &QPushButton::clicked, this, &MainWindow::onBtnAcheterLivre);
    connect(btnSupprimer, &QPushButton::clicked, this, &MainWindow::onBtnSupprimerLivre);
    connect(btnPretBiblio, &QPushButton::clicked, this, &MainWindow::onBtnPreterInterBiblio);

    // Filtres
    connect(btnVoirRomans, &QPushButton::clicked, this, &MainWindow::onBtnVoirRomans);
    connect(btnVoirBD, &QPushButton::clicked, this, &MainWindow::onBtnVoirBD);
    connect(btnVoirPoesie, &QPushButton::clicked, this, &MainWindow::onBtnVoirPoesie);
    connect(btnVoirTheatre, &QPushButton::clicked, this, &MainWindow::onBtnVoirTheatre);
    connect(btnVoirAlbums, &QPushButton::clicked, this, &MainWindow::onBtnVoirAlbums);

    // 6. CHARGEMENT DONNÉES
    chargerDonneesExemple();
}

// =========================================================================
// LOGIQUE : CHARGEMENT
// =========================================================================
void MainWindow::chargerDonneesExemple() {
    std::string nomFichier = "exemple.json";
    std::ifstream fichier(nomFichier);

    if (!fichier.is_open()) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir 'exemple.json'.");
        return;
    }

    try {
        nlohmann::json jsonData;
        fichier >> jsonData;
        m_db = Database::read(jsonData);

        m_listeTables->clear();
        new QListWidgetItem("Livres", m_listeTables);
        new QListWidgetItem("Adhérents", m_listeTables);
        new QListWidgetItem("Bibliothèques", m_listeTables);
        new QListWidgetItem("Emprunts", m_listeTables);
        new QListWidgetItem("Stock (Détient)", m_listeTables);
        new QListWidgetItem("Prêts Inter-Biblio", m_listeTables);

    } catch (const std::exception& e) {
        afficherMessageErreur(e);
    }
}

// =========================================================================
// LOGIQUE : AFFICHAGE
// =========================================================================
void MainWindow::onTableSelectionnee(QListWidgetItem* item) {
    if (!item) return;
    QString nom = item->text();

    if (nom.contains("Livres")) afficherTable(m_db->livre);
    else if (nom.contains("Adhérents")) afficherTable(m_db->adherent);
    else if (nom.contains("Emprunts")) afficherTable(m_db->emprunte);
    else if (nom.contains("Bibliothèques")) afficherTable(m_db->bibliotheque);
    else if (nom.contains("Stock")) afficherTable(m_db->detient);
    else if (nom.contains("Prêts")) afficherTable(m_db->prete);
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

    // --- 1. Types de base (String, Int) ---
    if (val.type() == typeid(std::string)) {
        return QString::fromStdString(std::any_cast<std::string>(val));
    }
    else if (val.type() == typeid(std::uint64_t)) {
        return QString::number(std::any_cast<std::uint64_t>(val));
    }
    else if (val.type() == typeid(std::uint8_t)) {
        return QString::number(std::any_cast<std::uint8_t>(val));
    }

    // --- 2. Gestion spécifique de Livre::Public ---
    else if (val.type() == typeid(Livre::Public)) {
        switch (std::any_cast<Livre::Public>(val)) {
            case Livre::Public::ADULTES:     return "ADULTES";
            case Livre::Public::ADOS:        return "ADOS";
            case Livre::Public::JEUNESSES:   return "JEUNESSES";
            case Livre::Public::TOUT_PUBLIC: return "TOUT PUBLIC";
            default: return "Inconnu";
        }
    }

    // --- 3. Gestion des autres Enums (Pour éviter [Donnée] ailleurs) ---
    
    // Poésie
    else if (val.type() == typeid(RecueilDePoesie::Style)) {
        switch (std::any_cast<RecueilDePoesie::Style>(val)) {
            case RecueilDePoesie::Style::VERS: return "VERS";
            case RecueilDePoesie::Style::PROSE: return "PROSE";
            case RecueilDePoesie::Style::VERS_ET_PROSE: return "VERS & PROSE";
            default: return "?";
        }
    }
    // Albums
    else if (val.type() == typeid(Album::Type)) {
        switch (std::any_cast<Album::Type>(val)) {
            case Album::Type::PHOTOS: return "PHOTOS";
            case Album::Type::DESSINS: return "DESSINS";
            case Album::Type::PHOTOS_ET_DESSINS: return "PHOTOS & DESSINS";
            default: return "?";
        }
    }
    // Romans (La liste est très longue, on affiche l'ID ou on gère les cas principaux)
    else if (val.type() == typeid(Roman::Genre)) {
        // Astuce : on convertit l'enum en entier pour l'affichage rapide
        // Si tu veux les textes complets, il faut faire un grand switch case
        return "Genre n°" + QString::number((int)std::any_cast<Roman::Genre>(val));
    }

    return "[Donnée]";
}

void MainWindow::afficherMessageErreur(const std::exception& e) {
    QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
}

// =========================================================================
// LOGIQUE : ACTIONS (BOUTONS)
// =========================================================================

void MainWindow::onBtnEmprunter() {
    bool ok1, ok2;
    int idAdh = QInputDialog::getInt(this, "Emprunt", "Numéro Adhérent:", 0, 0, 99999, 1, &ok1);
    if(!ok1) return;
    int idLiv = QInputDialog::getInt(this, "Emprunt", "Code Livre:", 0, 0, 99999, 1, &ok2);
    if(!ok2) return;

    try {
        m_db->emprunter(idAdh, idLiv);
        QMessageBox::information(this, "Succès", "Livre emprunté !");
        afficherTable(m_db->emprunte);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnRendre() {
    bool ok;
    int idLiv = QInputDialog::getInt(this, "Retour", "Code Livre:", 0, 0, 99999, 1, &ok);
    if(!ok) return;

    try {
        m_db->retournerAdherent(idLiv);
        QMessageBox::information(this, "Succès", "Livre rendu !");
        afficherTable(m_db->emprunte);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnAcheterLivre() {
    bool ok1, ok2;
    int idLiv = QInputDialog::getInt(this, "Achat", "Code Livre existant:", 0, 0, 99999, 1, &ok1);
    if(!ok1) return;
    int idBib = QInputDialog::getInt(this, "Achat", "Pour Biblio (Code):", 0, 0, 99999, 1, &ok2);
    if(!ok2) return;

    try {
        m_db->acheterLivre(idLiv, idBib);
        QMessageBox::information(this, "Succès", "Stock mis à jour.");
        afficherTable(m_db->detient);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnSupprimerLivre() {
    bool ok;
    int idLiv = QInputDialog::getInt(this, "Suppression", "Code Livre:", 0, 0, 99999, 1, &ok);
    if(!ok) return;

    try {
        m_db->supprimerLivre(idLiv);
        QMessageBox::information(this, "Succès", "Livre supprimé.");
        afficherTable(m_db->detient);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnPreterInterBiblio() {
    bool ok1, ok2;
    int idLiv = QInputDialog::getInt(this, "Prêt", "Code Livre:", 0, 0, 99999, 1, &ok1);
    if(!ok1) return;
    int idBib = QInputDialog::getInt(this, "Prêt", "Biblio Demandeur:", 0, 0, 99999, 1, &ok2);
    if(!ok2) return;

    try {
        m_db->preterLivre(idLiv, idBib);
        QMessageBox::information(this, "Succès", "Prêt inter-biblio validé.");
        afficherTable(m_db->prete);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

// =========================================================================
// LOGIQUE : FILTRES
// =========================================================================

void MainWindow::onBtnVoirRomans() {
    bool ok;
    int id = QInputDialog::getInt(this, "Romans", "ID Bibliothèque :", 0, 0, 9999, 1, &ok);
    if (!ok) return;
    try {
        auto res = m_db->romans(id);
        Table t({{"Code", DT_UINT64}, {"Titre", DT_STRING}, {"Auteur", DT_STRING}, {"Genre", DT_STRING}});
        for(const auto& r : res) t.append((uint64_t)r.code, r.titre, r.auteur, std::to_string((int)r.genre));
        afficherTable(t);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnVoirBD() {
    bool ok;
    int id = QInputDialog::getInt(this, "BDs", "ID Bibliothèque :", 0, 0, 9999, 1, &ok);
    if (!ok) return;
    try {
        auto res = m_db->bandeDessinees(id);
        Table t({{"Code", DT_UINT64}, {"Titre", DT_STRING}, {"Dessinateur", DT_STRING}});
        for(const auto& b : res) t.append((uint64_t)b.code, b.titre, b.dessinateur);
        afficherTable(t);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnVoirPoesie() {
    bool ok;
    int id = QInputDialog::getInt(this, "Poésie", "ID Bibliothèque :", 0, 0, 9999, 1, &ok);
    if (!ok) return;
    try {
        auto res = m_db->recueilsDePoesie(id);
        Table t({{"Code", DT_UINT64}, {"Titre", DT_STRING}, {"Style", DT_STRING}});
        for(const auto& p : res) t.append((uint64_t)p.code, p.titre, std::to_string((int)p.style));
        afficherTable(t);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnVoirTheatre() {
    bool ok;
    int id = QInputDialog::getInt(this, "Théâtre", "ID Bibliothèque :", 0, 0, 9999, 1, &ok);
    if (!ok) return;
    try {
        auto res = m_db->piecesDeTheatre(id);
        Table t({{"Code", DT_UINT64}, {"Titre", DT_STRING}, {"Siècle", DT_UINT8}});
        for(const auto& p : res) t.append((uint64_t)p.code, p.titre, p.siecle);
        afficherTable(t);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}

void MainWindow::onBtnVoirAlbums() {
    bool ok;
    int id = QInputDialog::getInt(this, "Albums", "ID Bibliothèque :", 0, 0, 9999, 1, &ok);
    if (!ok) return;
    try {
        auto res = m_db->albums(id);
        Table t({{"Code", DT_UINT64}, {"Titre", DT_STRING}, {"Type", DT_STRING}});
        for(const auto& a : res) t.append((uint64_t)a.code, a.titre, std::to_string((int)a.type));
        afficherTable(t);
    } catch (const std::exception& e) { afficherMessageErreur(e); }
}