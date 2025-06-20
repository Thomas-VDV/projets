package fr.ubx.poo.ubomb.game;

public record Position (int x, int y) {
    public Position(Position position) {
        this(position.x, position.y);
    }

    public boolean equals(Position position) {
        return this.x == position.x() && this.y == position.y();
    }
}
