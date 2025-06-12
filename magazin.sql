USE magazin_online;

CREATE TABLE Produs (
                        id INT AUTO_INCREMENT PRIMARY KEY,
                        denumire VARCHAR(100) NOT NULL,
                        pret DECIMAL(10,2) NOT NULL,
                        stoc INT NOT NULL
);

CREATE TABLE Client (
                        id INT AUTO_INCREMENT PRIMARY KEY,
                        nume VARCHAR(100) NOT NULL
);

CREATE TABLE Cos (
                     id INT AUTO_INCREMENT PRIMARY KEY,
                     client_id INT,
                     FOREIGN KEY (client_id) REFERENCES Client(id) ON DELETE CASCADE
);

CREATE TABLE Cos_Produse (
                             cos_id INT,
                             produs_id INT,
                             cantitate INT NOT NULL,
                             PRIMARY KEY (cos_id, produs_id),
                             FOREIGN KEY (cos_id) REFERENCES Cos(id) ON DELETE CASCADE,
                             FOREIGN KEY (produs_id) REFERENCES Produs(id) ON DELETE CASCADE
);
