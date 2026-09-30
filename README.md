# Indonesian-English Digital Dictionary

A command-line Indonesian-English digital dictionary developed in **C** as an academic project for the Data Structures course.

The project contains **512 dictionary entries** and demonstrates how multiple data structures can be combined to manage, search, add, and delete dictionary data.

## Overview

The application provides a simple dictionary system that allows users to:

* View all dictionary entries
* Search for words by their initial letter
* Search for a specific word
* Add new dictionary entries
* Delete existing entries
* Store dictionary data using file-based persistence
* Perform case-insensitive searches

Each dictionary entry contains the Indonesian word, English translation, synonyms, definition, and a unique identifier.

## Data Structures

The project implements several data structures, each serving a different purpose within the application.

| Data Structure              | Purpose                                        |
| --------------------------- | ---------------------------------------------- |
| Array                       | Stores the main dictionary data                |
| Binary Search Tree (BST)    | Organizes words based on their initial letters |
| AVL Tree                    | Provides balanced searching for specific words |
| Hash Table                  | Supports efficient word lookup                 |
| Circular Doubly Linked List | Maintains ordered letter nodes                 |
| Queue                       | Processes word additions and deletions         |

## Main Features

### 1. Display All Words

Displays all available dictionary entries, including:

* Word code
* Indonesian word
* English translation
* Indonesian synonym
* English synonym
* Definition

### 2. Search by Initial Letter

Users can enter an initial letter to display all dictionary entries beginning with that letter.

Example:

```text
Input: B

Output:
[B001] Baju - Shirt
[B002] Bakar - Burn
[B003] Banjir - Flood
```

### 3. Specific Word Search

Users can search for a specific Indonesian word.

Example:

```text
Input: Rumah

Output:
Code        : R018
Indonesia   : Rumah
English     : House
Synonym ID  : Bangunan
Synonym EN  : Home
Definition  : Tempat tinggal.
```

### 4. Add a New Word

Users can add a new dictionary entry by providing:

* Indonesian word
* English translation
* Indonesian synonym
* English synonym
* Definition

The application validates the input and checks whether the word already exists before adding it.

### 5. Delete a Word

Users can remove an existing dictionary entry by entering the word they want to delete.

After an entry is added or deleted, the supporting data structures are rebuilt to reflect the updated dictionary data.

## Data Management

The application loads dictionary data from a text file during initialization.

Each entry follows this format:

```text
code#indonesian#english#indonesian_synonym#english_synonym#definition
```

Example:

```text
A001#Abjad#Alphabet#Aksara#Letters#Kumpulan huruf berdasarkan urutan lazim dalam bahasa.
```

The updated dictionary data is written back to the file after modification.

## Program Flow

```text
Start
  │
  ├── Load dictionary file
  │
  ├── Build data structures
  │     ├── Hash Table
  │     ├── BST
  │     ├── CDLL
  │     └── AVL Tree
  │
  ├── Display Main Menu
  │
  ├── View All Words
  │
  ├── Search Word
  │
  ├── Add Word
  │     └── Update & Rebuild Structures
  │
  ├── Delete Word
  │     └── Update & Rebuild Structures
  │
  └── Exit
```

## Challenges

One of the main challenges was maintaining consistency between multiple data structures that represented the same dictionary dataset.

When a word was added or deleted, the main dictionary data had to be updated while the supporting structures, including the BST, AVL Tree, Circular Doubly Linked List, and Hash Table, also had to reflect the changes.

Another challenge was implementing different search mechanisms for different use cases. Initial-letter searches and specific word searches use different structures within the application.

Memory management was also an important consideration because several of the structures use dynamically allocated nodes that need to be properly freed when rebuilding the structures.

## What I Learned

This project provided practical experience in implementing and combining different data structures within a single C application.

Through the project, I gained experience with:

* Binary Search Trees
* AVL Trees and tree rotations
* Hash Tables
* Circular Doubly Linked Lists
* Queues
* Arrays
* Recursion
* Dynamic memory allocation
* File handling
* Searching and sorting
* Data validation
* Maintaining consistency between data structures

More importantly, the project helped me understand that choosing a data structure depends on the operation being performed and the way the data needs to be accessed.

## Outcome

The completed application provides a functional command-line Indonesian-English dictionary containing **512 entries**.

It successfully demonstrates the integration of multiple data structures into one application while supporting core dictionary operations such as searching, adding, deleting, displaying, and persistent data storage.

## Technologies

* **Language:** C
* **Concepts:** Data Structures, Algorithms, File Handling, Dynamic Memory Allocation
* **Structures:** Array, BST, AVL Tree, Hash Table, CDLL, Queue

## Academic Context

This project was developed as part of the **Data Structures** course at BINUS University.

**Team Members:**

* Hans Christian Kosasih
* Jonathan Elloy Saputra
* Stevanus Sunandar

## Screenshots

Screenshots of the application can be found in the [`screenshots`](./screenshots) directory.

## Project Status

Completed as an academic project.
