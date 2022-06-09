#include <stdlib.h>
#include <assert.h>

#include "ADTQueue.h"

// Ενα Queue είναι pointer σε αυτό το struct
struct queue
{
	queueNode dummy; // χρησιμοποιούμε dummy κόμβο, ώστε ακόμα και η κενή λίστα να έχει έναν κόμβο.
	queueNode last;	 // δείκτης στον τελευταίο κόμβο, ή στον dummy (αν η λίστα είναι κενή)
	int size;		 // μέγεθος, ώστε η queue_size να είναι Ο(1)
	int allowed_size;
};

struct queue_node
{
	queueNode next; // Δείκτης στον επόμενο
	void *value;	// Η τιμή που αποθηκεύουμε στον κόμβο
};

Queue queue_create(int size)
{
	// Πρώτα δημιουργούμε το stuct
	Queue queue = malloc(sizeof(*queue));
	queue->size = 0;
	// Χρησιμοποιούμε dummy κόμβο, ώστε ακόμα και μια άδεια λίστα να έχει ένα κόμβο
	// (απλοποιεί τους αλγορίθμους). Οπότε πρέπει να τον δημιουργήσουμε.
	//
	queue->dummy = malloc(sizeof(*queue->dummy));
	queue->dummy->next = NULL; // άδεια λίστα, ο dummy δεν έχει επόμενο

	// Σε μια κενή λίστα, τελευταίος κόμβος είναι επίσης ο dummy
	queue->last = queue->dummy;

	queue->allowed_size = size;

	return queue;
}

int queue_size(Queue queue)
{
	return queue->size;
}

queueNode queue_first(Queue queue)
{
	// Ο πρώτος κόμβος είναι ο επόμενος του dummy.
	//
	return queue->dummy->next;
}

queueNode queue_next(Queue queue, queueNode node)
{
	assert(node != NULL); // LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)
	return node->next;
}

void *queue_node_value(Queue queue, queueNode node)
{
	assert(node != NULL); // LCOV_EXCL_LINE
	return node->value;
}

int queue_insert(Queue queue, void *value)
{
	// Αν το node είναι NULL απλά εισάγουμε μετά τον dummy κόμβο!
	// Αυτή ακριβώς είναι η αξία του dummy, δε χρειαζόμαστε ξεχωριστή υλοποίηση.
	if (queue->size < 20)
	{

		queueNode node = queue->dummy;

		// Δημιουργία του νέου κόμβου
		queueNode new = malloc(sizeof(*new));
		new->value = value;

		// Σύνδεση του new ανάμεσα στο node και το node->next
		new->next = node->next;
		node->next = new;

		// Ενημέρωση των size & last
		queue->size++;
		if (queue->last == node)
			queue->last = new;

		return 1;
	}
	
	return 0;
}

void queue_pop(Queue queue, queueNode node)
{
	assert(queue != NULL);
	if (node == queue->dummy->next)
	{
		queue->dummy->next = node->next;
		free(node->value);
		free(node);
	}
	else
	{
		queueNode prev_node = queue->dummy->next;
		while (prev_node->next != node)
		{
			prev_node = prev_node->next;
		}
		prev_node->next = node->next;
		free(node->value);
		free(node);
	}
	queue->size--;
}

void queue_destroy(Queue queue)
{
	// Διασχίζουμε όλη τη λίστα και κάνουμε free όλους τους κόμβους,
	// συμπεριλαμβανομένου και του dummy!
	//
	queueNode node = queue->dummy;
	while (node != NULL)
	{								 // while αντί για for, γιατί θέλουμε να διαβάσουμε
		queueNode next = node->next; // το node->next _πριν_ κάνουμε free!

		// Καλούμε τη destroy_value, αν υπάρχει (προσοχή, όχι στον dummy!)
		if (node != queue->dummy)
			free(node->value);

		free(node);
		node = next;
	}

	// Τέλος free το ίδιο το struct
	free(queue);
}