package fr.ubx.poo.ubomb.go;

import fr.ubx.poo.ubomb.engine.Timer;
import fr.ubx.poo.ubomb.game.Direction;
import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Monster;

import java.util.ArrayList;
import java.util.List;

import static fr.ubx.poo.ubomb.game.Direction.*;

public class Bomb extends GameObject{
    private final int level;
    private final Timer explosionTimer = new Timer(4000);
    private int imageNum = 3;


    public Bomb(Game game,Position position,int level) {
        super(game,position);
        explosionTimer.start();
        this.level =level;
    }

    @Override
    public void explode() {
        //Si une bombe explose, elle fait exploser les autres
        explosionTimer.end();
    }

    public int getLevel(){return level;}

    public Timer getExplosionTimer(){return explosionTimer;}

    public int getImageNum(){
        return imageNum;
    }


    public void update(long now) {
        //On change d'image toutes les secondes pour animer l'explosion
        getExplosionTimer().update(now);
        long bombTimerRemaining = getExplosionTimer().remaining();
        if (3000 < bombTimerRemaining && bombTimerRemaining <= 4000){
            imageNum = 3;
        } if (2000 < bombTimerRemaining && bombTimerRemaining <= 3000){
            imageNum = 2;
        } if (1000 < bombTimerRemaining && bombTimerRemaining <= 2000){
            imageNum = 1;
        } if (0 < bombTimerRemaining && bombTimerRemaining <= 1000){
            imageNum = 0;
        }
        this.setModified(true);
    }


    public void bombExplode(long now){
        //On vérifie si un monstre est sur la case de la bombe quand elle explose et s'il n'est pas invulnérable
        for(var monster: game.listMonster(level)){
            monster.getInvulnerabilityTimer().update(now);
            if(monster.getPosition().equals(this.getPosition()) && monster.getInvulnerabilityTimer().notIsRunning()){
                monster.addNbLives(-1);
                if (monster.notIsAlive()){
                    monster.remove();
                }
                monster.getInvulnerabilityTimer().start();
            }
        }

        //On vérifie si une bombe est sur la case de la bombe quand elle explose
        for(var checkBomb: game.listBombs(level)) {
            if(checkBomb.getPosition().equals(this.getPosition()) && this!=checkBomb){
                checkBomb.explode();
            }
        }

        //On vérifie si le joueur n'est pas sur la case de la bombe quand elle explose
        if (this.getPosition().equals(game.player().getPosition()) && this.getLevel()==level){
            game.player().addNbLives(-1);
        }
    }

    public int bombExlpoRangeDir(Direction dir,long now){
        int updatedRange = 0;
        //On vérifie pour chaque niveau de la portée, s'il y a un decor sur le passage ou la fin de la map
        for (int range = 1; range <= game.player().getRangeBomb(); range++){
            GameObject newGameObject = game.grid(level).get(dir.nextPosition(this.getPosition(), range));
            if (newGameObject!=null && newGameObject.isBonus()){
                //Si le gameObject est un bonus, alors il explose, mais la déflagration s'arrête sur la case du bonus
                newGameObject.explode();
                updatedRange = range;
                break;
            } else if (newGameObject!=null && newGameObject.isDecor() || (dir == UP && (this.getPosition().y() - updatedRange <= 0)) || dir == DOWN && (this.getPosition().y() + updatedRange >= game.grid(level).height()-1) ||
                    dir == LEFT && (this.getPosition().x() - updatedRange <= 0) || dir == RIGHT && (this.getPosition().x() + updatedRange >= game.grid(level).width()-1)){
                //Si le gameObject est un decor, alors il n'explose pas, donc la déflagration ne l'atteins pas et la déflagration ne sort pas de la map
                break;
            } else if (dir.nextPosition(this.getPosition(), range).equals(game.player().getPosition())) {
                //Si la bombe atteint un joueur alors il perd une vie et la déflagration passe quand même à la case de portée suivante
                game.player().addNbLives(-1);
            }

            //Si la bombe atteint un monstre alors il perd une vie et la déflagration passe quand même à la case de portée suivante
            List<Monster> monsterDead = new ArrayList<>();
            for(var monster: game.listMonster(level)){
                monster.getInvulnerabilityTimer().update(now);
                if(monster.getPosition().equals(dir.nextPosition(this.getPosition(), range)) && monster.getInvulnerabilityTimer().notIsRunning()){
                    monster.addNbLives(-1);
                    if (monster.notIsAlive()){
                        monster.remove();
                        monsterDead.add(monster);
                    }
                    monster.getInvulnerabilityTimer().start();
                }
            }

            //On supprime de la liste les monstres morts
            for(var monster:monsterDead){
                game.listMonster(level).remove(monster);
            }

            //Si la bombe atteint une bombe alors elle explose et la déflagration passe quand même à la case de portée suivante
            for(var checkBomb: game.listBombs(level)) {
                if(checkBomb.getPosition().equals(dir.nextPosition(this.getPosition(), range))){
                    checkBomb.explode();
                }
            }

            updatedRange = range;
        }
        return updatedRange;
    }
}
