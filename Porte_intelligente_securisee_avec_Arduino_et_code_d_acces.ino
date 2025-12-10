#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
LiquidCrystal_I2C ecran(0x27, 16, 2);
Servo servo;

const byte ligne = 4, colonne = 4;
char touches[4][4] = {{'1','2','3','A'},
                      {'4','5','6','B'},
                      {'7','8','9','C'},
                      {'*','0','#','D'}};
byte pin_lignes[4] = {9,8,7,6}, pin_colonnes[4] = {5,4,3,2};
Keypad clavier = Keypad(makeKeymap(touches), pin_lignes, pin_colonnes, ligne, colonne);

char entree[5] = "", code[5] = "0707";
int i = 0, essais = 0;
bool porte_fermee = true;
unsigned long derniere_mise_a_jour = 0;
const unsigned long delai_auto_fermeture = 5000;

const byte mode_normal = 0;
const byte mode_modification_code = 1;
const byte mode_menu = 2;
const byte mode_aide = 3;
byte mode_actuel = mode_normal;
char nouveau_code[5];
bool mode_confirmation = false;

void setup() {
  ecran.init();
  ecran.backlight();
  servo.attach(10);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  ecran.clear();
  ecran.print("Porte IS");
  ecran.setCursor(0, 1);
  ecran.print("Par KH & ST!");
  bip(2);
  delay(2000);
  ecran.clear();
  ecran.print("Initialisation");
  for (int j = 0; j < 3; j++) {
    ecran.setCursor(j * 5, 1);
    ecran.print(".");
    bip(1);
    delay(300);
  }
  afficher_instructions();
  maj_porte();
}

void loop() {
  char t = clavier.getKey();
  if (t) {
    bip_touche();
    derniere_mise_a_jour = millis();
    switch (mode_actuel) {
      case mode_normal: gestion_mode_normal(t); break;
      case mode_modification_code: gestion_mode_modification(t); break;
      case mode_menu: gestion_menu(t); break;
      case mode_aide: gestion_aide(t); break;
    }
  }

  if (!porte_fermee && millis() - derniere_mise_a_jour > delai_auto_fermeture) {
    ecran.clear();
    ecran.print("Auto-fermeture");
    bip_long(1);
    delay(1500);
    porte_fermee = true;
    maj_porte();
    afficher_instructions();
    derniere_mise_a_jour = millis();
  }
}

void gestion_mode_normal(char t) {
  if (t == 'A') {
    if (!porte_fermee) {
      mode_actuel = mode_menu;
      afficher_menu();
    } else {
      erreur("Menu verrouille");
    }
  } 
  else if (t == 'C') {
    if (i == 0) {
      erreur("Code vide!");
    } else {

      entree[i] = '\0';
      if (strcmp(entree, code) == 0) {
        essais = 0;
        porte_fermee = !porte_fermee;
        maj_porte();
        ecran.clear();
        ecran.print(porte_fermee ? "Porte fermee" : "Porte ouverte");
        delay(1500);
        afficher_instructions();
        derniere_mise_a_jour = millis();
      } else {
        essais++;
        signaler_erreur();
        ecran.clear();
        ecran.print("Mauvais code");
        delay(1500);
        if (essais >= 3) bloquage();
        afficher_instructions();
      }
      i = 0;
    }

  } 
  else if (t == 'D') {
    if (i > 0) {
      i--;
      ecran.setCursor(5 + i, 1);
      ecran.print(" ");
      ecran.setCursor(5 + i, 1);
    }
  } 
  else if ((t >= '0' && t <= '9') || t == '#' || t == '*') {
    if (i < 4) {
      entree[i++] = t;
      ecran.setCursor(5 + i - 1, 1);
      ecran.print(".");
    }
  }
}

