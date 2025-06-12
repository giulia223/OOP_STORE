#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <limits>
#include <memory>
#include "sqlite3.h"
#include<fstream>
#include<string>
#include<cstring>
#include <cstdlib>
#include <ctime>
#include <map>
#include <functional>

//FUNCTII SQL
sqlite3* db;
int executeSQL(sqlite3* db, const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Eroare SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return rc;
    }
    return SQLITE_OK;
}

bool conecteazaBazaDeDate(const std::string& numeDB) {
    if (sqlite3_open(numeDB.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Eroare la deschiderea bazei de date: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    std::cout << "Conectat la baza de date!\n";
    return true;
}

void inchideBazaDeDate() {
    sqlite3_close(db);
}
std::string escapeString(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\'') {
            result += "''";
        } else {
            result += c;
        }
    }
    return result;
}
void adaugaProdusSQL(const std::string& nume, double pret, int stoc) {
    std::string escapedNume = escapeString(nume); // Escape string-ul nume
    std::string sql = "INSERT INTO Produs (nume, pret, stoc) VALUES ('" +
                        escapedNume + "', " + std::to_string(pret) + ", " + std::to_string(stoc) + ");";

    char* errorMsg;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        std::cerr << "Eroare la inserare: " << errorMsg << "\n";
        sqlite3_free(errorMsg);
    } else {
        std::cout << "Produs adaugat cu succes!\n";
    }
}

int afiseazaProduseSQL() {
    std::string sql = "SELECT id, nume, pret, stoc FROM Produs;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Eroare la pregatirea interogarii: " << sqlite3_errmsg(db) << "\n";
        return 0;
    }

    std::cout << "ID | Nume | Pret | Stoc\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << sqlite3_column_int(stmt, 0) << " | "
                  << sqlite3_column_text(stmt, 1) << " | "
                  << sqlite3_column_double(stmt, 2) << " | "
                  << sqlite3_column_int(stmt, 3) << "\n";
    }
    sqlite3_finalize(stmt);
    return 1;
}


void reduceStocSQL(const std::string& nume_produs, int cantitate=1) {
    const std::string querySelect = "SELECT id, stoc FROM Produs WHERE nume = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, querySelect.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Eroare la pregatirea interogării de selectare: " << sqlite3_errmsg(db) << "\n";
        return;
    }
    sqlite3_bind_text(stmt, 1, nume_produs.c_str(), -1, SQLITE_STATIC);

    // Verifică dacă produsul există în baza de date
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id_produs = sqlite3_column_int(stmt, 0);
        int stoc_curent = sqlite3_column_int(stmt, 1);

        if (stoc_curent >= cantitate) {
            const std::string queryUpdate = "UPDATE Produs SET stoc = stoc - ? WHERE id = ?";
            sqlite3_stmt* stmtUpdate;
            if (sqlite3_prepare_v2(db, queryUpdate.c_str(), -1, &stmtUpdate, nullptr) != SQLITE_OK) {
                std::cerr << "Eroare la pregătirea interogarii de actualizare: " << sqlite3_errmsg(db) << "\n";
                sqlite3_finalize(stmt);
                return;
            }
            sqlite3_bind_int(stmtUpdate, 1, cantitate);
            sqlite3_bind_int(stmtUpdate, 2, id_produs);

            if (sqlite3_step(stmtUpdate) != SQLITE_DONE) {
                std::cerr << "Eroare la actualizarea stocului: " << sqlite3_errmsg(db) << "\n";
            } else {
                std::cout << "Stocul produsului '" << nume_produs << "' a fost actualizat cu succes.\n";
            }
            sqlite3_finalize(stmtUpdate);
        } else {
            std::cerr << "Stoc insuficient pentru produsul '" << nume_produs << "'.\n";
        }
    } else {
        std::cerr << "Produsul '" << nume_produs << "' nu a fost gasit în baza de date.\n";
    }
    sqlite3_finalize(stmt);
}

void adaugaClientSQL(const std::string& nume, int id) {
    std::string sql = "INSERT INTO Client (id, nume) VALUES (" + std::to_string(id) + ", '" + nume + "');";
    if (db == nullptr) {
        std::cerr << "Conexiunea la baza de date nu este valida" << std::endl;
        return;
    }
    char* errorMsg;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        std::cerr << "Eroare la inserare: " << errorMsg << "\n";
        sqlite3_free(errorMsg);
    } else {
        std::cout << "Client adaugat cu succes!\n";
    }
}

void stergeUtilizatorSQL(int id) {
    std::string query = "DELETE FROM clienti WHERE id_client = ?";
}


