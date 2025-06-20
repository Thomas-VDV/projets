package fr.ubx.poo.ubomb.game;

import fr.ubx.poo.ubomb.go.character.Player;
import fr.ubx.poo.ubomb.go.character.Monster;
import fr.ubx.poo.ubomb.go.Bomb;
import fr.ubx.poo.ubomb.go.decor.Decor;


import java.util.ArrayList;
import java.util.List;

public class Game {

    private final Configuration configuration;
    private final Player player;
    private final Grid[] grid;
    private final List<List<Monster>> listMonster = new ArrayList<>();
    private final List<List<Bomb>> listBombs= new ArrayList<>();
    private int level;

    public Game(Configuration configuration, Grid[] tabLevel) {
        this.configuration = configuration;
        this.grid = tabLevel;
        player = new Player(this, configuration.playerPosition());
        level = 0;

        //Initialisation des listes de monstres et de bombes
        for(int nbLevel=0;nbLevel<grid.length;nbLevel++){
            listBombs.add(new ArrayList<>());
            listMonster.add(new ArrayList<>());
            for(var pos : configuration().listPositionMonster().get(nbLevel)){
                if(nbLevel==0){
                    listMonster.get(nbLevel).add(new Monster(pos,this,configuration.monsterVelocity()*1000/(nbLevel+1),1,nbLevel));

                }else{
                    listMonster.get(nbLevel).add(new Monster(pos,this,configuration.monsterVelocity()*1000/(nbLevel+1),nbLevel+nbLevel%2,nbLevel));
                }
            }
        }
    }

    public Configuration configuration() {
        return configuration;
    }

   public Grid grid() {
        //Permet de récupérer la grille du niveau actuel
        return grid[level];
    }
    public Grid grid(int level) {
        //Permet de récupérer la grille d'un niveau particulier
        return grid[level];
    }

    public Player player() {
        return this.player;
    }

    public int nbLevel(){return grid.length;}

    public int level(){
        return level;
    }

    public void addLevel(int add){
        level += add;
    }

    public List<Monster> listMonster() {
        //Renvoie la liste de monstres du niveau actuel
        return listMonster.get(level);
    }

    public List<Monster> listMonster(int level) {
        //Renvoie la liste de monstres d'un niveau particulier
        return listMonster.get(level);
    }

    public List<Bomb> listBombs() {return listBombs.get(level);}

    public List<Bomb> listBombs(int level) {return listBombs.get(level);}

    public Bomb createBomb(){
        //Si le joueur a une bombe et qu'il n'est pas sur une porte, alors il crée une bombe et la place sur sa position
        Decor target = grid().get(player.getPosition());
        if (player().getNbBomb() <= 0 || (target!=null) && (target.isDoorNextOpened() || target.isDoorPrevOpened()))
            return null;
        Bomb newBomb = new Bomb(this,player().getPosition(),level);
        listBombs().add(newBomb);
        player().addNbBomb(-1);
        return newBomb;
    }
}