void gestion_mode_modification(char t) {
  if (!mode_confirmation) {
    if (t == 'C') {
      if (i == 0) {
        erreur("Code vide!");
        mode_actuel = mode_normal;
        afficher_instructions();
      } else if (i < 4) {
        erreur("Code trop court");
      } else {
        nouveau_code[i] = '\0';
        mode_confirmation = true;
        ecran.clear();
        ecran.print("Confirmez code:");
        i = 0;
      }

    } 
    else if (t == 'D') {
      if (i > 0) {
        i--;
        ecran.setCursor(5 + i, 1);
        ecran.print(" ");
        ecran.setCursor(5 + i, 1);
      } else {
        mode_actuel = mode_normal;
        afficher_instructions();
      }
    } 
    else if ((t >= '0' && t <= '9') || t == '#' || t == '*') {
      if (i < 4) {
        nouveau_code[i++] = t;
        ecran.setCursor(5 + i - 1, 1);
        ecran.print(".");
      }
    }
  } 

  else {
    if (t == 'C') {
      if (i == 0) {
        erreur("Code vide!");
        mode_confirmation = false;
        mode_actuel = mode_normal;
        afficher_instructions();
      } else if (i < 4) {
        erreur("Code trop court");
        mode_confirmation = false;
        ecran.clear();
        ecran.print("Nouveau code:");
        i = 0;
      } else {
        entree[i] = '\0';
        if (strcmp(entree, nouveau_code) == 0) {
          strcpy(code, nouveau_code);
          ecran.clear();
          ecran.print("Code change avec");
          ecran.setCursor(0, 1);
          ecran.print("succes!");
          signaler_succes();
        } else {erreur("Codes differents");}
        mode_confirmation = false;
        mode_actuel = mode_normal;
        delay(1500);
        afficher_instructions();
        i = 0;
      }

    } 
    else if (t == 'D') {
      if (i > 0) {
        i--;
        ecran.setCursor(5 + i, 1);
        ecran.print(" ");
        ecran.setCursor(5 + i, 1);
      }
    } 
    else if ((t >= '0' && t <= '9') || t == '#' || t == '*') {
      if (i < 4) {
        entree[i++] = t;
        ecran.setCursor(5 + i - 1, 1);
        ecran.print(".");
      }
    }
  }
}

void gestion_menu(char t) {
  if (porte_fermee) {
    erreur("Acces refuse");
    mode_actuel = mode_normal;
    afficher_instructions();
    return;
  }

  switch (t) {
    case '1':
      mode_actuel = mode_modification_code;
      ecran.clear();
      ecran.print("Nouveau code:");
      i = 0;
      break;
    case '2':
      porte_fermee = true;
      maj_porte();
      ecran.clear();
      ecran.print("Porte fermee!");
      delay(1500);
      mode_actuel = mode_normal;
      afficher_instructions();
      break;
    case '3':
      mode_actuel = mode_aide;
      afficher_aide1();
      break;
    case '4':
    case 'D':
      mode_actuel = mode_normal;
      afficher_instructions();
      break;
  }
}

void gestion_aide(char t) {
  if (t == 'B') afficher_aide2();
  else if (t == 'A' || t == 'D') afficher_aide1();
  else if (t == 'C') {
    mode_actuel = mode_normal;
    afficher_instructions();
  }
}

void afficher_menu() {
  ecran.clear();
  ecran.print("1:Code 2:Fermer");
  ecran.setCursor(0, 1);
  ecran.print("3:Aide 4:Retour");
}

void afficher_aide1() {
  ecran.clear();
  ecran.print("A:Menu C:Valider");
  ecran.setCursor(0, 1);
  ecran.print("D:Retour/Effacer");
}

void afficher_aide2() {
  ecran.clear();
  ecran.print("*:Ancien code");
  ecran.setCursor(0, 1);
  ecran.print("B:Suite C:Retour");
}

void afficher_instructions() {
  ecran.clear();
  if (!porte_fermee) ecran.print("A:Menu");
  else ecran.print("Code:");
  ecran.setCursor(0, 1);
  ecran.print(">");
}

void bloquage() {
  ecran.clear();
  ecran.print("Bloque 10s!");
  for (int j = 0; j < 10; j++) {
    ecran.setCursor(13, 1);
    ecran.print(10 - j);
    digitalWrite(11, HIGH);
    bip(1);
    delay(500);
    digitalWrite(11, LOW);
    delay(500);
  }
  essais = 0;
}

void maj_porte() {
  servo.write(porte_fermee ? 0 : 90);
  digitalWrite(11, porte_fermee);
  digitalWrite(12, !porte_fermee);
  bip_long(1);
}

void erreur(const char* msg) {
  ecran.clear();
  ecran.print(msg);
  signaler_erreur();
  delay(1300);
}

void signaler_erreur() {
  digitalWrite(11, HIGH);
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(13, LOW);
  if (!porte_fermee) {
  digitalWrite(11, LOW);
  }
}

void signaler_succes() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(12, HIGH);
    bip(1);
    delay(100);
    digitalWrite(12, LOW);
    if (i < 2) delay(100);
  }
}

void bip_touche() {
  digitalWrite(13, HIGH);
  delay(30);
  digitalWrite(13, LOW);
}

void bip(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(13, HIGH);
    delay(80);
    digitalWrite(13, LOW);
    if (i < n - 1) delay(80);
  }
}

void bip_long(int n) {
  for (int i = 0; i < n; i++) {
    analogWrite(13, 127);
    delay(500);
    digitalWrite(13, LOW);
    if (i < n - 1) delay(200);
  }
}