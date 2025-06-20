package fr.ubx.poo.ubomb.view;
import fr.ubx.poo.ubomb.go.Bomb;
import javafx.scene.image.Image;
import javafx.scene.layout.Pane;

public class SpriteBombs extends Sprite {

    public SpriteBombs(Pane layer, Bomb bomb) {
        super(layer, null, bomb);
        updateImage();
    }

    @Override
    public void updateImage() {
        //Permet de changer l'image de la bombe lors de l'avancement de la mèche
        Bomb bombs = (Bomb) getGameObject();
        Image image = ImageResourceFactory.getBomb(bombs.getImageNum()).getImage();
        setImage(image);
    }
}

