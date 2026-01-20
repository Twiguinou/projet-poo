#ifndef PROJET_POO_UI_MAIN_WINDOW_H
#define PROJET_POO_UI_MAIN_WINDOW_H

#include <QMainWindow>
#include <memory>
#include "Database.h" // On inclut la définition complète de la Database

class QListWidget;
class QTableWidget;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
    MainWindow();
    ~MainWindow() = default;

private slots:
    // Navigation
    void onTableSelectionnee(QListWidgetItem* item);
    
    // Actions Adhérents
    void onBtnEmprunter();
    void onBtnRendre();
    
    // Actions Bibliothèque
    void onBtnPreterInterBiblio();
    void onBtnAcheterLivre();
    void onBtnSupprimerLivre();
    
    // Filtres / Affichage
    void onBtnVoirRomans();
    void onBtnVoirBD();

private:
    // UI Elements
    QListWidget* m_listeTables;
    QTableWidget* m_tableauDonnees;

    // Le Cœur du système (Backend)
    std::unique_ptr<Database> m_db;

    // Initialise des données de test
    void chargerDonneesExemple();
    
    // Affiche une table générique
    void afficherTable(const Table& table);
    
    // Helpers
    void afficherMessageErreur(const std::exception& e);
    QString anyToString(const std::any& val) const;
    
    // Convertit un vecteur d'objets (ex: vector<Roman>) en Table pour l'affichage
    template <typename T>
    void afficherResultatRequete(const std::vector<T>& resultats, const QString& titre);
};

#endif