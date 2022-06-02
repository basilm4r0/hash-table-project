// COMP2421 Project 3 by Basil Mari     Student ID: 1191027     Date: 2/6/2022      Section: 4

#include <stdio.h>
#include <stdlib.h>
#define _SVID_SOURCE
#include <string.h>
#include <math.h>
#include <sys/types.h>

int HT_INITIAL_BASE_SIZE = 10;
int numOfData = 0;
int HT_PRIME_1 = 13;
int HT_PRIME_2 = 17;
int NumOfData = 0;

//Safe malloc functions
static void *xmalloc_fatal(size_t size) {
  if (size==0) return NULL;
  fprintf(stderr, "Out of memory.");
  exit(1);
}

void *xmalloc (size_t size) {
  void *ptr = malloc (size);
  if (ptr == NULL) return xmalloc_fatal(size);
  return ptr;
}

void *xcalloc (size_t nmemb, size_t size) {
  void *ptr = calloc (nmemb, size);
  if (ptr == NULL) return xmalloc_fatal(nmemb*size);
  return ptr;
}

void *xrealloc (void *ptr, size_t size) {
  void *p = realloc (ptr, size);
  if (p == NULL) return xmalloc_fatal(size);
  return p;
}

char *xstrdup (const char *s) {
  void *ptr = xmalloc(strlen(s)+1);
  strcpy (ptr, s);
  return (char*) ptr;
}

int is_prime(const int x) {
    if (x < 2) { return -1; }
    if (x < 4) { return 1; }
    if ((x % 2) == 0) { return 0; }
    for (int i = 3; i <= floor(sqrt((double) x)); i += 2) {
        if ((x % i) == 0) {
            return 0;
        }
    }
    return 1;
}

//find next prime int after x
int next_prime(int x) {
    while (is_prime(x) != 1) {
        x++;
    }
    return x;
}

//removes newline character from end of string
int TrimNewline(char string[]) {
	if ((strlen(string) > 0) && (string[strlen (string) - 1] == '\n'))
		string[strlen (string) - 1] = '\0';
	return 1;
}

//course data structure
typedef struct {
    char* name;
	int credits;
	char* code;
	char* department;
	char* topics;
} ht_item;

//hash table data structure
typedef struct {
	int size_index;
	int size;
	int count;
	int collisions;
	ht_item** items;
} ht_hash_table;

//API
static ht_item HT_DELETED_ITEM = {NULL, 0, NULL, NULL, NULL};
void ht_insert(ht_hash_table* ht, const char* name, int credits, const char* code, const char* department, const char* topics);
ht_item* ht_search(ht_hash_table* ht, const char* name);
void ht_delete(ht_hash_table* h, const char* name);

//duplicates strings
char* strdup(const char *src) {
    char* dst = xmalloc(strlen (src) + 1);  // Space for length plus nul
    if (dst == NULL) return NULL;          // No memory
    strcpy(dst, src);                      // Copy the characters
    return dst;                            // Return the new string
}

//creates new hash table item
static ht_item* ht_new_item(const char* name, int credits, const char* code, const char* department, const char*topics) {
    ht_item* i = xmalloc(sizeof(ht_item));
    i->name = strdup(name);
	i->credits = credits;
    i->code = strdup(code);
    i->department = strdup(department);
    i->topics = strdup(topics);
    return i;
}

//deletes hash table item
static void ht_del_item(ht_item* i) {
    free(i->name);
    free(i->code);
    free(i->department);
    free(i->topics);
    free(i);
}

//deletes hash table and contents
void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != & HT_DELETED_ITEM) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

//hash function
static int ht_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long) pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

//double hashing function
static int ht_get_hash(const char* s, const int num_buckets, const int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
	if (hash_b % num_buckets == 0) {
		hash_b = 1;
	}
	return (hash_a + (attempt * hash_b)) % num_buckets; // Not adding 1 to hash_b here
}

//linear hashing functions
static int ht_get_hash_linear(const char* s, const int num_buckets, const int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	return (hash_a + attempt % num_buckets); // Not adding 1 to hash_b here
}

//finds item in double hashed hash table and deletes it
void ht_delete(ht_hash_table* ht, const char* name) {
    int index = ht_get_hash(name, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->name, name) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
				ht->count--;
                return;
            }
        }
        index = ht_get_hash(name, ht->size, i);
        item = ht->items[index];
        i++;
    }
}

