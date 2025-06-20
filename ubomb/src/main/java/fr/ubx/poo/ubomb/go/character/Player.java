/*
 * Copyright (c) 2020. Laurent Réveillère
 */

package fr.ubx.poo.ubomb.go.character;

import fr.ubx.poo.ubomb.game.Direction;
import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.Movable;
import fr.ubx.poo.ubomb.go.TakeVisitor;
import fr.ubx.poo.ubomb.go.decor.*;
import fr.ubx.poo.ubomb.go.decor.bonus.Key;


import static fr.ubx.poo.ubomb.game.Direction.*;

public class Player extends Character implements Movable, TakeVisitor {
    private int nbBomb;
    private int rangeBomb;
    private int nbCle;

    /*Savoir si le joueur a changé de level, la valeur sera -1 s'il revient à un niveau précédent,
    0, s'il n'a pas changé de level et 1 s'il va au niveau précédent*/
    private int changeLevel;
    private boolean boxCollision;


    public Player(Game game, Position position) {
        super(game, position,game.configuration().playerLives(),game.configuration().playerInvisibilityTime());
        nbBomb = game.configuration().bombBagCapacity();
        rangeBomb = 1;
        nbCle = 0;
        boxCollision = false;
        changeLevel = 0;
    }

    //Gestion des clés
    public int getCle() {return nbCle;}
    public void addCle(int add){nbCle+=add;}
    public void take(Key key){addCle(1);}

    //Gestion des levels
    public int getChangeLevel(){ return changeLevel;}
    public void setChangeLevel(int i){
        changeLevel = i;
    }
    public void changeLevel() {
        if(changeLevel==-1){
            game.addLevel(-1);
        }
        if( changeLevel==1)
            game.addLevel(1);

        //On cherche à savoir où on doit mettre le joueur
        for (var decor : game.grid().values()) {
            if (decor.isDoorPrevOpened() && changeLevel == 1) {
                this.setPosition(decor.getPosition());
            }
            if (decor.isDoorNextOpened() && changeLevel == -1) {
                this.setPosition(decor.getPosition());
            }
        }
        changeLevel = 0;
    }

    //Gestion de la collision avec une boite
    public void setBoxCollision(boolean bool){
        boxCollision = bool;
    }
    public boolean getBoxCollision(){return boxCollision;}


    //Gestion des bombes
    public int getNbBomb(){return nbBomb;}
    public void addNbBomb(int nb){
        if(nbBomb + nb >= 0) {
            nbBomb += nb;
        }
    }
    public int getRangeBomb(){return rangeBomb;}
    public void addRangeBomb(int nb){
        if(rangeBomb + nb >= 1) {
            rangeBomb += nb;
        }
    }

    //Gestion des mouvements
    public void doMove(Direction direction) {
        // This method is called only if the move is possible, do not check again
        Position nextPos = direction.nextPosition(getPosition());
        Decor next = game.grid().get(nextPos);
        if (next != null) {
            if(!next.isBox()){
                //Si le bonus n'est pas une boite, le joueur le prend
                next.takenBy(this);
                setPosition(nextPos);
            }else{
                //Vérification que la boite ne sort pas des limites de la map
                boolean isOnBorder = (
                        (nextPos.x() == 0 && getDirection() == LEFT) ||
                                (nextPos.x() == game.grid().width()-1 && getDirection() == RIGHT) ||
                                (nextPos.y() == 0 && getDirection() == UP) ||
                                (nextPos.y() == game.grid().height()-1 && getDirection() == DOWN)
                );
                //Vérification qu'il n'y ait pas de décor
                Decor targetBox = game.grid().get(getDirection().nextPosition(nextPos));
                boolean boxCanMove = targetBox == null;

                //Vérification qu'il n'y ait pas de bombe
                for(var bombs:game.listBombs()){
                    if(direction.nextPosition(nextPos).equals(bombs.getPosition()))
                        boxCanMove=false;
                }

                //Vérification qu'il n'y ait pas de monstre
                for(var monster:game.listMonster()){
                    if(direction.nextPosition(nextPos).equals(monster.getPosition()))
                        boxCanMove=false;
                }

                if (!isOnBorder && boxCanMove){
                    //Si on peut, on prend la caisse, et on change la position
                    next.takenBy(this);
                    setPosition(nextPos);
                }
            }
        }else{
            //Si le decor est null, on peut bouger
            setPosition(nextPos);
        }

    }

    public final boolean canMove(Direction direction) {
        //On vérifie si le joueur peut aller dans une direction
        Decor target = game.grid().get(direction.nextPosition(this.getPosition()));
        if(target!=null){
            return target.notWalkableBy(this);
        }
        return canMove(direction,game.level());
    }

    public void checkCollision(long now) {
        // Check a collision between a monster and the player
        update();
        getInvulnerabilityTimer().update(now);
        if(getInvulnerabilityTimer().notIsRunning()){
            for(var monster: game.listMonster()){
                if(monster.getPosition().equals(game.player().getPosition())){
                    addNbLives(-1);
                    getInvulnerabilityTimer().start();
                }
            }
        }
    }

}
