/*
 * Copyright (c) 2020. Laurent Réveillère
 */

package fr.ubx.poo.ubomb.engine;

import fr.ubx.poo.ubomb.game.Direction;
import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Position;
import fr.ubx.poo.ubomb.go.character.Player;
import fr.ubx.poo.ubomb.go.decor.*;
import fr.ubx.poo.ubomb.go.Bomb;
import fr.ubx.poo.ubomb.go.decor.bonus.Box;
import fr.ubx.poo.ubomb.view.*;
import javafx.animation.AnimationTimer;
import javafx.animation.TranslateTransition;
import javafx.application.Platform;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;
import javafx.stage.Stage;
import javafx.util.Duration;


import java.util.*;

import static fr.ubx.poo.ubomb.game.Direction.*;


public final class GameEngine {
    //Classe gérant la partie graphique du jeu
    private static AnimationTimer gameLoop;
    private final Game game;
    private final Player player;
    private final List<Sprite> sprites = new LinkedList<>();
    private final Set<Sprite> cleanUpSprites = new HashSet<>();
    private final Stage stage;
    private StatusBar statusBar;
    private Pane layer;
    private Input input;

    public GameEngine(Game game, final Stage stage) {
        this.stage = stage;
        this.game = game;
        this.player = game.player();
        initialize();
        buildAndSetGameLoop();

    }

    private void initialize() {
        //Mise en place de la partie graphique du jeu
        Group root = new Group();
        layer = new Pane();

        int height = game.grid().height();
        int width = game.grid().width();
        int sceneWidth = width * ImageResource.size;
        int sceneHeight = height * ImageResource.size;
        Scene scene = new Scene(root, sceneWidth, sceneHeight + StatusBar.height);
        scene.getStylesheets().add(Objects.requireNonNull(getClass().getResource("/css/application.css")).toExternalForm());

        stage.setScene(scene);
        stage.setResizable(false);
        stage.sizeToScene();
        stage.hide();
        stage.show();

        input = new Input(scene);
        root.getChildren().add(layer);
        statusBar = new StatusBar(root, sceneWidth, sceneHeight, game);
        recreateSprite();

    }

    private void recreateSprite(){
        //Clean old sprites
        sprites.forEach(Sprite::remove);
        sprites.clear();

        // Create sprites
        for (var decor : game.grid().values()) {
            sprites.add(SpriteFactory.create(layer, decor));
            decor.setModified(true);
        }
        for(var monster : game.listMonster()){
            sprites.add(new SpriteMonster(layer,monster));
        }
        for(var bombs : game.listBombs()){
            sprites.add(new SpriteBombs(layer,bombs));
        }
        sprites.add(new SpritePlayer(layer, player));

        //Ajout d'un mur de pierre pour ne pas avoir de mur invisible
        if(game.level()!=0){
            if (game.grid(0).width() > game.grid().width()-1) {
                for (int y = 0 ; y < game.grid().height() ; y++){
                    sprites.add(SpriteFactory.create(layer, new Stone(new Position(game.grid().width(),y))));
                }
            }
            if (game.grid(0).height() > game.grid().height()-1) {
                for (int x = 0 ; x < game.grid().width() ; x++){
                    sprites.add(SpriteFactory.create(layer, new Stone(new Position(x,game.grid().height()))));
                }
            }
            sprites.add(SpriteFactory.create(layer, new Stone(new Position(game.grid().width(),game.grid().height())))); // la case du coin
        }
    }

    void buildAndSetGameLoop() {
        gameLoop = new AnimationTimer() {
            public void handle(long now) {
                // Check keyboard actions
                processInput();

                // Do actions
                update(now);
                game.player().checkCollision(now);
                checkExplosions(now);

                // Graphic update
                cleanupSprites();
                render();
                statusBar.update();

            }
        };
    }

    private void checkExplosions(long now) {
        //Vérification de l'explosion d'une bombe
        for(int level=0;level<game.nbLevel();level++){
            for (Bomb bomb : game.listBombs(level)){
                bomb.getExplosionTimer().update(now);
                if (bomb.getExplosionTimer().remaining() <= 0){
                    Position bombPos = bomb.getPosition();

                    //Gestion des dégâts de la bombe
                    bomb.bombExplode(now);
                    int rangeUp = bomb.bombExlpoRangeDir(UP,now);
                    int rangeDown = bomb.bombExlpoRangeDir(DOWN,now);
                    int rangeLeft = bomb.bombExlpoRangeDir(LEFT,now);
                    int rangeRight = bomb.bombExlpoRangeDir(RIGHT,now);

                    //Suppression de la bombe
                    game.listBombs(level).remove(bomb);
                    bomb.remove();
                    //Animation de la bombe
                    if(bomb.getLevel()==game.level()) {
                        animateExplosion(bomb.getPosition(),bomb.getPosition());
                        animateExplosion(bomb.getPosition(), UP.nextPosition(bombPos, rangeUp));
                        animateExplosion(bomb.getPosition(), DOWN.nextPosition(bombPos, rangeDown));
                        animateExplosion(bomb.getPosition(), LEFT.nextPosition(bombPos, rangeLeft));
                        animateExplosion(bomb.getPosition(), RIGHT.nextPosition(bombPos, rangeRight));
                    }
                    //On ajoute une nouvelle bombe au joueur
                    game.player().addNbBomb(1);
                    break;
                }
            }
        }
    }