//finds item in linearly hashed hash table and deletes it
void ht_delete_linear(ht_hash_table* ht, const char* name) {
    int index = ht_get_hash_linear(name, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->name, name) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
				ht->count--;
                return;
            }
        }
        index = ht_get_hash_linear(name, ht->size, i);
        item = ht->items[index];
        i++;
    }
}

//resized hash table
static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->size_index = base_size;

    ht->size = next_prime(ht->size_index);

    ht->count = 0;
	ht->collisions = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

//creates new hash table
ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

//resizes hash table
static void ht_resize(ht_hash_table* ht, const int direction) {
    const int new_size_index = ht->size_index + direction;
    if (new_size_index < 0) {
        // Don't resize down the smallest hash table
        return;
    }
    // Create a temporary new hash table to insert items into
    ht_hash_table* new_ht = ht_new_sized(new_size_index);
    // Iterate through existing hash table, add all items to new
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->name, item->credits, item->code, item->department, item->topics);
        }
    }

    // Pass new_ht and ht's properties. Delete new_ht
    ht->size_index = new_ht->size_index;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

//approximately doubles hash table size
static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->size_index * 2;
    ht_resize(ht, new_size);
}

//inserts item into hash table
void ht_insert(ht_hash_table* ht, const char* name, int credits, const char* code, const char* department, const char* topics) {
	const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }
    ht_item* item = ht_new_item(name, credits, code, department, topics);
    int index = ht_get_hash(item->name, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    while (cur_item != NULL) {
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->name, name) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->name, ht->size, i);
        cur_item = ht->items[index];
        i++;
		ht->collisions++;
    }
    ht->items[index] = item;
    ht->count++;
}

