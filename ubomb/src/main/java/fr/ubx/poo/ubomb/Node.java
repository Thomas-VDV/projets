package fr.ubx.poo.ubomb;

import java.util.ArrayList;
import java.util.List;

public class Node<T> {
    private final T data;
    private final List<Node<T>> neighbours;

    public Node(T data) {
        this.data = data;
        this.neighbours = new ArrayList<>();
    }

    public T getData() { return this.data; }

    public void addEdge(Node<T> to) {
        //Permet d'ajouter un arc de ce node vers un autre
        if(to!=null){

            this.neighbours.add(to);
        }
    }

    public List<Node<T>> getNeighbours() {
        //Permet de récupérer la liste de ses enfants
        return this.neighbours; }

    public boolean equals(Node<T> node) {
        return data.equals(node.getData()) ;
    }
}
