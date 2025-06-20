import networkx as nx
import matplotlib.pyplot as plt
import random
from tikzplotlib import get_tikz_code

#*************************************afficherGraphe*******************************************************#
def afficherGraphe(G,sommets=False,nom_sommet=False,poids=False,couleur=False,export_png=False,
                   export_tikz=False,nom="diamant_azteque",block=True):
    """
        Fonction qui affiche un graphe avec différentes options.

        :param G: le graphe à afficher (objet de type networkx.Graph).
        :param sommets: si True, affiche les sommets du graphe (booléen).
        :param nom_sommet: si True, affiche le nom des sommets (booléen).
        :param poids: si True, affiche le poids des arêtes (booléen).
        :param couleur: si True, colore les arêtes selon leur direction (booléen).
        :param export_png: si True, exporte le graphe en format PNG (booléen).
        :param export_tikz: si True, exporte le graphe en format TikZ (booléen).
        :param nom: le nom du fichier d'export (str).
        :param block: si True, ne passe pas à l'instruction suivante sans avoir fermer
        l'affichage (booléen).
    """
    plt.close()
    pos = nx.get_node_attributes(G, "pos")
    arete = G.edges()
    plt.axis('equal')
    plt.axis('off')
    if(sommets):
        x, y = zip(*pos.values())
        plt.scatter(x, y, c='black')
    if(nom_sommet):
        nx.draw_networkx_labels(G, pos,font_size=12, font_color='black', font_weight='bold', 
                                verticalalignment='bottom', horizontalalignment='center', alpha=1)
    if(couleur):
        tab_couleur = ["green" if x[0][0] == x[1][0] else "blue" for x in arete]
    else:
        tab_couleur = "black"
    nx.draw_networkx_edges(G, pos, edgelist=arete, edge_color=tab_couleur)
    if(poids):
        etiquette_arete = nx.get_edge_attributes(G,'poids')
        nx.draw_networkx_edge_labels(G, pos, edge_labels=etiquette_arete)
    if(export_png):
        plt.savefig(nom+".png",dpi=300)
    if(export_tikz):
        tikz_code = get_tikz_code(axis_width='6cm',axis_height='6cm')
        with open(nom+'.tex', 'w') as file:
            file.write(tikz_code)
    plt.show(block=block)


#*************************************creeDiamantAteque****************************************************#
def creeDiamantAteque(n):
    """
    Fonction qui crée un graphe représentant un diamant aztèque d'ordre n.
    
    :param n: entier positif représentant l'ordre du diamant.
    :return: un objet de type `networkx.Graph` représentant le diamant aztèque.
    
    Les sommets sont représentés par des tuples correspondant à leurs coordonées dans le plan.
    """
    G = nx.Graph()
    for y in range(n,-n-1,-1):
        for x in range(-n,n+1):
            if(abs(x-1/2)+abs(y-1/2)<=n):
                G.add_node((x,y),pos=(x,y))
    return G

#*************************************ajoutAretes**********************************************************#
def ajoutAretes(G):
    """
    Fonction qui ajoute les arêtes entre les sommets du graphe `G` si deux sommets ont une distance euclidienne de 1 
    en prenant en compte leurs coordonnées dans le plan.
    
    :param G: un objet de type `networkx.Graph` représentant le diamant aztèque.
    
    Les arêtes ajoutées ont une pondération de 1.
    """
    liste_sommets = list(G.nodes())
    for x in range (len(liste_sommets)):
        for y in range (x+1,len(liste_sommets)):
            if (abs(liste_sommets[x][0]-liste_sommets[y][0])+abs(liste_sommets[x][1]-
                                                                 liste_sommets[y][1])==1):
                G.add_edge(liste_sommets[x],liste_sommets[y],poids=1)


                        
#*************************************calculPoids**********************************************************#
def calculPoids(graphePondere,n):
    """
    Calcule les poids des arêtes pour chaque diamant aztèque d'ordre allant de 1 à n.

    :param graphePondere: un objet de type `networkx.Graph` représentant le graphe pondéré initial.
    :param n: entier positif représentant l'ordre du plus grand diamant aztèque.
    :return: une liste de graphes, où le i-ème élément correspond au diamant aztèque d'ordre i.
    """
    tabGraphe = []
    for i in range(n,0,-1):
        tabGraphe.append(creeDiamantAteque(i))
        for v in range(i,-i-1,-1):
            for u in range(-i,i+1):
                a = (u,v)
                b = (u+1,v)
                c = (u,v-1)
                d = (u+1,v-1)
                if((u+v)%2 == i%2 and tabGraphe[n-i].has_node(a) and tabGraphe[n-i].has_node(b) and
                    tabGraphe[n-i].has_node(c) and tabGraphe[n-i].has_node(d)):
                    w = graphePondere.get_edge_data(a,b)['poids']
                    y = graphePondere.get_edge_data(a,c)['poids']
                    x = graphePondere.get_edge_data(b,d)['poids']
                    z = graphePondere.get_edge_data(c,d)['poids']
                    tabGraphe[n-i].add_edge(a,b, poids=z/(w*z+x*y))
                    tabGraphe[n-i].add_edge(a,c, poids=x/(w*z+x*y))
                    tabGraphe[n-i].add_edge(b,d, poids=y/(w*z+x*y))
                    tabGraphe[n-i].add_edge(c,d, poids=w/(w*z+x*y))
    return tabGraphe

