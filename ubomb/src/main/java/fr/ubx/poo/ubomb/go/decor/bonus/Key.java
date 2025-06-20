/*
 * Copyright (c) 2020. Laurent Réveillère
 */

package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;

public class Key extends Bonus {
    public Key(Position position) {
        super(position);
    }

    @Override
    public void explode() {/*On la redéfinie pour que la clé n'explose pas*/}

    @Override
    public void takenBy(Player player) {
        player.take(this);
        this.remove();
    }

}
