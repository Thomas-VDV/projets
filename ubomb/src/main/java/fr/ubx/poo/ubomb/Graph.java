package fr.ubx.poo.ubomb;

import java.util.*;



public class Graph<T> {
    private final HashSet<Node<T>> nodes;

    public Graph() {
        this.nodes = new HashSet<>();
    }
    public void addNode(T data) {
        this.nodes.add(new Node<>(data));
    }

    public Node<T> getNode(T data) {
        /*Permet de récupérer un noeud ayant une certaine donnée, s'il y en a plusieurs, il retourne le premier,
            mais dans notre cas, on n'a qu'un graphe de position donc on n'a pas de problème*/
        for (Node<T> node : nodes) {
            if(data.equals(node.getData())){
                return node;
            }
        }
        return null;
    }

    public HashSet<Node<T>> nodes() {
        //Permet de parcourir tous les nœuds du graphe facilement
        return nodes;
    }

    public int sizeGraph(){
        return nodes.size();
    }

}