#*************************************changerPoids*********************************************************#
def changerPoidsUneArete(G,sommets,nouveauPoids):
    """
        Change le poids de l'arrête entre les sommets d'un graphe G avec le poids donné dans 
        nouveauPoids.
        
        :param G: objet de type `networkx.Graph` représentant le graphe.
        :param nouveauPoids: un dictionnaire contenant les nouveaux poids des arêtes sous la forme 
        (sommet1, sommet2): poids.
    """
    (sommet1,sommet2) = sommets
    if(not G.has_node(sommet1)):
        print("Erreur, le sommet "+str(sommet1)+" n'existe pas")
    elif (not G.has_node(sommet2)):
        print("Erreur, le sommet "+str(sommet2)+" n'existe pas")
    elif(not G.has_edge(sommet1,sommet2)):
        print("Erreur, l'arête ("+str(sommet1)+","+str(sommet2)+") n'existe pas")
    else :
        G.get_edge_data(sommet1,sommet2)['poids'] = nouveauPoids

#*************************************changerPoids*********************************************************#
def changerPoidsTousAretes(G,nouveauxPoids):
    """
        Change les poids des arêtes d'un graphe G avec les poids donnés dans nouveauxPoids.
        
        :param G: objet de type `networkx.Graph` représentant le graphe.
        :param nouveauxPoids: un dictionnaire contenant les nouveaux poids des arêtes sous la forme 
        (sommet1, sommet2): poids.
    """
    for (sommet1,sommet2) in nouveauxPoids:
        if(not G.has_node(sommet1)):
            print("Erreur, le sommet "+str(sommet1)+" n'existe pas")
        elif (not G.has_node(sommet2)):
            print("Erreur, le sommet "+str(sommet2)+" n'existe pas")
        elif(not G.has_edge(sommet1,sommet2)):
            print("Erreur, l'arête ("+str(sommet1)+","+str(sommet2)+") n'existe pas")
        else :
            G.get_edge_data(sommet1,sommet2)['poids'] = nouveauxPoids[(sommet1,sommet2)]

#*************************************shufflingDomino******************************************************#
def shufflingDomino(tabGraphe,n):
    """
        Fonction qui crée une configuration aléatoire de dominos dans un diamant aztèque d'ordre n en
        utilisant l'algorithme de mélange.
        
        :param tabGraphe: liste de graphes pondérés représentant les diamants aztéques d'ordre 1 à n
        :param n: entier positif représentant l'ordre du diamant.
        :return: un objet de type `networkx.Graph` représentant la configuration aléatoire de dominos.
        
        Les sommets sont représentés par des tuples correspondant à leur coordonée dans le plan.
    """
    G = creeDiamantAteque(n)
    for i in range(1,n+1):
        H = creeDiamantAteque(n)
        for v in range(i,-i-1,-1):
            for u in range(-i,i+1):
                a = (u,v)
                b = (u+1,v)
                c = (u,v-1)
                d = (u+1,v-1)
                #Vérification si a est le coin supérieur gauche d'une cellule
                if((u+v)%2 == i%2 and tabGraphe[n-i].has_node(a) and tabGraphe[n-i].has_node(b) and 
                   tabGraphe[n-i].has_node(c) and tabGraphe[n-i].has_node(d)):
                    #Phase de destruction
                    if G.has_edge(a,b) and G.has_edge(c,d) or G.has_edge(a,c) and G.has_edge(b,d):
                        continue
                    #Phase de glissement
                    elif G.has_edge(a,b):
                        H.add_edge(c,d)
                    elif G.has_edge(a,c):
                        H.add_edge(b,d)
                    elif G.has_edge(b,d):
                        H.add_edge(a,c)
                    elif G.has_edge(c,d):
                        H.add_edge(a,b)
                    #Phase de creation
                    else:
                        abxcd = (tabGraphe[n-i].get_edge_data(a,b)['poids']*
                                 tabGraphe[n-i].get_edge_data(c,d)['poids'])
                        acxbd = (tabGraphe[n-i].get_edge_data(a,c)['poids']*
                                 tabGraphe[n-i].get_edge_data(b,d)['poids'])
                        proba = abxcd / (abxcd+acxbd)
                        if random.random() < proba :
                            H.add_edge(a,b)
                            H.add_edge(c,d)
                        else:
                            H.add_edge(a,c)
                            H.add_edge(b,d)
        G = H.copy()
        H.clear()
    return G

