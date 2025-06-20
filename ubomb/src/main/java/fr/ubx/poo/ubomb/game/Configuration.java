package fr.ubx.poo.ubomb.game;

import java.util.List;

/**
 * @param playerPosition Classe permettant de stocker tous les paramètres de base du jeu
 */
public record Configuration(Position playerPosition, int bombBagCapacity, int playerLives, long playerInvisibilityTime,
                            int monsterVelocity, long monsterInvisibilityTime,
                            List<List<Position>> listPositionMonster) {
}
