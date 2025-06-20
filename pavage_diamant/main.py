import argparse
import pavage_diamant_azteque as pda

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("n", type=int, help="entier positif représentant l'ordre du diamant aztèque.")
    parser.add_argument("-e", "--export_png", action="store_true", help="un booléen indiquant si le graphe doit être exporté au format png à chaque étape.")
    parser.add_argument("-t", "--export_tikz", action="store_true", help="un booléen indiquant si le graphe doit être exporté au format tikz à chaque étape.")
    parser.add_argument("-o", "--nom_fichier", type=str, default="diamant_azteque", help="une chaîne de caractères représentant le nom du fichier d'export.")
    parser.add_argument("-i", "--nombre_pavage", type=int, default="1", help="un entier indiquant le nombre de pavage aléatoire tiré.")
    args = parser.parse_args()

    #Affichage du graphe pour connaître le nom des sommets
    graphePondere = pda.creeDiamantAteque(args.n)
    pda.ajoutAretes(graphePondere)
    pda.afficherGraphe(graphePondere,sommets=True,nom_sommet=True,block=False,poids=True)
    while True :
        reponse = input("Entrez le poids à changer sous la forme '(sommet1,sommet2):poids' ou tapez 'ok' pour terminer : ")
        if reponse.lower() == "ok":
            break
        try:
            sommets, poids_nouveau = reponse.split(":")
            nouveauPoids = float(poids_nouveau)
            sommets = eval(sommets)  # Convertir la chaîne de caractères en tuple
            pda.changerPoidsUneArete(graphePondere,sommets,nouveauPoids)
            pda.afficherGraphe(graphePondere,sommets=True,nom_sommet=True,block=False,poids=True)
        except (ValueError, SyntaxError):
            print("Erreur: Entrez une réponse valide sous la forme '(sommet1,sommet2):poids'.")
        
    tabGraph = pda.calculPoids(graphePondere,args.n)
    for i in range(args.nombre_pavage):
        G = pda.shufflingDomino(tabGraph,args.n)
        pda.afficherGraphe(G,couleur=True,export_png=args.export_png,export_tikz=args.export_tikz,nom=args.nom_fichier)  
    