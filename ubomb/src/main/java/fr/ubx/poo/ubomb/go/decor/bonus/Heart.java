package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class Heart extends Bonus{
    public Heart(Position position) {
        super(position);
    }

    public void takenBy(Player player){
        //Si le joueur le prend, il gagne une vie
        player.addNbLives(1);
        this.remove();
    }
}
