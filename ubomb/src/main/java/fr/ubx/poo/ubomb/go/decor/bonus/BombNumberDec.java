package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class BombNumberDec extends Bonus{
    public BombNumberDec(Position position) {
        super(position);
    }

    public void takenBy(Player player){
        //Si le joueur le prend, il perd une bombe
        player.addNbBomb(-1);
        this.remove();
    }
}