void adaugaComandaSQL(int id, int id_client, double total) {
    std::string sql = "INSERT INTO Comanda (id, id_client, total) VALUES (" + std::to_string(id) + ", " + std::to_string(id_client) + ", " + std::to_string(total) + ");";
    char* errorMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Eroare la inserare comanda: " << sqlite3_errmsg(db) << "\n";
        sqlite3_free(errorMsg);
    } else {
        std::cout << "Comanda adaugata cu succes!\n";
    }
}

void addProdComandaSQL(int id_comanda, const std::string& nume_produs) {
    std::string sql = "INSERT INTO Comanda_Produse (id_comanda, nume) VALUES (" +
                    std::to_string(id_comanda) + ", '" + nume_produs + "');";

    char* errorMsg;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        std::cerr << "Eroare la inserarea produsului în comanda: " << errorMsg << "\n";
        sqlite3_free(errorMsg);
    } else {
        std::cout << "Produsul " << nume_produs << " a fost adaugat la comanda " << id_comanda << std::endl;
    }
}

void stergeProdusSQL(int id_produs) {
    std::string sql = "DELETE FROM Produs WHERE id = " + std::to_string(id_produs) + ";";
    char* errorMsg;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        std::cerr << "Eroare la ștergere: " << errorMsg << "\n";
        sqlite3_free(errorMsg);
    } else {
        std::cout << "Produs șters!\n";
    }
}


//CLASE
class Produs {
protected:
    std::string nume;
    double pret;
    int stoc;
    std::vector<std::function<void(const std::string&)>> observers;
public:
    Produs(): nume(""), pret(0), stoc(0){}
    Produs(const std::string& nume, double pret, int stoc) {
        this->nume=nume;
        this->pret=pret;
        this->stoc=stoc;
    }
    [[nodiscard]] std::string getNume()const{return nume;}
    [[nodiscard]] double getPret() const{return pret;}
    [[nodiscard]] int getStoc()const{return stoc;}
    void setNume(const std::string& n){nume=n;}
    void setPret(float p){pret=p;}
    void setStoc(int s){stoc=s;}
    void adaugaObserver(std::function<void(const std::string&)> obs) {
        observers.push_back(obs);
    }
    void reduceStoc(int cantitate) {
        if (stoc>=cantitate) {
            stoc=stoc-cantitate;
            if (stoc < 5) {  // prag low stock
                for (auto& obs : observers) {
                    obs("Produsul " + nume + " are stoc scazut: " + std::to_string(stoc));
                }
            }
        }
    }
    void afiseazaProdus() {
        std::cout<<"produs:"<<nume<<" pret: "<<pret<<std::endl;
    }
    friend std::ostream& operator<<(std::ostream& os, const Produs& produs) {
        os<<produs.nume<<" pret: "<<produs.pret<<std::endl;
        os<<"stoc: "<<produs.stoc<<std::endl;
        return os;
    }

    Produs& operator+=(const Produs& other) {
        if (this->nume == other.nume && this->pret == other.pret) {
            this->stoc += other.stoc;
        return *this;
    }}
};

