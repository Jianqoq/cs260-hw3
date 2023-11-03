#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256
#define CONSTANT 128

typedef struct Entry {
  char *word;
  struct Entry *next;
} Entry;

typedef struct HashSet {
  Entry **Entries;
  size_t size;
} HashSet;

size_t hash(char *word, size_t size) {

  size_t index = 0;
  int idx = 0;
  while (word[idx] != '\0') {
    index = (index + word[idx]) * 101;
    idx++;
  }
  return (size_t)(index * CONSTANT) % size;
}

HashSet *init(size_t size) {
  Entry **entries = (Entry **)malloc(sizeof(Entry *) * (size + 1));
  for (size_t i = 0; i < (size + 1); i++)
    entries[i] = NULL;
  HashSet *set = (HashSet *)malloc(sizeof(HashSet));
  set->Entries = entries;
  set->size = size + 1;
  return set;
}

void insert(HashSet *set, char *word) {
  size_t index = hash(word, set->size);
  if (set->Entries[index] != NULL) {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    char *string = (char *)malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(string, word);
    entry->word = string;
    entry->next = NULL;
    Entry *next = set->Entries[index];
    while (next->next != NULL) {
      next = next->next;
    }
    next->next = entry;
  } else {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    char *string = (char *)malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(string, word);
    entry->word = string;
    entry->next = NULL;
    set->Entries[index] = entry;
  }
}

char *get(HashSet *set, char *word) {
  size_t index = hash(word, set->size);
  if (set->Entries[index] != NULL) {
    if (!strcmp(word, set->Entries[index]->word)) {
      return set->Entries[index]->word;
    } else {
      Entry *entry = set->Entries[index]->next;
      while (entry != NULL) {
        if (!strcmp(word, entry->word))
          return entry->word;
        entry = entry->next;
      }
    }
  } else {
    return NULL;
  }
}

int is_member(HashSet *set, char *word) {
  size_t index = hash(word, set->size);
  if (set->Entries[index] != NULL) {
    if (!strcmp(word, set->Entries[index]->word)) {
      return 1;
    } else {
      Entry *entry = set->Entries[index]->next;
      while (entry != NULL) {
        if (!strcmp(word, entry->word))
          return 1;
        entry = entry->next;
      }
    }
  } else {
    return 0;
  }
}

void swap(char *word, int index1, int index2) {
  char tmp = word[index1];
  word[index1] = word[index2];
  word[index2] = tmp;
}

