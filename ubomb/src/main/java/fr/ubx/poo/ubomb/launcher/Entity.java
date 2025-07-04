package fr.ubx.poo.ubomb.launcher;

public enum Entity {
    Empty('_'),
    Box('B'),
    Stone('S'),
    Tree('T'),

    BombRangeInc('>'),
    BombRangeDec('<'),
    BombNumberInc('+'),
    BombNumberDec('-'),
    Heart('H'),
    Key('K'),
    DoorPrevOpened('V'),
    DoorNextOpened('N'),
    DoorNextClosed('n'),
    Monster('M'),
    Princess('W');

    private final char code;

    Entity(char c) {
        this.code = c;
    }

    public static Entity fromCode(char c) {
        //Permet de transformer un char en entity
        for (Entity entity : values()) {
            if (entity.code == c)
                return entity;
        }
        throw new MapException("Invalid character " + c);
    }
}
