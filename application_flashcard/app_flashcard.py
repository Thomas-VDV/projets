import sqlite3
from datetime import *
import json
from colorama import Fore, Style
import shutil
import os

############# CREATION_DATATBASE #############
def creation_database(conn,cursor):
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS decks (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        nb_new_cards_day INTEGER NOT NULL,
        date_reached TEXT NULL
    )
    ''')

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS flashcards (
        id INTEGER PRIMARY KEY,
        deck_id INTEGER NOT NULL,
        data TEXT NOT NULL,
        new INTEGER NOT NULL CHECK (new IN (0, 1)), 
        last_review_date TEXT NOT NULL,
        next_review_date TEXT NOT NULL,
        FOREIGN KEY (deck_id) REFERENCES decks (id)
    )
    ''')

    conn.commit()

############# UPDATE_DATABASE #############
def update_database(conn,cursor):
    cursor.execute('SELECT * FROM decks')
    decks = cursor.fetchall()
    if(decks == None):
        return
    
    today = datetime.now().strftime('%Y-%m-%d')

    for deck in decks:
        if(deck[3] != today):
            cursor.execute('''
            UPDATE decks
            SET nb_new_cards_day = ?, date_reached = ?
            WHERE id = ?
            ''', (20,today,deck[0]))

############# SAVE_DATABASE #############
def save_database():
    shutil.copyfile("flashcards.db", ".flashcards.db")

############# RESTORE #############
def restore_database():
    response = input("Are you sure you want to restore the database at the last application launch ? (y/n) ")
    print()

    if(response != "y"):
        return
    
    shutil.copyfile(".flashcards.db","flashcards.db")

############# GET_DECK_ID_BY_NAME #############
def get_deck_id_by_name(cursor,deck):
    cursor.execute('SELECT id FROM decks WHERE name = ?', (deck,))
    deck_id = cursor.fetchone() 
    if deck_id:
        return deck_id[0]
    else:
        return None  

############# CREATE_DECK_NOT_EXIST #############
def create_deck_not_exist(conn,cursor):
    name = input(f"Name the deck : ").strip().lower()

    if(name == ''):
        print("The entry is empty\n")
        return

    deck_id = get_deck_id_by_name(cursor,name)
    if(deck_id != None):
        print("The deck already exists")
        return -1

    cursor.execute('INSERT INTO decks (name,nb_new_cards_day,date_reached) VALUES (?,?,?)', (name,20,datetime.now().strftime('%Y-%m-%d')))
    conn.commit() 

    deck_id = get_deck_id_by_name(cursor,name)
    return deck_id

############# SELECT_DECK #############
def select_deck(conn,cursor,new_deck=False,have_card_to_review=False):
    cursor.execute('''
        SELECT * FROM decks
        ''',)   
    decks = cursor.fetchall()

    print("Select : (1/2/3/...)")
    for i in range(1,len(decks)+1):
        if(have_card_to_review):
            cards_to_review = get_cards_to_review(conn,cursor,decks[i-1][0])
            if(cards_to_review == None):
                print(" "+str(i)+" - "+decks[i-1][1]+" (no card to review)")
            else :
                print(" "+str(i)+" - "+decks[i-1][1]+" ("+str(len(cards_to_review))+" card to review)")
                
        else :
            print(" "+str(i)+" - "+decks[i-1][1])

    
    if(new_deck):
        print(" "+str(len(decks)+1)+" - New deck")

    response = input(f"You choose : ").strip().lower()
    if(response == ''):
        print("The entry is empty\n")
        return -1
    print()

    try:
        if(int(response) == len(decks) + 1 and new_deck):
            return create_deck_not_exist(conn,cursor)
    except:
        return -1

    if(int(response) > len(decks)):
        print("The number is not correct")
        print()
        return -1
    
    response_2 = input(f"You've chosen the deck {response}. Are you sure ? (y/n) ")
    print()
    if(response_2 == "y"):
        return decks[int(response)-1][0]
       
    return -1

