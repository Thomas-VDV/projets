package fr.ubx.poo.ubomb.go.decor.bonus;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Monster;
import fr.ubx.poo.ubomb.go.character.Player;


public class Box extends Bonus {
    public Box(Position position) {
        super(position);
    }

    public boolean notWalkableBy(Monster Monster) {
        //Le monstre ne peut pas marcher sur les boites
        return true;
    }

    @Override
    public void takenBy(Player player){
        remove();
        player.setBoxCollision(true);
    }

    public boolean isBox(){
        //Permet de savoir si GameObject est une box
        return true;
    }
}
