package fr.ubx.poo.ubomb.go.decor;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class Door extends Decor{
    public Door(Position position) {
        super(position);
    }
    public boolean notWalkableBy(Player player) {
        //Le joueur peut marcher sur les portes
        return true;
    }
}