############# GET_CARDS_TO_REVIEW #############
def get_cards_to_review(conn,cursor,deck_id):
    cursor.execute('''
    SELECT * FROM decks
    WHERE id = ? 
    ''', (deck_id,))
    deck = cursor.fetchone()
    if(deck[2] <= 0):
        return

    today = datetime.now().strftime('%Y-%m-%d')
    
    cursor.execute('''
    SELECT * FROM flashcards
    WHERE deck_id = ? AND next_review_date <= ? AND new = 0
    ''', (deck_id, today))    
    cards_to_review = cursor.fetchall()

    cursor.execute('''
    SELECT * FROM flashcards
    WHERE deck_id = ? AND new = 1
    LIMIT ?
    ''', (deck_id,deck[2]))
    new_cards = cursor.fetchall()

    cards_to_review.extend(new_cards)
    return cards_to_review

############# UPDATE_NEXT_REVIEW_DATE #############
def update_next_review_date(conn,cursor,card,first_time):
    last_review_date = datetime.strptime(card[4], '%Y-%m-%d')
    today = datetime.now()

    nb_day_difference = (today - last_review_date).days
    offset = 1

    if( card[3] == 1 or not first_time):
        offset = 1

    elif( 1 <= nb_day_difference and nb_day_difference <= 6):
        offset = 7

    elif( 7 <= nb_day_difference and nb_day_difference <= 29):
        offset = 30

    elif( 30 <= nb_day_difference):
        offset = 180     

    new_review_date = today + timedelta(days=offset)
    cursor.execute('''
    UPDATE flashcards
    SET next_review_date = ?, new = 0
    WHERE id = ?
    ''', (new_review_date.strftime('%Y-%m-%d'), card[0]))
    conn.commit()

############# UPDATE_LIMIT #############
def update_limit(conn,cursor,deck_id):
    cursor.execute('''
    SELECT * FROM decks
    WHERE id = ? 
    ''', (deck_id,))
    deck = cursor.fetchall()

    cursor.execute('''
    UPDATE decks
    SET nb_new_cards_day = ?
    WHERE id = ?
    ''', (deck[0][2]-1,deck_id))

    conn.commit()

############# TEST_A_LIST #############
def test_a_list(conn,cursor,deck_id,cards_to_review,first_time):
    next_review = []
    for card in cards_to_review:
        dico = json.loads(card[2])

        print("Enter q for quit the test")

        fail = False
        for key in dico:
            print("What is the back of",end=" ")
            print(Fore.YELLOW + key + Style.RESET_ALL, end=" ")
            response = input(f"? ").strip().lower()

            if(response == "q"):
                print()
                return ([],False)

            if(response == dico[key]):
                print(Fore.GREEN + "It's correct ! Good Job !"+ Style.RESET_ALL)
            else :
                print(Fore.RED + "Too bad, the right answer is "+dico[key]+ Style.RESET_ALL)
                fail = True

        if(fail):
            next_review.append(card)

        if(not fail):
            update_next_review_date(conn,cursor,card,first_time)

        if(not fail and card[3]):
            update_limit(conn,cursor,deck_id)

        print()
    return (next_review,True)


############# TEST #############
def test(conn,cursor):
    deck_id = select_deck(conn,cursor,have_card_to_review=True)
    if(deck_id == -1):
        print("Error in deck selection\n")
        return

    cards_to_review = get_cards_to_review(conn,cursor,deck_id)
    if(cards_to_review == None):
        print("You have already review all cards for today !\n")
        return

    cards_to_review = test_a_list(conn,cursor,deck_id,cards_to_review,True)
    while(len(cards_to_review[0]) != 0 ):
        cards_to_review = test_a_list(conn,cursor,deck_id,cards_to_review[0],False)

    if(cards_to_review[1]):
        print("Good job ! You have review all card for today !\n")
    else:
        print("You're leaving us already ? You didn't work all the cards\n")