std::shared_ptr<Produs> gasesteProdusSQL(const std::string& nume_produs) {
    std::string sql = "SELECT * FROM Produs WHERE nume = '" + nume_produs + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Eroare la pregătirea interogării: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double pret = sqlite3_column_double(stmt, 2);
        int stoc = sqlite3_column_int(stmt, 3);

        auto produs = std::make_shared<Produs>(nume, pret, stoc);
        sqlite3_finalize(stmt);
        return produs;
    } else {
        std::cerr << "Produsul nu a fost gasit în baza de date." << std::endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

class Comanda {
    std::vector<std::shared_ptr<Produs>> produse;
    int id_client;
    int id_comanda;
public:
    explicit Comanda( int id_client) {
        this->id_client=id_client;
        srand(time(nullptr));
        this->id_comanda=rand()%101;}

    Comanda( const Comanda& other, bool pastreaza_id=false) {
        if (pastreaza_id){
            this->id_comanda=other.id_comanda;}
        else{
            this->id_comanda=rand()%101;}
        this->id_client=other.id_client;
        for (const auto& produs : other.produse) {
            produse.push_back(std::make_shared<Produs>(*produs)); }}

    //operator =
    Comanda& operator=(const Comanda& other) {
        if (this!=&other) {
            this->id_client=other.id_client;
            this->id_comanda=other.id_comanda;
            produse.clear();
            for (const auto& produs : other.produse) {
                produse.push_back(std::make_shared<Produs>(*produs));}}
        return *this;}

    Comanda(Comanda&& other) noexcept
        : produse(std::move(other.produse)),
          id_client(other.id_client),
          id_comanda(other.id_comanda) {}

    Comanda& operator=(Comanda&& other) noexcept {
        if (this != &other) {
            produse = std::move(other.produse);
            id_client = other.id_client;
            id_comanda = other.id_comanda;}
        return *this;}

    //operator <<
    friend std::ostream& operator<<(std::ostream& os, const Comanda& comanda) {
        os<<"comanda id:"<<comanda.id_comanda<<std::endl;
        os << "Client ID: " << comanda.id_client << "\n";
        os << "Produse in comanda:\n";
        for (const auto& produs : comanda.produse) {
            os << *produs << "\n";}
        return os;}

    void adaugaProdus(const std::shared_ptr<Produs>&produs) {
        produse.push_back(produs);
        produs->reduceStoc(1);}

    [[nodiscard]] double getTotal() const {
        double total=0;
        for (const auto&p: produse) total+=p->getPret();
        return total;}

    [[nodiscard]] int getIdComanda()const{return id_comanda;}

    [[nodiscard]] int getIdClient()const{return id_client;}

    void afiseazaProduse() const {
        std::cout << "Produse in comanda " << id_comanda << ":\n";
        for (const auto& produs : produse) {
            produs->afiseazaProdus();}}
};



class User{
protected:
        int id_client;
        std::string nume;
public:
        User( const::std::string& nume){
            this->nume=std::move(nume);
            srand(time(nullptr));
            this->id_client=rand()%101;  };

        int getId()const{return id_client;}

        std::string getNume()const{return nume;}

        virtual void afiseazaUser() const{
              std::cout<<"id-client: "<<id_client<<" nume: "<<nume<<std::endl;}

        virtual void citire(std::istream& is) {
            std::cout << "Introdu nume:\n";
            is >> nume;
            srand(time(nullptr));
            id_client = rand() % 101;}

        virtual int addPct(int) {
            throw std::runtime_error("Functia addPct nu este disponibila pentru acest tip de user.");}

        virtual ~User()=default;

        friend std::istream& operator>>(std::istream& is, User& user);
};


std::istream& operator>>(std::istream& is, User& user) {
    user.citire(is);
    return is;}


class Furnizor: virtual public User{
protected:
    std::vector<std::shared_ptr<Produs>>produseFurnizate;

public:
    Furnizor(const std::string& nume):User(nume){};

    void adaugaProdus(const std::shared_ptr<Produs>& produs) {
        produseFurnizate.push_back(produs);}

    const std::vector<std::shared_ptr<Produs>>& getProduseFurnizate() const {
        return produseFurnizate;}

    void afiseazaUser() const override {
        std::cout << "Furnizor: " << nume << " (ID: " << id_client << ")\n";
        for (const auto& produs : produseFurnizate) {
            produs->afiseazaProdus();}}
};


class Client:virtual public User{
        std::vector<std::shared_ptr<Produs>> produse;
protected:
        int puncte;
public:
        Client(): User(""){puncte=0;}

        Client(const std::string& nume):User(nume), puncte(0){};
        int addPct(int valoare)override{
            puncte += std::abs(valoare % 20);
            return puncte;}
        virtual void afiseazaOferte() const {
            std::cout<<"-15%reducere in limita stocului disponibil la produse de ingrijire a tenului"<<std::endl;}
        void citire(std::istream& is) override {
            User::citire(is);      }

        ~Client() override=default;
 };




class ClientPremium: public Client{
      int discount;
public:
     ClientPremium(const std::string& nume):User(nume), Client(nume){};

     void checkDiscount(){
          if(puncte>=100) {discount=45; std::cout<<"Ai discount de 45%"<<std::endl;}
          else if(puncte>=50) {discount=30; std::cout<<"Ai discount de 30%"<<std::endl;}}

     virtual void AfiseazaOferte() const {
         std::cout<<"-35%reducere in limita stocului disponibil la produse de ingrijire a tenului"<<std::endl;}

     virtual ~ClientPremium()=default;
};


class Review{
public:
    virtual void scrieReview(const std::string& text, int rating)=0;
    virtual ~Review()=default;
};


class ClientRev: public Client, public Review{
public:
    ClientRev(const std::string& nume):User(nume),Client(nume){};

    void scrieReview(const std::string& text, int rating) override {
        std::cout << "Review de la " << nume << ": " << text << " (" << rating << "/5)\n";
        std::ofstream fout("reviewuri.txt", std::ios::app);
        if (!fout) {
            std::cerr << "Eroare la deschiderea fisierului reviewuri.txt\n";
            return;
        }
        fout << "Review de la " << nume << ": " << text << " (" << rating << "/5)\n";
        };

    virtual void AfiseazaOferte() const {
        std::cout<<"-25%reducere in primele 24h de la review"<<std::endl;}

    virtual ~ClientRev()=default;
};


class Magazin {
         std::vector<std::shared_ptr<Produs>> inventar;
         std::vector<Comanda> comenzi;
         std::vector<std::shared_ptr<User>> utilizatori;
         std::vector<std::shared_ptr<Furnizor>>furnizori;
        Magazin()=default;
        Magazin(const Magazin&) = delete;
        Magazin& operator=(const Magazin&) = delete;
        Magazin(Magazin&&) = delete;
        Magazin& operator=(Magazin&&) = delete;
public:
        static Magazin& getInstance() {
            static Magazin instance;
            return instance;
        }
         void adaugaUtilizator(const std::shared_ptr<User>& user) {
            utilizatori.push_back(user);}

         const std::vector<std::shared_ptr<User>>& getUtilizatori() {
                return utilizatori;}

         const std::shared_ptr<User> getUtilizator(const std::string nume) {
            for (const auto& u : utilizatori) {
                if (u->getNume()==nume) {
                    return u;}}
            return nullptr;}

         void afiseazaUtilizatori() {
                for (const auto& u : utilizatori) {
                    u->afiseazaUser();
                    if (auto cp = std::dynamic_pointer_cast<ClientPremium>( u)) {
                        cp->checkDiscount();}}}

         void stergeUtilizator(int idClient) {
            utilizatori.erase(std::remove_if(utilizatori.begin(), utilizatori.end(),
                [idClient](const std::shared_ptr<User>& u) {
                    return u->getId() == idClient;
                }), utilizatori.end());}

         void adaugaProdus(const std::shared_ptr<Produs>&produs) {
            if (produs->getPret() <= 0) {
                throw std::invalid_argument("Pretul produsului trebuie sa fie mai mare decat 0.");
                }inventar.push_back(produs);}

         std::shared_ptr<Produs> gasesteProdus(const std::string& nume) {
            for ( const auto&p: inventar) {
                if (p->getNume()==nume) return p;}
            return nullptr;}

         const std::vector<std::shared_ptr<Produs>>& getInventar() {
            return inventar;}

         void sorteazaProduseDupaPret() {
            std::sort(inventar.begin(), inventar.end(), [](const std::shared_ptr<Produs>& a, const std::shared_ptr<Produs>& b) {
                return a->getPret() < b->getPret();});
            std::cout << "Produse sortate dupa pret:\n";
            for (const auto& produs : inventar) {
                produs->afiseazaProdus(); }}

         void adaugaComanda(const Comanda& comanda) {
            Comanda comandaCopie(comanda, true);
            comenzi.push_back(std::move(comandaCopie));}

         Comanda* gasesteComanda(int id_comanda) {
            for (auto& comanda : comenzi) {
                if (comanda.getIdComanda() == id_comanda) {
                    return &comanda;}}
            return nullptr;}

         void stergeComanda(int id_comanda) {
            for (auto it = comenzi.begin(); it != comenzi.end(); ++it) {
                if (it->getIdComanda() == id_comanda) {
                    std::cout << "Se sterge comanda cu ID-ul " << id_comanda << "\n";
                    comenzi.erase(it);
                    return;}}
            std::cout << "Comanda cu ID-ul " << id_comanda << " nu a fost gasita.\n";}

         void afisareComenzi() {
            for (const auto&p:comenzi) {
                std::cout<<"pentru clientul:"<<p.getIdClient()<<" ";
                p.afiseazaProduse();
                std::cout<<std::endl;}}

         void golesteComenzi() {
            comenzi.clear();}

         const std::vector<Comanda>& getComenzi() {
            return comenzi;}

         void afiseazaInventar() {
            for (const auto& produs : inventar) {
                std::cout << "Produs in inventar: " << produs->getNume() << "\n";}}

         void golesteInventar() {
            inventar.clear();}

         void adaugaFurnizor(const std::shared_ptr<Furnizor>& furnizor) {
            furnizori.push_back(furnizor);
        }

         std::vector<std::shared_ptr<Furnizor>>& getFurnizori() {
            return furnizori;
        }

         void adaugaProdusFurnizor(){
            std::cout << "Selecteaza furnizorul:\n";
            for (size_t i = 0; i < furnizori.size(); ++i) {
                std::cout << i + 1 << ". " << furnizori[i]->getNume() << '\n';}

            int opt;
            std::cin >> opt;

            if (opt < 1 || opt > furnizori.size()) {
                std::cout << "Optiune invalida.\n";
                return;}

            std::string numeProdus;
            double pret;
            int stoc;

            std::cout << "Introdu nume produs: ";
            std::cin >> numeProdus;
            std::cout << "Introdu pret: ";
            std::cin >> pret;
            std::cout << "Introdu stoc: ";
            std::cin >> stoc;

            auto produs = std::make_shared<Produs>(numeProdus, pret, stoc);
            furnizori[opt - 1]->adaugaProdus(produs);
            std::cout << "Produs adaugat pentru " << furnizori[opt - 1]->getNume() << "!\n";}

    ~Magazin() {
            std::cout << "Magazinul a fost distrus.\n";}
};




class Colaborator: public Client, public Furnizor{
protected:
    std::string nume_firma;
    int codColaborator;
public:
    Colaborator(std::string nume, int cod):User(nume), Client(nume), Furnizor(nume), codColaborator(cod), nume_firma(nume){};

    void afiseazaInfo() {
        std::cout << "Colaborator: " << nume_firma << " Cod: " << codColaborator << "\n";
        std::cout << "Produse furnizate:\n";
        for (auto& p : produseFurnizate) p->afiseazaProdus();
        std::cout << "Comenzi efectuate:\n";
        for (auto& c : Magazin::getInstance().getComenzi()) {
            if (c.getIdClient() == id_client) {
                c.afiseazaProduse();}}}
};


class Factura {
        int id_comanda;
        int nr_factura;
        double valoare;
public:
        Factura(int id_comanda, Magazin& magazin) {
            srand(time(nullptr));
            this->nr_factura=rand()%101;
            Comanda* comanda = Magazin::getInstance().gasesteComanda(id_comanda);
            if (comanda != nullptr) {
                valoare = comanda->getTotal();
            } else {
                std::cout << "Comanda nu a fost gasita!\n";
                valoare = 0;}}

        void afisareFactura() const {
            std::cout<<"factura #"<<nr_factura<<" Valoare="<<valoare<<std::endl;}
};

class ProdusBuilder {
private:
    std::shared_ptr<Produs> produs;
public:
    ProdusBuilder() { produs = std::make_shared<Produs>(); }

    ProdusBuilder& cuNume(const std::string& nume) {
        produs->setNume(nume);
        return *this;
    }

    ProdusBuilder& cuPret(float pret) {
        produs->setPret(pret);
        return *this;
    }
    ProdusBuilder& cuStoc(int stoc) {
        produs->setStoc(stoc);
        return *this;
    }


    std::shared_ptr<Produs> build() {
        return produs;
    }
};



class SistemExtern {
public:
    virtual void cumparaProdus(const std::string& nume, double pret, int stoc) = 0;
    virtual ~SistemExtern() = default;
};


class FurnizorAdapter : public SistemExtern {
private:
    std::shared_ptr<Furnizor> furnizor;

public:
    explicit FurnizorAdapter(std::shared_ptr<Furnizor> f) : furnizor(std::move(f)) {}

    void cumparaProdus(const std::string& nume, double pret, int stoc) override {
        auto produs = std::make_shared<Produs>(nume, pret, stoc);
        furnizor->adaugaProdus(produs);
        std::cout << "Produs adaugat de adapter pentru furnizorul " << furnizor->getNume() << ": ";
        produs->afiseazaProdus();
    }
};



void adaugaUtilizatorSQL(const std::shared_ptr<User>& user) {
    std::string tip;
    if (dynamic_cast<ClientPremium*>(user.get())) {
        tip = "ClientPremium";
    } else if (dynamic_cast<ClientRev*>(user.get())) {
        tip = "ClientReview";
    } else if (dynamic_cast<Client*>(user.get())) {
        tip = "Client";
    } else {
        tip = "User";
    }

    std::string sql = "INSERT INTO Utilizator (id, nume, tip) VALUES (" +
                      std::to_string(user->getId()) + ", '" +
                      user->getNume() + "', '" + tip + "');";

    if (db == nullptr) {
        std::cerr << "Conexiunea la baza de date nu este valida" << std::endl;
        return;
    }

    char* errorMsg;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        std::cerr << "Eroare la inserare: " << errorMsg << "\n";
        sqlite3_free(errorMsg);
    } else {
        std::cout << "Adaugat cu succes!\n";
    }
}

