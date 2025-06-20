package fr.ubx.poo.ubomb.go;

import fr.ubx.poo.ubomb.go.character.Monster;
import fr.ubx.poo.ubomb.go.character.Player;

public interface Walkable {
    default boolean notWalkableBy(Player player) {
        //Permet de savoir sur quel GameObject peut marcher un joueur
        return false;
    }

    default boolean notWalkableBy(Monster Monster) {
        //Permet de savoir sur quel GameObject peut marcher un monstre
        return true;
    }
}