############# ADD_FLASHCARD #############
def add_flashcard(conn,cursor,deck_id, data,new,last_review_date, next_review_date):
    json_data = json.dumps(data)

    cursor.execute('''
    SELECT 1 FROM flashcards
    WHERE deck_id = ? AND data = ?
    ''', (deck_id,json_data))
    
    result = cursor.fetchone()
    if(result != None):
        return

    cursor.execute('''
    INSERT INTO flashcards (deck_id, data ,new,last_review_date, next_review_date)
    VALUES (?, ?, ?,?,?)
    ''', (deck_id, json_data,new,last_review_date,next_review_date))

    conn.commit()

############# ADD_CARD_MANUAL #############
def add_card_manual(conn,cursor):
    deck_id = select_deck(conn,cursor,True)
    if(deck_id == -1):
        print("Error in deck selection\n")
        return
    
    not_quit = True
    while(not_quit):
        nb_pairs = input(f"Enter the number of pairs on the card (or q to exit) : ")
        if(nb_pairs == 'q'):
            print()           
            break

        if(nb_pairs == ''):
            print("You don't have pass a number\n")
            break

        try:
            dico = {}
            for i in range(int(nb_pairs)):
                key = input(f"Request : ").strip().lower()
                if(key == ''):
                    print("The entry is empty\n")
                    return
                
                value = input(f"Answer : ").strip().lower()
                if(value == ''):
                    print("The entry is empty\n")
                    return

                dico.update({key: value})
        except:
            print("You don't have pass a number\n")
            break
        
        add_flashcard(conn,cursor,deck_id,dico,1,datetime.now().strftime('%Y-%m-%d'),datetime.now().strftime('%Y-%m-%d')) 
        print() 