#*************************************pavageDiamantAzteque*************************************************#
def pavageDiamantAzteque(n,nouveauxPoids={},export_png=False,export_tikz=False,
                         nom="diamant_azteque"):
    """
        Cette fonction permet de créer et d'afficher un pavage de diamants aztèques d'ordre 1 à n.

        :param n: entier positif représentant l'ordre du plus grand diamant aztèque dans le pavage.
        :param nouveauxPoids: un dictionnaire optionnel permettant de changer les poids des arêtes du graphe
        initial. Les clés sont des paires de sommets et les valeurs sont les nouveaux poids.
        :param export_png: booléen optionnel indiquant s'il faut exporter le pavage au format png ou non.
        La valeur par défaut est False.
        :param export_tikz: booléen optionnel indiquant s'il faut exporter le pavage au format tikz ou non. 
        La valeur par défaut est False.
        :param nom: une chaîne de caractères optionnelle représentant le nom du fichier d'export. La valeur 
        par défaut est "diamant_azteque".

    """
    graphePondere = creeDiamantAteque(n)
    ajoutAretes(graphePondere)
    changerPoidsTousAretes(graphePondere,nouveauxPoids)
    tabGraph = calculPoids(graphePondere,n)
    G = shufflingDomino(tabGraph,n)
    afficherGraphe(G,couleur=True,export_png=export_png,export_tikz=export_tikz,nom=nom)

#*************************************destruction**********************************************************#
def destruction(G,n):
    """
        Détruit les arêtes qui sont dans une même cellule du graphe G.

        :param G: un objet de type `networkx.Graph` représentant le graphe initial.
        :param n: entier positif représentant l'ordre du diamant aztèque.
        :return: un nouvel objet de type `networkx.Graph` correspondant au graphe obtenu après destruction.
    """
    H = G.copy()
    for v in range(n,-n-1,-1):
        for u in range(-n,n+1):
            a = (u,v)
            b = (u+1,v)
            c = (u,v-1)
            d = (u+1,v-1)
            #Vérification si a est le coin supérieur gauche d'une cellule
            if((u+v)%2 == n%2 and G.has_node(a) and G.has_node(b) and G.has_node(c) and G.has_node(d)):
                if G.has_edge(a,b) and G.has_edge(c,d):
                    H.remove_edge(a,b)
                    H.remove_edge(c,d)
                if G.has_edge(a,c) and G.has_edge(b,d):
                    H.remove_edge(a,c)
                    H.remove_edge(b,d)
    return H

#*************************************glissement***********************************************************#
def glissement(G,n):
    """
        Effectue un glissement des arrêtes étant seul dans une cellule du graphe G.

        :param G: un objet de type `networkx.Graph` représentant le diamant aztèque d'ordre n.
        :param n: entier positif représentant l'ordre du diamant aztèque.
        :return: un nouvel objet de type `networkx.Graph` représentant le diamant aztèque d'ordre n après le glissement.
    """
    H = G.copy()
    for v in range(n,-n-1,-1):
        for u in range(-n,n+1):
            a = (u,v)
            b = (u+1,v)
            c = (u,v-1)
            d = (u+1,v-1)
            #Vérification si a est le coin supérieur gauche d'une cellule
            if((u+v)%2 == n%2 and G.has_node(a) and G.has_node(b) and G.has_node(c) and G.has_node(d)):
                if G.has_edge(a,b):
                    H.remove_edge(a,b)
                    H.add_edge(c,d)
                elif G.has_edge(a,c):
                    H.remove_edge(a,c)
                    H.add_edge(b,d)
                elif G.has_edge(b,d):
                    H.remove_edge(b,d)
                    H.add_edge(a,c)
                elif G.has_edge(c,d):
                    H.remove_edge(c,d)
                    H.add_edge(a,b)
    return H

