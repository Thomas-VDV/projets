package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class BombRangeDec extends Bonus {
    public BombRangeDec(Position position) {
        super(position);
    }

    public void takenBy(Player player){
        //Si le joueur le prend, la portée de ses bombes diminue
        player.addRangeBomb(-1);
        this.remove();
    }
}
