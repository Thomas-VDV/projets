package fr.ubx.poo.ubomb.go.decor;

import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.GameObject;
import fr.ubx.poo.ubomb.go.Takeable;
import fr.ubx.poo.ubomb.go.character.Player;


public abstract class Decor extends GameObject implements Takeable {

    public Decor(Position position) {
        super(position);
    }

    public void takenBy(Player player){/*Permet de prendre tous les decor même si ce ne sont pas des bonus*/}

    public DoorNextOpened openDoor(Game game){
        //Permet d'implémenter l'ouverture des portes fermées
        return null;
    }

    public boolean isPrincess(){
        //Permet de vérifier si un decor est une princesse
        return false;
    }

    public boolean isBox(){
        //Permet de vérifier si un decor est une boite
        return false;
    }

    public boolean isDecor() {
        //Permet de vérifier si un GameObject est un decor
        return true;
    }

    public boolean isDoorPrevOpened() {
        //Permet de vérifier si un decor est une porte ouverte qui mène vers le niveau précédent
        return false;
    }

    public boolean isDoorNextOpened() {
        //Permet de vérifier si un decor est une porte ouverte qui mène vers le niveau suivant
        return false;
    }
}