############# ADD_CARD_FROM_FILE #############
def add_card_from_file(conn,cursor):
    file = input(f"Name of the file : ").strip().lower()

    if(not os.path.exists(file)):
        print("Error, the file name "+file+" doesn't exist\n")
        return

    deck_id = select_deck(conn,cursor,True)
    if(deck_id == -1):
        print("Error in deck selection\n")
        return

    with open(file, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if line.strip() == '':
                continue

            dico = {}
            datas = line.strip().split('/')
            for data in datas:
                parts = data.strip().split(':')
                if len(parts) != 2:
                    print(f"The line doesn't match the format : {line.strip()}")
                    continue

                key = parts[0].strip().lower()
                value = parts[1].strip().lower()
                dico.update({key: value})

            add_flashcard(conn,cursor,deck_id,dico,1,datetime.now().strftime('%Y-%m-%d'),datetime.now().strftime('%Y-%m-%d'))
    print() 

############# DELETE_CARD #############
def delete_card(conn,cursor):
    deck_id = select_deck(conn,cursor)
    if(deck_id == -1):
        print("Error in deck selection\n")
        return
    
    cursor.execute('''
        SELECT * FROM flashcards
        WHERE deck_id = ?
        ''',(deck_id,))   
    deck = cursor.fetchall()

    for card in deck:
        data = json.loads(card[2])
        print(str(card[0])+" - "+" / ".join(f"{key} : {value}" for key, value in data.items()))
    print()
    card_id = input("You choose the card number : ")

    cursor.execute('''
        SELECT * FROM flashcards
        WHERE id = ?
        ''',(int(card_id),))   
    card = cursor.fetchone()

    if(card == None):
        print("The number choose is not valid\n")
        return

    data = json.loads(card[2])
    response = input("You choose to delete the card "+card_id+" - "+" / ".join(f"{key} : {value}" for key, value in data.items())+". Are you sure ? (y/n) ")
    print()
    if(response == "n"):
        return
    
    cursor.execute('DELETE FROM flashcards WHERE id = ?', (card_id,))
    conn.commit()


############# DELETE_DECK #############
def delete_deck(conn,cursor):
    deck_id = select_deck(conn,cursor)
    if(deck_id == -1):
        print("Error in deck selection\n")
        return
    
    cursor.execute('DELETE FROM flashcards WHERE deck_id = ?', (deck_id,))
    cursor.execute('DELETE FROM decks WHERE id = ?', (deck_id,))
    conn.commit()

############# EXPORT_DECK #############
def export_deck(conn,cursor):
    deck_id = select_deck(conn,cursor)
    if(deck_id == -1):
        print("Error in deck selection\n")
        return

    name_file = input("Enter the name of the file : ")
    if(name_file == ''):
        print("The entry is empty\n")
        return
    
    name_file = name_file + ".txt"
    print()

    cursor.execute('''
    SELECT * FROM flashcards
    WHERE deck_id = ?
    ''',(deck_id,))   
    deck = cursor.fetchall()

    with open(name_file, 'a') as file:
        for card in deck:
            dico = json.loads(card[2])
            keys = list(dico.keys())
            string = ""

            for i in range(len(keys)-1):
                string += keys[i] + " : " + dico[keys[i]] + " / "
            
            key = keys[len(keys)-1]
            string += key + " : " + dico[key] + "\n"

            file.write(string)

############# DISPLAY_DATABASE #############
def display_database(cursor):
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    tables = cursor.fetchall()

    for table in tables:
        table_name = table[0]
        print(f"\nTable: {table_name}")

        cursor.execute(f"SELECT * FROM {table_name}")
        rows = cursor.fetchall()

        cursor.execute(f"PRAGMA table_info({table_name})")
        column_info = cursor.fetchall()
        column_names = [info[1] for info in column_info]
        
        print(" | ".join(column_names))
        print("-" * 40)

        for row in rows:
            print(" | ".join(str(item) for item in row))
    print()


############# MAIN #############
def main():
    conn = sqlite3.connect('flashcards.db')
    cursor = conn.cursor()

    creation_database(conn,cursor)
    update_database(conn,cursor)
    save_database()

    not_leave = True
    while(not_leave):       
        response = input(f"Select : (1/2/3/...)\n"+
                        "  1 - Test \n"
                        "  2 - Add \n"
                        "  3 - Delete\n"
                        "  4 - Database management\n"
                        "  5 - Leave\n"+
                        "You choose : ").strip().lower()
        print()

        if(response == str(1)):
            test(conn,cursor)

        elif(response == str(2)):
            response = input(f"Select : (1/2/3/...)\n"
                              "  1 - Add card\n"+
                              "  2 - Add card from file\n"
                              "  3 - Return \n"+
                              "You choose : ").strip().lower()
            print()

            if(response == str(1)):
                add_card_manual(conn,cursor)
            elif(response == str(2)):
                add_card_from_file(conn,cursor) 
        
        elif(response == str(3)):
            response = input(f"Select : (1/2/3/...)\n"
                              "  1 - Delete a card\n"+
                              "  2 - Delete a deck\n"
                              "  3 - Return \n"+
                              "You choose : ").strip().lower()
            print()

            if(response == str(1)):
                delete_card(conn,cursor)
            elif(response == str(2)):
                delete_deck(conn,cursor)
        
        elif(response == str(4)):
            response = input(f"Select : (1/2/3/...)\n"
                              "  1 - Export a deck \n"
                              "  2 - Restore the database\n"+
                              "  3 - See the database\n"
                              "  4 - Return \n"+
                              "You choose : ").strip().lower()
            print()

            if(response == str(1)):
                export_deck(conn,cursor)
            
            elif(response == str(2)):
                restore_database()

            elif(response == str(3)):
                display_database(cursor)

        elif(response == str(5)):
            not_leave = False

        else :
            print("The number is not correct \n")

    print("Thank you for using me !")
    conn.close()


if __name__ == "__main__":
    main()