class Helper {
public:
    template <typename T, typename Functie>
    static void afiseazaDaca(const std::vector<std::shared_ptr<T>>& lista, Functie conditie) {
        for (const auto& elem : lista) {
            if (conditie(*elem)) {
                std::cout << *elem << "\n";
            }
        }
    }

    template <typename T>
    static std::shared_ptr<T> produsMaxPret(const std::vector<std::shared_ptr<T>>& produse) {
        if (produse.empty()) return nullptr;
        auto maxProdus = produse[0];
        for (const auto& p : produse) {
            if (p->getPret() > maxProdus->getPret()) {
                maxProdus = p;
            }
        }
        return maxProdus;
    }

};



    int main() {
        if (!conecteazaBazaDeDate("C:/Users/giuli/DataGripProjects/MagazinOnline2/identifier.db")) return 1;
        auto& magazin = Magazin::getInstance();
        //patt. structural
        auto furnizor = std::make_shared<Furnizor>("MAC");
        std::unique_ptr<SistemExtern> adapter = std::make_unique<FurnizorAdapter>(furnizor);
        adapter->cumparaProdus("Ruj", 69.99, 20);
        adapter->cumparaProdus("Fond de ten", 89.99, 10);
        furnizor->afiseazaUser();
        magazin.adaugaFurnizor(furnizor);

        auto furnizor1 = std::make_shared<Furnizor>("Sephora");
        magazin.adaugaFurnizor(furnizor1);
        // magazin.adaugaProdusFurnizor();
        adaugaProdusSQL("Ruj", 99.99, 12);
        adaugaProdusSQL("Rimel", 1500, 20);

        auto produsNou = ProdusBuilder()
                        .cuNume("Ruj")
                        .cuStoc(12)
                        .cuPret(99.99)
                        .build();
        magazin.adaugaProdus(produsNou);
        auto produsNou2 = ProdusBuilder()
                           .cuNume("Rimel")
                           .cuStoc(20)
                           .cuPret(1500)
                           .build();
        magazin.adaugaProdus(produsNou2);

        Produs produs("Masca", 99.99, 10);
        produs.adaugaObserver([](const std::string& mesaj) {
      std::cout << "Observer 1: " << mesaj << std::endl;
  });
        produs.reduceStoc(6);
        
          //creare user
        auto user1 = std::make_shared<Client>("Ana");
        magazin.adaugaUtilizator(user1);
        adaugaUtilizatorSQL(user1);
        user1->getId();

        //creare comanda pt Ana
        Comanda comanda(user1->getId());
        magazin.adaugaComanda(comanda);
        adaugaComandaSQL(comanda.getIdComanda(), user1->getId(), comanda.getTotal());
        std::cout<<"id comanda:"<<comanda.getIdComanda()<<std::endl;
        std::shared_ptr<Produs> produs1 = std::make_shared<Produs>("Primer", 3500, 10);
        comanda.adaugaProdus(produs1);

        //Constructor copiere
        // Comanda c2=comanda;
        // magazin.adaugaComanda(c2);
        // adaugaComandaSQL(c2.getIdComanda(), user1->getId(), comanda.getTotal());

        //operatorul << pentru comanda
        // std::cout<<"constructor copiere:"<<std::endl<<c2;


       // operator >>
         // std::shared_ptr<User> u = std::make_shared<Client>();
         // std::cin >> *u;
         // magazin.adaugaUtilizator(u);
         // adaugaUtilizatorSQL(u);
        //  operator +=
          // Produs p1("blush", 200.50, 2);
          // Produs p2("blush", 200.50, 3);
          // p1+= p2;
          // std::cout<<p1<<std::endl;

        auto maxProdus = Helper::produsMaxPret(Magazin::getInstance().getInventar());
        if (maxProdus) {
            std::cout << "Produsul cu cel mai mare pret: " << maxProdus->getNume() << " - " << maxProdus->getPret() << std::endl;
        }



    bool running = true;
    while (running) {
        int id_comanda, id_client, optiune;
        std::string nume;
        std::cout<<"fiecare optiune trebuie sa fie pe o linie separata!"<<std::endl;
        std::cout<<"Comenzi meniu:"<<std::endl;
        std::cout<<"-1->Iesire meniu"<<std::endl;
        std::cout<<"0->Inregistrare Client Nou"<<std::endl;
        std::cout<<"1->Adauga produse in comanda existenta"<<std::endl;
        std::cout<<"2->Creare comanda noua"<<std::endl;
        std::cout<<"3->Afisare comanda"<<std::endl;
        std::cout<<"4->Afisare factura"<<std::endl;
        std::cout<<"5->Elimina o comanda"<<std::endl;
        std::cout<<"6->Afisare produse disponibile"<<std::endl;
        std::cout<<"7->Afisare comenzi preluate"<<std::endl;
        std::cout<<"8->Adauga produse noi in inventarul furnizorului"<<std::endl;
        std::cout<<"9->Adauga review"<<std::endl;
        std::cout<<"10->Sorteaza produsele"<<std::endl;
        std::cout<<"11->Adaugare furnizor"<<std::endl;
        std::cout<<"12->Adauga produse pt furnizor "<<std::endl;
        std::cout<<"13->Selecteaza produse de la furnizor "<<std::endl;
        std::cout<<"14->Adauga colaborator"<<std::endl;
        std::cout<<"15->afiseaza produse dupa conditie"<<std::endl;

        std::cin >> optiune;
            switch(optiune){
             case -1: {running=false; break;}
             case 0: {//creare obiect client
                     std::cout<<"Nume:"<<std::endl;
                     std::cin>>nume;
                     auto user=std::make_shared<Client>(nume);
                     std::cout<<"Id ul tau este:"<<user->getId()<<std::endl;
                     Magazin::getInstance().adaugaUtilizator(user);
                     adaugaUtilizatorSQL(user);
                     user->afiseazaOferte();
                break;}

             case 1: {//adauga produs in comanda
                std::cout<<"nume:"<<std::endl;
                std::cin>>nume;
                std::cout << "Introdu id-ul comenzii in care vrei sa adaugi produsul: ";
                std::cin >> id_comanda;
                Comanda* comanda = Magazin::getInstance().gasesteComanda(id_comanda);
                if (comanda != nullptr) {
                    std::string nume_produs;
                    std::cout << "Introdu numele produsului de adaugat: ";
                    std::cin >> nume_produs;
                    std::shared_ptr<Produs> produs = magazin.gasesteProdus(nume_produs);

                    if (produs != nullptr) {
                        try {
                            comanda->adaugaProdus(produs);}
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Eroare la adaugarea produsului: " << e.what() << std::endl;}

                        addProdComandaSQL(id_comanda, nume_produs);
                        std::cout << "Produsul a fost adaugat la comanda " << id_comanda << std::endl;
                        comanda->afiseazaProduse();
                        auto client=Magazin::getInstance().getUtilizator(nume);
                        try {
                            client->addPct(produs->getPret());}
                        catch (const std::runtime_error& e) {
                            std::cout << "Eroare: " << e.what() << std::endl;}}
                    else {
                        std::cout << "Produsul nu a fost găsit în inventar" << std::endl;}}
                else {
                    std::cout << "Comanda cu id-ul " << id_comanda << " nu a fost gasita" << std::endl;}

             if (comanda->getTotal()>399) {
                    std::cout<<"acum poti fi client premium"<<std::endl;
                    auto user = Magazin::getInstance().getUtilizator(nume);
                    int idClient = user->getId();
                    auto& utilizatori = Magazin::getInstance().getUtilizatori();
                    Magazin::getInstance().stergeUtilizator(idClient);
                    stergeUtilizatorSQL(idClient);
                    auto userNou = std::make_shared<ClientPremium>(nume);
                    Magazin::getInstance().adaugaUtilizator(userNou);
                    adaugaUtilizatorSQL(userNou);
                    userNou->AfiseazaOferte();
                    userNou->checkDiscount();
}
             break;}

             case 2:{// creeaza comanda noua
                if (id_client==0) {
                    std::cout<<"introdu id ul de client:"<<std::endl;
                    std::cin>>id_client;}

                Comanda c1(id_client);
                id_comanda=c1.getIdComanda();
                magazin.adaugaComanda(c1);
                adaugaComandaSQL(id_comanda, id_client,0 );
                std::cout<<"comanda id: "<<c1.getIdComanda()<<std::endl;

                std::string nume_produs;
                std::cout << "Introdu numele produsului de adaugat: ";
                std::cin >> nume_produs;


                std::shared_ptr<Produs> produs = gasesteProdusSQL(nume_produs);

                if (produs != nullptr ) {
                    c1.adaugaProdus(produs);
                    std::cout << "Produsul a fost adaugat la comanda " << id_comanda << std::endl;
                    c1.afiseazaProduse();
                    adaugaComandaSQL(id_comanda,id_client, c1.getTotal());
                } else {
                    std::cout << "Produsul nu a fost găsit în inventar." << std::endl;
                }
                 if (c1.getTotal()>399) {
                 std::cout<<"acum poti fi client premium"<<std::endl;
                 std::cout<<"introdu numele tau: ";
                 std::cin>>nume;
                 ClientPremium client(nume);
                 client.AfiseazaOferte();
                 std::cout<<"strange puncte pentru discount"<<std::endl;}
             break;}

             case 3:{//afisare produse din comanda cu id dat de user
                std::cout<<"introdu id_comanda:"<<std::endl;
                std::cin>>id_comanda;
                Comanda* comandaa = Magazin::getInstance().gasesteComanda(id_comanda);

                 if (comandaa == nullptr) {
                     std::cerr << "Eroare: Comanda nu a fost gasita" << std::endl;
                 } else {
                     comandaa->afiseazaProduse();}
             break;}

             case 4:{ //afis factura cu id dat de user
                std::cout<<"introdu id_comanda:"<<std::endl;
                std::cin>>id_comanda;
                Factura f(id_comanda, magazin);
                f.afisareFactura();
             break;}

             case 5:{ //elimin o comanda
                std::cout<<"Introdu id_comenzii pe care vrei sa o elimini:"<<std::endl;
                std::cin>>id_comanda;
                magazin.stergeComanda(id_comanda);
             break;}

             case 6:{ //geter pt inventar
                afiseazaProduseSQL();
             break;}

             case 7: {//geter pt comenzi magazin
                magazin.afisareComenzi();
             break;}

             case 8:{//ob produs nou
                 double pret=2;
                 int stoc=3;
                 Magazin::getInstance().adaugaProdusFurnizor();
                 Magazin::getInstance().adaugaProdus(std::make_shared<Produs>(nume, pret, stoc));
                 adaugaProdusSQL(nume, pret, stoc);
             break;}

             case 9:{//review
                  std::string parere;
                   int rating;
                 std::cout<<"Vei primi un cont special!"<<std::endl;
                 std::cout<<"introdu nume:"<<std::endl;
                 std::cin>>nume;
                 std::cout<<"introdu id ul de client:"<<std::endl;
                 std::cin>>id_client;
                 ClientRev client(nume);
                 std::cout<<"spune ne parerea ta:"<<std::endl;
                 std::cin.ignore();
                 std::getline(std::cin >> std::ws, parere);
                 std::cout<<"ofera rating:"<<std::endl;
                 std::cin>>rating;
                 client.scrieReview(parere, rating);
                 Magazin::getInstance().stergeUtilizator(id_client);
                 stergeUtilizatorSQL(id_client);
                 auto userNou = std::make_shared<ClientPremium>(nume);
                 Magazin::getInstance().adaugaUtilizator(userNou);
                 adaugaUtilizatorSQL(userNou);


             break;}

             case 10:{//sortare produse
                 Magazin::getInstance().sorteazaProduseDupaPret();
             break;}

             case 11:{//adaugare furnizor
                 std::cout<<"introdu numele furnizorului:"<<std::endl;
                 std::cin>>nume;
                 Magazin::getInstance().adaugaFurnizor(std::make_shared<Furnizor>(nume));
                 Magazin::getInstance().adaugaProdusFurnizor();
             break;}

             case 12:{//adauga produse pt furnizor
                 Magazin::getInstance().adaugaProdusFurnizor();
             break;}

             case 13:{//selecteaza produse de la furnizor
                 auto& furnizori = Magazin::getInstance().getFurnizori();
                 std::cout << "Numar furnizori: " << furnizori.size() << '\n';

                 std::cout << "Selecteaza furnizorul:\n";
                 for (size_t i = 0; i < furnizori.size(); ++i) {
                     std::cout << i + 1 << ". " << furnizori[i]->getNume() << '\n';}

                 int opt;
                 std::cin >> opt;

                 if (opt < 1 || opt > furnizori.size()) {
                     std::cout << "Optiune invalida\n";}
                 auto furnizorSelectat = furnizori[opt - 1];
                 const auto& produse = furnizorSelectat->getProduseFurnizate();

                 if (produse.empty()) {
                     std::cout << "Acest furnizor nu are produse disponibile\n";
                     break;
                 }

                 std::cout << "Produsele furnizorului " << furnizorSelectat->getNume() << ":\n";
                 for (size_t i = 0; i < produse.size(); ++i) {
                     std::cout << i + 1 << ". " << produse[i]->getNume()
                               << " - " << produse[i]->getPret() << " RON\n";
                 }

                 std::cout << "Selecteaza un produs pentru a-l adauga in magazin:\n";
                 int optProdus;
                 std::cin >> optProdus;
                 if (optProdus < 1 || optProdus > (int)produse.size()) {
                     std::cout << "Optiune invalida\n";
                     break;
                 }
                 Magazin::getInstance().adaugaProdus(produse[optProdus - 1]);
                 adaugaProdusSQL(produse[optProdus - 1]->getNume(),produse[optProdus - 1]->getPret(), produse[optProdus - 1]->getStoc());
                 std::cout << "Produsul a fost adaugat in inventarul magazinului\n";
             break;}

             case 14:{//colaborator
                int cod, stoc;
                double pret;
                std::cout<<"Introdu nume:"<<std::endl;
                std::cin>>nume;
                std::cout<<"introdu codul tau"<<std::endl;
                std::cin>>cod;
                Colaborator c(nume,cod);
                std::cout<<"produs:"<<std::endl;
                std::cin>>nume;
                 std::cout<<"pret:"<<std::endl;
                std::cin>>pret;
                 std::cout<<"stoc:"<<std::endl;
                std::cin>>stoc;
                c.adaugaProdus(std::make_shared<Produs>(nume, pret, stoc));
             break;}
                case 15:{
                 Helper::afiseazaDaca(Magazin::getInstance().getInventar(), [](const Produs& p) {
                   return p.getPret() > 1000;});break;}
                default:
                    std::cout << "Optiune invalida. Incearca din nou." << std::endl;
        }

}
        inchideBazaDeDate();
        return 0;
    }



