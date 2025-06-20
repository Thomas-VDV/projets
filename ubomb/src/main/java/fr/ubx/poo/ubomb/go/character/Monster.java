package fr.ubx.poo.ubomb.go.character;


import fr.ubx.poo.ubomb.Graph;
import fr.ubx.poo.ubomb.Node;
import fr.ubx.poo.ubomb.engine.Timer;
import fr.ubx.poo.ubomb.game.Direction;
import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.decor.*;

import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;

import static fr.ubx.poo.ubomb.game.Direction.*;

public class Monster extends Character {
    private final Timer monsterTimer;
    private final int level;


    public Monster(Position position,Game game,int monsterVelocity,int nbLives,int level) {
        super(game, position,nbLives,game.configuration().monsterInvisibilityTime());
        this.monsterTimer = new Timer(monsterVelocity);
        monsterTimer.start();
        this.level = level;
    }

    public void updateMonster(long now) {
        //On fait bouger le monstre si son timer est fini
        monsterTimer.update(now);
        if(monsterTimer.notIsRunning()) {
            monsterTimer.start();
            if(level==game.nbLevel()-1 && game.level()==game.nbLevel()-1){
                requestMove(cheminPlusCourt());
            }else{
                requestMove(Direction.random());
            }
        }
        update();
    }

    public final boolean canMove(Direction direction) {
        //On teste si un monstre peut aller dans une direction
        Decor target = game.grid(level).get(direction.nextPosition(this.getPosition()));
        if(target!=null && target.notWalkableBy(this))
            return false;
        return canMove(direction,level);
    }

    public void doMove(Direction direction){
        Position nextPos = direction.nextPosition(getPosition());
        setPosition(nextPos);
    }


    public boolean isAccessible(Position pos){
        //On teste si une position est accessible pour un monstre
        if(pos.x()<0 || pos.x()>=game.grid(level).height() || pos.y()<0 || pos.y()>=game.grid(level).height()){
            return false;
        }
        return game.grid(level).get(pos) == null || !game.grid(level).get(pos).notWalkableBy(this);
    }

    public Graph<Position> getGraph() {
        //On crée un graph avec toutes les cases de la grille
        Graph<Position> g = new Graph<>();
        for (int i = 0; i < game.grid(level).width(); i++) {
            for (int j = 0; j < game.grid(level).height(); j++) {
                g.addNode(new Position(i, j));
            }
        }

        //On ajoute les arrête suivant si le monstre peut passer d'une case à une autre
        for(var node:g.nodes()){
            Position nodePos = node.getData();
            if(isAccessible(DOWN.nextPosition(nodePos))){
                node.addEdge(g.getNode(DOWN.nextPosition(nodePos)));
            }
            if(isAccessible(UP.nextPosition(nodePos))){
                node.addEdge(g.getNode(UP.nextPosition(nodePos)));
            }
            if(isAccessible(LEFT.nextPosition(nodePos))){
                node.addEdge(g.getNode(LEFT.nextPosition(nodePos)));
            }
            if(isAccessible(RIGHT.nextPosition(nodePos))){
                node.addEdge(g.getNode(RIGHT.nextPosition(nodePos)));
            }
        }

        return g;
    }




    public Direction cheminPlusCourt(){

        //Si le monstre est déjà sur le joueur alors il part dans une direction aléatoire
        if(game.player().getPosition().equals(this.getPosition())){
            return Direction.random();
        }

        //On crée un graphe de la map
        Graph<Position> graphe = getGraph();

        //On récupère le nœud du joueur et celui du monstre
        Node<Position> player = graphe.getNode(game.player().getPosition());
        Node<Position> monstre = graphe.getNode(this.getPosition());

        //On fait un parcours en profondeur en partant du nœud du monstre en récupérant le père de chaque noeud
        Hashtable<Node<Position>, Integer> couleur = new Hashtable<>(graphe.sizeGraph());
        Hashtable<Node<Position>, Node<Position>> pere = new Hashtable<>(graphe.sizeGraph());
        LinkedList<Node<Position>> file = new LinkedList<>();

        //On définit un noeud null en créant un noeud en dehors de la map
        Node<Position> nodePositionNull = new Node<>(new Position(game.grid(level).width()+1,game.grid(level).height()+1));


        int noir = 2;
        int gris = 1;
        int blanc = 0;

        //On initialise les deux tableaux avec les valeurs par défauts
        for(var node:graphe.nodes()) {
            couleur.put(node,blanc);
            pere.put(node,nodePositionNull);
        }

        //On prépare le premier noeud
        couleur.put(monstre,gris);
        file.add(monstre);

        //On fait le parcours en profondeur
        while(file.size()!=0){
            Node<Position> v = file.peekFirst();
            List<Node<Position>> voisinV = v.getNeighbours();
            for (Node<Position> w : voisinV) {
                if (couleur.get(w) == blanc) {
                    couleur.put(w, gris);
                    pere.put(w, v);
                    file.add(w);
                }
            }
            file.pollFirst();
            couleur.put(v,noir);
        }



        //Si le père n'est pas accessible, on renvoie une direction aléatoire
        if(pere.get(player).equals(nodePositionNull)){
            return Direction.random();
        }

        //On récupère le premier descendant qui mène vers le joueur
        while(pere.get(player)!=monstre){
            player = pere.get(player);
        }

        //On transforme le noeud en direction
        if(player.getData().equals(new Position(this.getPosition().x(), this.getPosition().y() + 1))){
            return DOWN;
        }else if(player.getData().equals(new Position(this.getPosition().x(), this.getPosition().y() - 1))){
            return UP;
        }else if(player.getData().equals(new Position(this.getPosition().x() - 1, this.getPosition().y()))){
            return LEFT;
        }else if(player.getData().equals(new Position(this.getPosition().x() + 1, this.getPosition().y()))){
            return RIGHT;
        }
        return Direction.random();
    }
}








