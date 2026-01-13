#ifndef BIBLIOTHEQUE_H
#define BIBLIOTHEQUE_H
using namespace std;

class Bibliotheque{
    private:
        string nom;
        string adresse;
        id code;
        Array<Livre> listeLivre;
        Array<Adherent> listeAdherent;

    public:
        Bibliotheque(string nom="", string adresse="", id code=0);
        ~Bibliotheque();
        void emprunterLivre(int);
        void afficherLivres(string);
        void acheterLivre();
        void supprimerLivre();
        void rendreLivres();
        void setListeLivres(Array<Livre> listeLivre);
        void setListeAdherent(Array<Adherent> listeAdherent);
}


#endif