//inserts item into linearly hashed table
void ht_insert_linear(ht_hash_table* ht, const char* name, int credits, const char* code, const char* department, const char* topics) {
	const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }
    ht_item* item = ht_new_item(name, credits, code, department, topics);
    int index = ht_get_hash_linear(item->name, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    while (cur_item != NULL) {
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->name, name) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash_linear(item->name, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}

//finds item in hash table
ht_item* ht_search(ht_hash_table* ht, const char* name) {
    int index = ht_get_hash(name, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
		if (item != &HT_DELETED_ITEM) {
			if (strcmp(item->name, name) == 0) {
				return item;
			}
		}
        index = ht_get_hash(name, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}

//finds item in linearly hashed table
ht_item* ht_search_linear(ht_hash_table* ht, const char* name) {
    int index = ht_get_hash_linear(name, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
		if (item != &HT_DELETED_ITEM) {
			if (strcmp(item->name, name) == 0) {
				return item;
			}
		}
        index = ht_get_hash_linear(name, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}

//loads data into both double hashed table and linearly hashed table
void load_data(ht_hash_table* ht, ht_hash_table* ht_linear){
	FILE* in;
	char name[64];
	int credits;
	char code[16];
	char department[64];
	char topics[256];
	char line[500];
	in = fopen("offered_courses.txt","r");
	while(fgets(line,499,in) != 0){	//iterate file lines
		if(feof(in))
			break;
		else {
			strncpy(name, strtok(line,":"), 64);
			credits = atoi(strtok(NULL,"#"));
			strncpy(code, strtok(NULL,"#"), 16);
			strncpy(department, strtok(NULL,"/"), 64);
			strncpy(topics, strtok(NULL,"\n"), 256);
			ht_insert(ht, name, credits, code, department, topics);
			ht_insert_linear(ht_linear, name, credits, code, department, topics);
		}
	}
	fclose(in);
}

//prints hash table
void print_hash(ht_hash_table* ht){
    for(int i=0; i<ht->size; i++){
		if(ht->items[i] != NULL) {
			if(ht->items[i] != &HT_DELETED_ITEM )
				printf("At index: %d   %s  %d  %s  %s  %s\n", i, ht->items[i]->name, ht->items[i]->credits, ht->items[i]->code, ht->items[i]->department,ht->items[i]->topics);
			else
				printf("At index: %d        \n",i);
        }
		else
			printf("At index: %d        \n",i);
	}
}

//prints hash table to saved_courses.txt
void save_hash(ht_hash_table* ht){
	FILE* out;
	out = fopen("saved_courses.txt", "w");
    for(int i=0; i<ht->size; i++){
		if(ht->items[i] != NULL) {
			if(ht->items[i] != &HT_DELETED_ITEM )
				fprintf(out, "At index: %d   %s  %d  %s  %s  %s\n", i, ht->items[i]->name, ht->items[i]->credits, ht->items[i]->code, ht->items[i]->department,ht->items[i]->topics);
			else
				fprintf(out, "At index: %d        \n",i);
        }
		else
			fprintf(out, "At index: %d        \n",i);
	}
}

//main body of program contains user interface
int main() {
	int option;
	char name[64];
	int credits;
	char code[16];
	char department[64];
	char topics[256];
	char input[256];
	int load;

	ht_item* item;
    ht_hash_table* ht = ht_new();
    ht_hash_table* ht_linear = ht_new();
	load_data(ht, ht_linear);

	printf("\nEnter a number to perform one of the following operations:\n1. Print hash table\n2. Print hash table size and load factor.\n3. Print hash function.\n4. Add course to hash table.\n5. Find course in hash table.\n6. Delete course.\n7. Compare number of collisions between two hashing methods.\n8. Save hash table to saved_courses.txt.\n9. Exit program.\n");
	do {
		printf("Option: ");		//Printing prompt and reading user input
		fgets(input, 63, stdin);
		sscanf(input, "%d", &option);
		switch (option) {

			case (1):			//Option 1: Print hash table
				printf("Double hashing table:\n");
				print_hash(ht);
				printf("\nLinear collision handling table:\n");
				print_hash(ht_linear);
				break;

			case (2):			//Option 2: Print hash tables' size and load factor
				load = ht->count * 100 / ht->size;
				printf("Hash table size: %d,\tload factor: %d%%\n", ht->size, load);
				break;

			case (3):			//Option 3: Print hash function
				printf("double hash function:\n");
				printf("for (int i = 0; i < len_s; i++) {\n\thash += (long) pow(a, len_s - (i+1)) * s[i];\n\thash = hash %% m;\n}\n");
				break;

			case (4):			//Option 4: Add course to hash table
				printf("New course name: ");
				fgets(input, 63, stdin);
				TrimNewline(input);
				strcpy(name, input);
				printf("Credit hours: ");
				fgets(input, 63, stdin);
				sscanf(input, "%d", &credits);
				printf("Couse code: ");
				fgets(input, 15, stdin);
				sscanf(input, "%15s", code);
				printf("Department: ");
				fgets(input, 63, stdin);
				sscanf(input, "%63s", department);
				printf("Course topics: ");
				fgets(input, 255, stdin);
				TrimNewline(input);
				strcpy(topics, input);
				ht_insert(ht, name, credits, code, department, topics);
				ht_insert_linear(ht_linear, name, credits, code, department, topics);
				break;

			case (5):			// Option 5: Find course in hash table
				printf("Course name: ");
				fgets(input, 63, stdin);
				TrimNewline(input);
				strcpy(name, input);
				if ((item = ht_search(ht, name)) == NULL)
					printf("Course does not exit!\n");
				else
					printf("Course information:   %s  %d  %s  %s  %s\n", item->name, item->credits, item->code, item->department, item->topics);
				break;

			case (6):			// Option 6: Delete course from hash table
				printf("Course name: ");
				fgets(input, 63, stdin);
				TrimNewline(input);
				strcpy(name, input);
				if (ht_search(ht, name) != NULL) {
					ht_delete(ht, name);
					ht_delete_linear(ht_linear,name);
					printf("Course deleted.\n");
				}
				else
					printf("Course doesn't exist!");
				break;

			case (7):			// Option 7: Compare number of collisions between the two hashing methods
				printf("Double hash table collisions: %d\nLinear hash table collisions: %d\n", ht->collisions, ht_linear->collisions);
				break;

			case (8):			// Option 8: Save hash table to saved_courses.txt
				save_hash(ht);
				break;

			case (9):			// Option 9: Exit program
				printf("Exiting program...\n");
				break;

			default:			// Default option: print error message and prompt user for valid input
				printf("Option invalid. Please enter a valid option.\n");
				break;
		}
	} while(option != 9); //check if exit option has been chosen

    ht_del_hash_table(ht);
    ht_del_hash_table(ht_linear);
}
