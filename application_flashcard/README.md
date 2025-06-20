# Flashcards Application

## Description

This application allows you to manage flashcards using an SQLite database. It includes features for adding, deleting, and testing flashcards, managing decks, and performing database backups and restorations.

## Features

- **Deck Management**: Create, select, and delete decks.
- **Card Management**: Add, delete, and export cards from a file or manually.
- **Review Testing**: Review cards based on their review dates and update review dates based on responses.
- **Backup and Restoration**: Backup the database state and restore data from a backup.

## Prerequisites

- Python 3.x
- Python modules: `sqlite3`, `datetime`, `json`, `colorama`, `shutil`, `os`

## Installation

Clone this repository and install the necessary dependencies:

```bash
git clone https://github.com/yourusername/flashcards-app.git
cd flashcards-app
pip install colorama
```

## Usage

1. **Database Initialization**: The `flashcards.db` database will be created automatically when you run the program for the first time.

2. **Running the Application**:
   
   ```bash
   python flashcards_app.py
   ```

   Follow the on-screen instructions to choose different options such as testing cards, adding or deleting cards, or managing the database.

3. **Available Commands**:
   - **Test**: Review available cards.
   - **Add**: Add cards manually or from a file.
   - **Delete**: Delete cards or decks.
   - **Database Management**: Export decks, restore the database, or view its content.

## Code Structure

- `creation_database(conn, cursor)`: Creates the necessary tables in the database.
- `update_database(conn, cursor)`: Updates the database information.
- `save_database()`: Saves the current state of the database.
- `restore_database()`: Restores the database from the last backup.
- `get_deck_id_by_name(cursor, deck)`: Retrieves the ID of a deck by its name.
- `create_deck_not_exist(conn, cursor)`: Creates a new deck if it does not already exist.
- `select_deck(conn, cursor, new_deck=False, have_card_to_review=False)`: Allows you to select a deck.
- `get_cards_to_review(conn, cursor, deck_id)`: Retrieves cards to review for a given deck.
- `update_next_review_date(conn, cursor, card, first_time)`: Updates the next review date for a card.
- `update_limit(conn, cursor, deck_id)`: Updates the number of new cards available per day for a deck.
- `test_a_list(conn, cursor, deck_id, cards_to_review, first_time)`: Performs a test with a list of cards.
- `test(conn, cursor)`: Manages the card review process.
- `add_flashcard(conn, cursor, deck_id, data, new, last_review_date, next_review_date)`: Adds a new card to a deck.
- `add_card_manual(conn, cursor)`: Adds cards manually.
- `add_card_from_file(conn, cursor)`: Adds cards from a file.
- `delete_card(conn, cursor)`: Deletes a card from a deck.
- `delete_deck(conn, cursor)`: Deletes a deck along with its cards.
- `export_deck(conn, cursor)`: Exports cards from a deck to a file.
- `display_database(cursor)`: Displays the contents of the database.