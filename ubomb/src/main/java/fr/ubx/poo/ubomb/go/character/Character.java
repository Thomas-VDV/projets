package fr.ubx.poo.ubomb.go.character;

import fr.ubx.poo.ubomb.engine.Timer;
import fr.ubx.poo.ubomb.game.Direction;
import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.GameObject;
import fr.ubx.poo.ubomb.go.Movable;
import static fr.ubx.poo.ubomb.game.Direction.*;

public abstract class Character extends GameObject implements Movable {
    private Direction direction;
    private boolean moveRequested = false;
    private int nbLives;
    private final Timer invulnerabilityTimer;


    public Character(Game game, Position position,int nbLives,long invulnerabilityTime) {
        super(game, position);
        this.nbLives = nbLives;
        direction =Direction.DOWN;
        this.invulnerabilityTimer = new Timer(invulnerabilityTime);
    }

    //Gestion de la vie
    public int getNbLives(){
        return nbLives;
    }
    public void addNbLives(int add) {
        this.nbLives = nbLives + add;
    }
    public boolean notIsAlive() {
        return nbLives <= 0;
    }
    public Timer getInvulnerabilityTimer(){return invulnerabilityTimer;}

    //Gestion des mouvements
    public void requestMove(Direction direction) {
        //On veut faire un mouvement
        if (direction != this.direction) {
            this.direction = direction;
            setModified(true);
        }
        moveRequested = true;
    }

    public Direction getDirection(){return direction;}

    public void update() {
        //On fait un mouvement que si c'est requested et que l'action est possible
        if (moveRequested) {
            if (canMove(getDirection())) {
                doMove(getDirection());
            }
        }
        moveRequested = false;
    }

    public final boolean canMove(Direction direction, int level){
        //Vérification si le mouvement ne fait pas sortir de la taille de la grille du level
        if (direction == UP) {
            return getPosition().y() > 0;
        } else if (direction == DOWN) {
            return getPosition().y() < game.grid(level).height() - 1;
        } else if (direction == LEFT) {
            return getPosition().x() > 0;
        } else if (direction == RIGHT) {
            return getPosition().x() < game.grid(level).width() - 1;
        }
        return false;
    }
}
