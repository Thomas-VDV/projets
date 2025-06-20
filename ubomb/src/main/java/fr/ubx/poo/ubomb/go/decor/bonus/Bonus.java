/*
 * Copyright (c) 2020. Laurent Réveillère
 */

package fr.ubx.poo.ubomb.go.decor.bonus;

import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Monster;
import fr.ubx.poo.ubomb.go.character.Player;
import fr.ubx.poo.ubomb.go.decor.Decor;

public abstract class Bonus extends Decor {
    public Bonus(Position position) {
        super(position);
    }

    @Override
    public boolean notWalkableBy(Player player) {
        //Le joueur peut marcher sur tous les bonus
        return true;
    }

    public boolean notWalkableBy(Monster monstre) {
        //Le monstre peut marcher sur tous les bonus
        return false;
    }

    @Override
    public void explode() {
        //On enlève les bonus quand ils explosent
        remove();
    }

    @Override
    public boolean isBonus() {
        //Permet de savoir si un decor est un bonus
        return true;
    }

}
