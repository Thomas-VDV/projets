package fr.ubx.poo.ubomb.launcher;

import fr.ubx.poo.ubomb.Pair;
import fr.ubx.poo.ubomb.game.Configuration;
import fr.ubx.poo.ubomb.game.Game;
import fr.ubx.poo.ubomb.game.Level;
import fr.ubx.poo.ubomb.game.Position;
import java.io.IOException;
import java.io.Reader;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

public class GameLauncher {

    public static Game load() {
        //On crée une liste pour savoir où sont les monstres, dans le jeu par défaut, il n'y a qu'un niveau, et on met la place du monstre manuellement
        List<List<Position>> listPositionMonster = new ArrayList<>();
        listPositionMonster.add(new ArrayList<>());
        listPositionMonster.get(0).add(new Position(0,9));

        //On crée la configuration du jeu
        Configuration configuration = new Configuration(new Position(0, 0), 3, 5, 4000, 1, 1000,listPositionMonster);

        //On crée le level, et le tableau de level
        Level[] tabLevel = new Level[1];
        tabLevel[0] = new Level(new MapLevelDefault());
        return new Game(configuration, tabLevel);
    }

    public static Game load(Reader in) throws IOException {
        Properties config = new Properties();
        config.load(in);

        //On récupère les données qui sont dans le fichier
        boolean compression = Boolean.parseBoolean(config.getProperty("compression", "false"));
        int nbLevel = integerProperty(config,"levels",1);
        int playersLives = integerProperty(config,"playerLives",5);
        int monsterVelocity = integerProperty(config,"monsterVelocity",5);
        int playerInvisibilityTime = integerProperty(config,"playerInvisibilityTime",4000);
        int monsterInvisibilityTime = integerProperty(config,"monsterInvisibilityTime",1000);
        Position positionPlayer = stringToPosition(config.getProperty("player", "0x0"));

        Level[] tabLevel = new Level[nbLevel];
        List<List<Position>> listPositionMonster = new ArrayList<>();

        //Pour chaque level du jeu, on récupère les données de la map et la liste des positions des monstres
        for(int x=1;x<=nbLevel;x++){
            StringBuilder s = new StringBuilder();
            s.append("level");
            s.append(x);
            if(compression){
                //Si les maps sont codés avec une compression, on appelle StringRLEMap
                Pair<MapLevel,List<Position>> tmp = StringRLEtoMap(config.getProperty(s.toString(), ""));
                tabLevel[x-1] = new Level(tmp.value1());
                listPositionMonster.add(x-1,tmp.value2());
            }else{
                //Si les maps sont codés sans compression, on appelle StringtoMap
                Pair<MapLevel,List<Position>> tmp = StringtoMap(config.getProperty(s.toString(), ""));
                assert tmp != null;
                tabLevel[x-1] = new Level(tmp.value1());
                listPositionMonster.add(x-1,tmp.value2());
            }

        }

        Configuration configuration = new Configuration(positionPlayer, 3, playersLives, playerInvisibilityTime, monsterVelocity, monsterInvisibilityTime,listPositionMonster);
        return new Game(configuration,tabLevel);
    }

    private static int integerProperty(Properties config, String name, int defaultValue) {
        //Permet de récupérer un int dans un fichier
        return Integer.parseInt(config.getProperty(name, Integer.toString(defaultValue)));
    }

    public static Position stringToPosition(String pos){
        //Permet de récupérer une position dans un fichier
        return new Position(Integer.parseInt(String.valueOf(pos.charAt(0))),Integer.parseInt(String.valueOf(pos.charAt(2))));
    }

    private static Pair<MapLevel,List<Position>>  StringRLEtoMap(String string){
        //On calcule la hauteur et la largeur
        int height = 0;
        int width=0;
        int iString = 0;
        char tmp = string.charAt(iString);
        while(tmp!='x'){

            if(Character.isDigit(tmp)){
                height = height -1+ (tmp-'0');

            }else{
                height ++;
            }
            iString++;
            tmp = string.charAt(iString);
        }

        for(int i=0;i<string.length();i++){
            if(string.charAt(i)=='x'){
                width++;
            }
        }

        //On ajoute les éléments dans la map
        iString=0;
        MapLevel level = new MapLevel(width,height);
        List<Position> positionMonstre = new ArrayList<>();
        for(int i=0;i<width;i++){
            int cpt=0;
            for(int j=0;j<height;j++){
                tmp =string.charAt(iString);
                if(cpt!=0){
                    //Cas si on est en train de decoder à partir d'un chiffre
                    level.set(i,j, Entity.fromCode(string.charAt(iString-2)));
                    if(Entity.fromCode(string.charAt(iString-2))==Entity.Monster){
                        positionMonstre.add(new Position(i,j));
                    }
                    cpt--;
                }else {
                    if(Character.isDigit(tmp)){
                        //Cas si on a chiffre
                        cpt= tmp - '0' -2;
                        level.set(i,j, Entity.fromCode(string.charAt(iString-1)));
                        if(Entity.fromCode(string.charAt(iString-1))==Entity.Monster){
                            //Si on trouve un monstre, on récupère sa position
                            positionMonstre.add(new Position(i,j));
                        }
                        iString++;
                    }else{
                        //Cas si on a une lettre
                        level.set(i,j, Entity.fromCode(string.charAt(iString)));
                        if(Entity.fromCode(string.charAt(iString))==Entity.Monster){
                            //Si on trouve un monstre, on récupère sa position
                            positionMonstre.add(new Position(i,j));
                        }
                        iString++;
                    }
                }
            }
            iString++;


        }
        //On renvoie une MapLevel, ainsi qu'une liste de position de monstre
        return new Pair<>(level,positionMonstre);
    }

    public static Pair<MapLevel,List<Position>> StringtoMap(String string) {
        if(string.length()==0) {
            return null;
        }
        int iString=0;
        int height =0;

        //On calcule la hauteur et la largeur
        while(string.charAt(iString)!='x'){
            iString++;
            height++;
        }
        int width = string.length()/(height+1);
        iString=0;
        MapLevel level = new MapLevel(width,height);
        List<Position> positionMonstre = new ArrayList<>();
        //On décode la chaine de caractère
        for(int j=0;j<width;j++) {
            for(int i=0;i<height;i++){
                char tmp = string.charAt(iString);
                level.set(i,j, Entity.fromCode(tmp));
                if(Entity.fromCode(tmp)==Entity.Monster){
                    //Si on trouve un monstre, on récupère sa position
                    positionMonstre.add(new Position(i,j));
                }
                iString++;
            }

            iString++;
        }
        //On renvoie une MapLevel, ainsi qu'une liste de position de monstre
        return new Pair<>(level,positionMonstre);
    }
}
