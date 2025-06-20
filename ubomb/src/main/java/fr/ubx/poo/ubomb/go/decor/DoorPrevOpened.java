package fr.ubx.poo.ubomb.go.decor;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class DoorPrevOpened extends Door{
    public DoorPrevOpened(Position position) {
        super(position);
    }

    public void takenBy(Player player){
        //Si le joueur marche sur cette porte, il passe au level précédent
        player.setChangeLevel(-1);
    }
    public boolean isDoorPrevOpened() {
        //Permet de savoir qu'un decor est une porte vers un niveau précédent
        return true;
    }
}
