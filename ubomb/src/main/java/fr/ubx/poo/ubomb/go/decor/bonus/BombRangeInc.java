package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class BombRangeInc extends Bonus{
    public BombRangeInc(Position position) {
        super(position);
    }

    public void takenBy(Player player){
        //Si le joueur le prend, la portée de ses bombes augmente
        player.addRangeBomb(1);
        this.remove();
    }
}