int main(int argc, char **argv) {
  char *dictionaryFilePath =
      argv[1]; // this keeps the path to the dictionary file file
  char *inputFilePath = argv[2]; // this keeps the path to the input text file
                                 // mistyped words into dictionary or ignore
  int numOfWords = 0; // this variable will tell us how much memory to allocate
  int insertToDictionary = 0;
  if (argc == 4 && strcmp(argv[3], "add") == 0)
    insertToDictionary = 1;
  ////////////////////////////////////////////////////////////////////
  // read dictionary file
  FILE *fp = fopen(dictionaryFilePath, "r");
  char *line = NULL;       // variable to be used for line counting
  size_t lineBuffSize = 0; // variable to be used for line counting
  ssize_t lineSize;        // variable to be used for line counting

  // check if the file is accessible, just to make sure...
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }

  // First, let's count number of words in the dictionary.
  // This will help us know how much memory to allocate for our hash table
  while ((lineSize = getline(&line, &lineBuffSize, fp)) != -1)
    numOfWords++;

  // Printing line count for debugging purposes.
  // You can remove this part from your submission.
  // printf("%d\n",numOfWords);

  // HINT: You can initialize your hash table here, since you know the size of
  // the dictionary

  // rewind file pointer to the beginning of the file, to be able to read it
  // line by line.
  fseek(fp, 0, SEEK_SET);

  char wrd[BUFSIZE];
  size_t max_index = 0;
  size_t size = 200000;
  for (int i = 0; i < numOfWords; i++) {
    fscanf(fp, "%s \n", wrd);
    size_t index = hash(wrd, size);
    if (index > max_index) {
      max_index = index;
    }
  }
  HashSet *hash_set = init(max_index);
  assert(hash_set != NULL);
  fseek(fp, 0, SEEK_SET);
  for (int i = 0; i < numOfWords; i++) {
    fscanf(fp, "%s \n", wrd);
    size_t index = hash(wrd, size);
    insert(hash_set, wrd);
  }
  fclose(fp);

  ////////////////////////////////////////////////////////////////////
  // read the input text file word by word
  fp = fopen(inputFilePath, "r");

  // check if the file is accessible, just to make sure...
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    return -1;
  }

  // HINT: You can use a flag to indicate if there is a misspleed word or not,
  // which is initially set to 1
  int noTypo = 1;
  // read a line from the input file
  while ((lineSize = getline(&line, &lineBuffSize, fp)) != -1) {
    char *word;
    // These are the delimiters you are expected to check for. Nothing else is
    // needed here.
    const char delimiter[] = " ,.:;!\n";

    // split the buffer by delimiters to read a single word
    word = strtok(line, delimiter);

    // read the line word by word
    while (word != NULL) {
      // You can print the words of the inpit file for Debug purposes, just to
      // make sure you are loading the input text as intended
      int mis_spell = is_member(hash_set, word);
      if (!mis_spell) {
        noTypo++;
        printf("Misspelled word: %s\n", word);
        printf("Suggestions: "); // the suggested words should follow
        char *missing1 = (char *)malloc(sizeof(char) * strlen(word));
        char *missing2 = (char *)malloc(sizeof(char) * strlen(word));
        for (int i = 0; i < strlen(word) - 1; i++) {
          missing1[i] = word[i + 1];
          missing2[i] = word[i];
        }
        missing1[strlen(word) - 1] = '\0';
        missing2[strlen(word) - 1] = '\0';
        if (is_member(hash_set, missing1))
          printf("%s ", missing1);
        if (is_member(hash_set, missing2))
          printf("%s ", missing2);
        free(missing1);
        free(missing2);

        char *extra1 = (char *)malloc(sizeof(char) * (strlen(word) + 2));
        char *extra2 = (char *)malloc(sizeof(char) * (strlen(word) + 2));
        int tracker = 0;
        for (int j = 1; j < strlen(word) + 1; j++) {
          extra1[j] = word[tracker];
          extra2[j - 1] = word[tracker];
          tracker++;
        }
        extra1[strlen(word) + 1] = '\0';
        extra2[strlen(word) + 1] = '\0';
        for (int i = 'a'; i <= 'z'; i++) {
          extra1[0] = i;
          extra2[strlen(word)] = i;
          if (is_member(hash_set, extra1))
            printf("%s ", extra1);
          if (is_member(hash_set, extra2))
            printf("%s ", extra2);
        }
        free(extra1);
        free(extra2);

        char *word_cpy = (char *)malloc(sizeof(char) * (strlen(word) + 1));
        strcpy(word_cpy, word);
        for (int i = 0; i < strlen(word) - 1; i++) {
          swap(word_cpy, i, i + 1);
          if (is_member(hash_set, word_cpy))
            printf("%s ", word_cpy);
          strcpy(word_cpy, word);
        }
        printf("\n");

        if (insertToDictionary) {
          char *word_cpy2 = (char *)malloc(sizeof(char) * (strlen(word) + 1));
          strcpy(word_cpy2, word);
          insert(hash_set, word_cpy2);
        }
        free(word_cpy);
      }

      // INPUT/OUTPUT SPECS: use the following line for printing suggestions,
      // each of which will be separated by a comma and whitespace.

      word = strtok(NULL, delimiter);
    }
  }
  fclose(fp);
  for (int i = 0; i < hash_set->size; i++) {
    if (hash_set->Entries[i] != NULL) {
      if (hash_set->Entries[i]->next != NULL) {
        Entry *entry = hash_set->Entries[i]->next;
        while (entry != NULL) {
          free(entry->word);
          Entry *tmp = entry;
          entry = entry->next;
          free(tmp);
        }
        free(hash_set->Entries[i]->word);
        free(hash_set->Entries[i]);
      } else {
        free(hash_set->Entries[i]->word);
        free(hash_set->Entries[i]);
      }
    }
  }
  free(hash_set->Entries);
  free(hash_set);
  // HINT: If the flag noTypo is not altered (which you should do in the loop
  // above if there exists a word not in the dictionary), then you should print
  // "No typo!"
  if (noTypo == 1)
    printf("No typo!\n");

  // DON'T FORGET to free the memory that you allocated

  return 0;
}
