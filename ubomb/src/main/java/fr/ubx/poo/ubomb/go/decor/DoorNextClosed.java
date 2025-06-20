package fr.ubx.poo.ubomb.go.decor;

import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class DoorNextClosed extends Door{

    public DoorNextClosed(Position position) {
        super(position);
    }

    @Override
    public DoorNextOpened openDoor(Game game) {
        //On ouvre la porte fermée en créant une nouvelle porte ouverte à la position de la porte fermée
        DoorNextOpened newDoor = new DoorNextOpened(this.getPosition());
        game.grid().set(this.getPosition(), newDoor);
        game.player().addCle(-1);
        newDoor.setModified(true);
        return newDoor;
    }

    public boolean notWalkableBy(Player player) {
        //Le joueur ne peut pas marcher sur une porte fermée
        return false;
    }
}