#*************************************creation*************************************************************#
def creation(G,tabGraphe,n,i):
    """
        Remplit les cellules qui sont vides du graphe G

        :param G: un objet de type `networkx.Graph` représentant le graphe pondéré initial.
        :param tabGraphe: une liste de graphes, où le i-ème élément correspond au diamant aztèque d'ordre i.
        :param n: entier positif représentant l'ordre du plus grand diamant aztèque.
        :param i: entier positif représentant l'ordre du diamant aztèque à créer.
        :return: un objet de type `networkx.Graph` représentant le diamant aztèque d'ordre i.
    """
    H = creeDiamantAteque(i)
    for v in range(i,-i-1,-1):
        for u in range(-i,i+1):
            a = (u,v)
            b = (u+1,v)
            c = (u,v-1)
            d = (u+1,v-1)
            #Vérification si a est le coin supérieur gauche d'une cellule
            if((u+v)%2 == i%2 and G.has_node(a) and G.has_node(b) and G.has_node(c) and G.has_node(d)):
               if(not G.has_edge(a,b) and not G.has_edge(c,d) and not G.has_edge(a,c) and not G.has_edge(b,d)):
                    abxcd = (tabGraphe[n-i].get_edge_data(a,b)['poids']*
                                 tabGraphe[n-i].get_edge_data(c,d)['poids'])
                    acxbd = (tabGraphe[n-i].get_edge_data(a,c)['poids']*
                                 tabGraphe[n-i].get_edge_data(b,d)['poids'])
                    proba = abxcd / (abxcd+acxbd)
                    if random.random() < proba :
                        H.add_edge(a,b)
                        H.add_edge(c,d)
                    else:
                        H.add_edge(a,c)
                        H.add_edge(b,d)
    return H


#*************************************pavageDiamantAztequeAvecEtape****************************************#
def pavageDiamantAztequeAvecEtape(n,nouveauxPoids={},export_png=False,export_tikz=False,
                                   nom="diamant_azteque"):
    """
        Cette fonction permet de générer un pavage de diamants aztèques, étape par étape, en utilisant les 
        fonctions de destruction, de glissement et de création.

        :param n: entier positif représentant l'ordre du plus grand diamant aztèque.
        :param nouveauxPoids: un dictionnaire représentant les poids des arêtes à modifier dans le graphe 
        initial.
        :param export_png: un booléen indiquant si le graphe doit être exporté au format png à chaque étape.
        :param export_tikz: un booléen indiquant si le graphe doit être exporté au format tikz à chaque étape.
        :param nom: une chaîne de caractères représentant le nom du fichier d'export.
    """ 
    # Création du graphe initial
    graphePondere = creeDiamantAteque(n)
    ajoutAretes(graphePondere)
    changerPoidsTousAretes(graphePondere, nouveauxPoids)
    tabGraph = calculPoids(graphePondere, n)

    # Initialisation des variables
    iNom = 0
    G = creeDiamantAteque(1)
    G = creation(G, tabGraph,1,1)

    # Affichage du premier diamant aztèque
    afficherGraphe(G, couleur=True, export_png=export_png, export_tikz=export_tikz, nom=nom+str(iNom),sommets=True)
    iNom += 1

    # Génération des diamants aztèques étape par étape
    for i in range(2, n+1):
        # Création d'un nouveau diamant aztèque
        H = creeDiamantAteque(i)

        # Ajout du nouveau diamant aztèque au graphe
        G = nx.compose(G, H)
        # Destruction et affichage du sous-graphe
        A = destruction(G,i)
        afficherGraphe(A, couleur=True, export_png=export_png, export_tikz=export_tikz, nom=nom+str(iNom),sommets=True)
        iNom += 1

        # Glissement et affichage du sous-graphe
        B = glissement(A,i)
        afficherGraphe(B, couleur=True, export_png=export_png, export_tikz=export_tikz, nom=nom+str(iNom),sommets=True)
        iNom += 1

        # Création et affichage du nouveau diamant aztèque
        C = creation(G, tabGraph,n,i)
        G.clear()
        G = nx.compose(B, C)
        afficherGraphe(G, couleur=True, export_png=export_png, export_tikz=export_tikz, nom=nom+str(iNom),sommets=True)
        iNom += 1


#Exemple de différents cas d'utilisation

#Cas de génération d'un diamant aztèque d'ordre 50 avec juste l'affichage à l'écran (il faut fermer la 
# fenêtre pour continuer)
#pavageDiamantAzteque(50)

#Cas de génération d'un diamant aztèque d'ordre 50 avec l'affichage à l'écran et l'export en format tikz
#pavageDiamantAzteque(50,export_tikz=True)

#Cas de génération d'un diamant aztèque d'ordre 3 avec changement des poids des arêtes avec l'affichage à 
# l'écran
#nouveauxPoids = {((0,3),(0,2)):0,((1,3),(1,2)):0,((-2,1),(-1,1)):0,((-2,0),(-1,0)):0,((2,1),(3,1)):0,((2,0),(3,0)):0,((0,-1),(0,-2)):0,((1,-1),(1,-2)):0}
#pavageDiamantAzteque(3,nouveauxPoids=nouveauxPoids)

#Cas de génération d'un diamant aztèque d'ordre 3 avec affichage de tous les étapes de la génération
#pavageDiamantAztequeAvecEtape(3,export_png=True)