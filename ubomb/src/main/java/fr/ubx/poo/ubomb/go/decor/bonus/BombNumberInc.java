package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class BombNumberInc extends Bonus{
    public BombNumberInc(Position position) {
        super(position);
    }

    public void takenBy(Player player){
        //Si le joueur le prend, il gagne une bombe
        player.addNbBomb(1);
        this.remove();
    }
}