    private void animateExplosion(Position src, Position dst) {
        //Animation de la bombe
        ImageView explosion = new ImageView(ImageResource.EXPLOSION.getImage());
        TranslateTransition tt = new TranslateTransition(Duration.millis(200), explosion);
        tt.setFromX(src.x() * Sprite.size);
        tt.setFromY(src.y() * Sprite.size);
        tt.setToX(dst.x() * Sprite.size);
        tt.setToY(dst.y() * Sprite.size);
        tt.setOnFinished(e ->
            layer.getChildren().remove(explosion)
        );
        layer.getChildren().add(explosion);
        tt.play();
    }

    private void createNewBombs() {
        // Create a new Bomb is needed
        Bomb bomb= game.createBomb();
        if(bomb!=null){
            sprites.add(new SpriteBombs(layer,bomb));
        }
    }

    private void processInput() {
        //Vérification des actions saisie par le joueur
        if (input.isExit()) {
            gameLoop.stop();
            Platform.exit();
            System.exit(0);
        } else if (input.isMoveDown()) {
            player.requestMove(DOWN);
        } else if (input.isMoveLeft()) {
            player.requestMove(LEFT);
        } else if (input.isMoveRight()) {
            player.requestMove(Direction.RIGHT);
        } else if (input.isMoveUp()) {
            player.requestMove(UP);
        }else if(input.isKey() && player.getCle()>=1 && game.grid().get(player.getDirection().nextPosition(player.getPosition()))!=null){
            //Ouverture de la porte
            DoorNextOpened newDoor = game.grid().get(player.getDirection().nextPosition(player.getPosition())).openDoor(game);
            if(newDoor!=null){
                sprites.add(SpriteFactory.create(layer, newDoor));
            }
        } else if (input.isBomb()){
            createNewBombs();
        }

        input.clear();
    }

    private void showMessage(String msg, Color color) {
        //Affichage d'un message
        Text waitingForKey = new Text(msg);
        waitingForKey.setTextAlignment(TextAlignment.CENTER);
        waitingForKey.setFont(new Font(60));
        waitingForKey.setFill(color);
        StackPane root = new StackPane();
        root.getChildren().add(waitingForKey);
        Scene scene = new Scene(root, 400, 200, Color.WHITE);
        stage.setScene(scene);
        input = new Input(scene);
        stage.show();
        new AnimationTimer() {
            public void handle(long now) {
                processInput();
            }
        }.start();
    }


    private void update(long now) {
        //Update des monstres, des bombes dans tous les levels
        for(int level=0;level<game.nbLevel();level++){
            for (Bomb bomb : game.listBombs(level)){
                bomb.update(now);
            }
            for(var monster: game.listMonster(level)){
                monster.updateMonster(now);
            }
        }

        player.update();


        //Fin du jeu
        if (player.notIsAlive()) {
            gameLoop.stop();
            showMessage("You lose !", Color.RED);
        }
        if (game.grid().get(player.getPosition()) !=null && game.grid().get(player.getPosition()).isPrincess()) {
            gameLoop.stop();
            showMessage("You win !", Color.GREEN);
        }

        //Changement de level
        if(player.getChangeLevel()!=0){
            player.changeLevel();
            recreateSprite();
        }

        //Collision du joueur avec une box
        if (player.getBoxCollision()){
            player.setBoxCollision(false);
            Position boxPos = player.getDirection().nextPosition(player.getPosition());
            Box newBox = new Box(boxPos);
            game.grid().set(boxPos, newBox);
            sprites.add(SpriteFactory.create(layer, newBox));
            newBox.setModified(true);
        }

    }

    public void cleanupSprites() {
        //Changement des sprites modifiés
        sprites.forEach(sprite -> {
            if (sprite.getGameObject().isDeleted()) {
                game.grid().remove(sprite.getPosition());
                cleanUpSprites.add(sprite);
            }
        });
        cleanUpSprites.forEach(Sprite::remove);
        sprites.removeAll(cleanUpSprites);
        cleanUpSprites.clear();
    }

    private void render() {
        //Rendu des sprites
        for(Sprite s : sprites){
            s.render();
        }
    }

    public void start() {
        gameLoop.start();
    }
}