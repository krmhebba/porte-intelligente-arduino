# Porte Intelligente Sécurisée avec Arduino

## Description
Ce projet est un système de contrôle d'accès autonome basé sur Arduino. Il pilote un servomoteur pour verrouiller ou déverrouiller une porte suite à la saisie d'un code PIN sur un clavier matriciel. Le système intègre une interface utilisateur via un écran LCD et un retour sonore.

## Fonctionnalités Principales
* Authentification sécurisée : Saisie d'un code PIN à 4 chiffres pour ouvrir la porte.
* Verrouillage Automatique : La porte se referme automatiquement après 5 secondes d'inactivité.
* Système de Menu : Interface permettant de changer le code, fermer la porte manuellement ou consulter l'aide.
* Sécurité Active : Blocage du système pendant 10 secondes après 3 tentatives de code erronées (avec alarme sonore).
* Modification du Code : Possibilité pour l'utilisateur de définir un nouveau code d'accès via le menu.
* Feedback Utilisateur : Affichage des instructions sur écran LCD I2C et signaux sonores (Buzzer) pour chaque action.

## Matériel et Connectique
* Microcontrôleur : Arduino Uno
* Affichage : Écran LCD 16x2 (Interface I2C, adresse 0x27)
* Entrée : Clavier matriciel 4x4 (Pins Lignes: 9,8,7,6 | Colonnes: 5,4,3,2)
* Actionneur : Servomoteur (Pin 10)
* Signalisation :
  * LED Rouge / État Fermé (Pin 11)
  * LED Verte / État Ouvert (Pin 12)
  * Buzzer (Pin 13)

## Configuration par Défaut
* Code PIN initial : 0707
* Délai de fermeture automatique : 5000 ms

## Structure du Code
Le programme utilise une machine à états pour gérer les différents modes de fonctionnement :
* Mode Normal : Attente de saisie de code.
* Mode Menu : Navigation entre les options.
* Mode Modification : Processus de changement et confirmation du nouveau code.
* Mode Aide : Affichage des touches de navigation.

## Bibliothèques Requises
* Keypad.h
* LiquidCrystal_I2C.h
* Servo.h

## Auteurs
Projet réalisé par Karam Hiba en collaboration avec Tarek Souibri.
Les initiales "KH & ST" s'affichent au démarrage du système sur l'écran LCD.
