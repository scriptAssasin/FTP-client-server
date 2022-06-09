#pragma once // #include το πολύ μία φορά
#include <stdbool.h> 

// Οι σταθερές αυτές συμβολίζουν εικονικούς κόμβους _πριν_ τον πρώτο και _μετά_ τον τελευταίο
#define queue_BOF (queueNode)0
#define queue_EOF (queueNode)0

typedef struct queue* Queue;
typedef struct queue_node* queueNode;



// Δημιουργεί και επιστρέφει μια νέα λίστα.
Queue queue_create(int size);

// Επιστρέφει τον αριθμό στοιχείων που περιέχει η λίστα.

int queue_size(Queue list);

// Προσθέτει έναν νέο κόμβο στο τέλος

int queue_insert(Queue list, void* value);

// Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε
// destroy_value. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.

void queue_pop(Queue list, queueNode node);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει η λίστα list.
// Οποιαδήποτε λειτουργία πάνω στη λίστα μετά το destroy είναι μη ορισμένη.

void queue_destroy(Queue list);

// Διάσχιση της λίστας /////////////////////////////////////////////

queueNode queue_first(Queue list);
// Επιστρέφει τον κόμβο μετά από τον node, ή queue_EOF αν ο node είναι ο τελευταίος

queueNode queue_next(Queue list, queueNode node);

// Επιστρέφει το περιεχόμενο του κόμβου node

void* queue_node_value(Queue list, queueNode node);