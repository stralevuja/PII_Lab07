#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    void *data;       // pokazivac na dinamicki alociran podatak
    struct node *prev; // pokazivac na prethodni cvor
    struct node *next; // pokazivac na sljedeci cvor
} Node;

void add(Node **phead, Node **ptail, void *data, int(*cmp)(const void *, const void *));
void delete(Node **phead, Node **ptail, void *data, int(*cmp)(const void *, const void *));
Node *search(Node *head, Node *tail, void *data, int(*cmp)(const void *, const void *));
void write(Node *head, void(*write_one)(const void *));
void delete_list(Node **phead, Node **ptail);

/* helper funkcije */

// vraca negativan broj ako a < b, 0 ako su jednaki, pozitivan ako a > b
int cmp_string(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

void write_string(const void *data) {
    printf("%s\n", (const char *)data);
}

// pravi heap-alociranu kopiju stringa (potrebno jer lista free()-uje data pri brisanju)
char *dup_string(const char *s) {
    char *novi = malloc(strlen(s) + 1);
    if (novi) strcpy(novi, s);
    return novi;
}

int main(void) {
    Node *head = NULL, *tail = NULL;

    const char *dani[] = {"ponedjeljak", "utorak", "srijeda", "cetvrtak", "petak", "subota"};
    int n = sizeof(dani) / sizeof(dani[0]);

    // dodaje svaki dan u listu
    for (int i = 0; i < n; i++) {
        add(&head, &tail, dup_string(dani[i]), cmp_string);
    }

    printf("Lista nakon dodavanja:\n");
    write(head, write_string);

    delete(&head, &tail, "subota", cmp_string);

    printf("\nLista nakon brisanja 'subota':\n");
    write(head, write_string);

    // brise cijelu listu i oslobadjamo memoriju
    delete_list(&head, &tail);
    return 0;
}

// dodaje novi podatak u listu u rastucem redoslijedu, ignorise duplikate
void add(Node **phead, Node **ptail, void *data, int(*cmp)(const void *, const void *)) {
    // alocira novi cvor
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Nemoguce alocirati memoriju.\n");
        return;
    }
    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;

    if (*phead == NULL) {
        // lista je prazna, novi cvor postaje i glava i rep
        *phead = new_node;
        *ptail = new_node;
    } else {
        // trazimo poziciju gdje novi element treba da stoji (rastuci redoslijed)
        Node *current = *phead;
        while (current != NULL && cmp(current->data, data) < 0) {
            current = current->next;
        }

        // ako element vec postoji ignorise
        if (current != NULL && cmp(current->data, data) == 0) {
            free(new_node);
            return;
        }

        if (current == NULL) {
            // Prosli smo kroz cijelu listu, novi element je najveci -> ubaci na kraj
            (*ptail)->next = new_node;
            new_node->prev = *ptail;
            *ptail = new_node;
        } else if (current == *phead) {
            // Novi element je manji od glave -> ubaci na pocetak
            new_node->next = *phead;
            (*phead)->prev = new_node;
            *phead = new_node;
        } else {
            // Ubaci izmedju current->prev i current
            new_node->prev = current->prev;
            new_node->next = current;
            current->prev->next = new_node;
            current->prev = new_node;
        }
    }
}

// brise prvi cvor koji sadrzi trazeni podatak
void delete(Node **phead, Node **ptail, void *data, int(*cmp)(const void *, const void *)) {
    Node *current = *phead;
    while (current != NULL) {
        if (cmp(current->data, data) == 0) { // nasli smo cvor za brisanje
            if (current == *phead && current == *ptail) {
                // jedini cvor u listi
                *phead = NULL;
                *ptail = NULL;
            } else if (current == *phead) {
                // brise glavu, sljedeci cvor postaje nova glava
                *phead = current->next;
                (*phead)->prev = NULL;
            } else if (current == *ptail) {
                // brise rep, prethodni cvor postaje novi rep
                *ptail = current->prev;
                (*ptail)->next = NULL;
            } else {
                // brise cvor u sredini, premoscujemo veze susjeda
                current->prev->next = current->next;
                current->next->prev = current->prev;
            }
            free(current->data); // oslobadjamo dinamicki alociran podatak
            free(current);       // oslobadjamo sam cvor
            return; // nema duplikata, zavrsavamo nakon prvog pronalaska
        }
        current = current->next;
    }
}

// pretrazuje listu i vraca pokazivac na cvor sa trazenim podatkom, ili NULL ako nije nadjen
Node *search(Node *head, Node *tail, void *data, int(*cmp)(const void *, const void *)) {
    Node *current = head;
    while (current != NULL) {
        if (cmp(current->data, data) == 0) {
            return current; // nasli smo trazeni cvor
        }
        current = current->next;
    }
    return NULL; // podatak nije u listi
}

// prolazi kroz listu od glave do repa i ispisuje svaki podatak pomocu write_one
void write(Node *head, void(*write_one)(const void *)) {
    Node *current = head;
    while (current != NULL) {
        write_one(current->data);
        current = current->next;
    }
}

// brise sve cvorove liste i oslobadja memoriju (i data i cvor)
void delete_list(Node **phead, Node **ptail) {
    Node *current = *phead;
    while (current != NULL) {
        Node *next = current->next; // cuvamo sljedeci prije brisanja trenutnog
        free(current->data);        // oslobadjamo dinamicki alociran podatak
        free(current);              // oslobadjamo sam cvor
        current = next;
    }
    *phead = NULL; // glava i rep postaju NULL, lista je prazna
    *ptail = NULL;
}