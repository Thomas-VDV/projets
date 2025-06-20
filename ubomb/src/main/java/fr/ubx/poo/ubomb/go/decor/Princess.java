package fr.ubx.poo.ubomb.go.decor;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;


public class Princess extends Decor{
    public Princess(Position position) {
        super(position);
    }


    @Override
    public boolean isPrincess() {
        //Permet de savoir qu'un decor est une princesse
        return true;
    }

    public boolean notWalkableBy(Player player) {
        //Un joueur peut marcher sur une princesse
        return true;
    }
}
