/*
 * Copyright (c) 2020. Laurent Réveillère
 */

package fr.ubx.poo.ubomb.view;

import fr.ubx.poo.ubomb.go.character.Monster;
import javafx.scene.image.Image;
import javafx.scene.layout.Pane;

public class SpriteMonster extends Sprite {

    public SpriteMonster(Pane layer, Monster monster) {
        super(layer, null, monster);
        updateImage();
    }

    @Override
    public void updateImage() {
        //Permet de mettre à jour l'image suivant la direction dont vient le monstre
        Monster monster = (Monster) getGameObject();
        Image image = ImageResourceFactory.getMonster(monster.getDirection()).getImage();
        setImage(image);
    }
}

