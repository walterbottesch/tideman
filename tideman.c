#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

// Max number of candidates
#define MAX 9
#define MAXBUFFERSIZE     80
char buffer[MAXBUFFERSIZE];

void getAnInput();
void getAnInput() {
    buffer[0] = '\0';
    char c;
    int valid_choice;
    int char_count;
    do
    {
        valid_choice = 0;
        char_count = 0;
        c = getchar();
        while (c != '\n' && char_count < MAXBUFFERSIZE)
        {
            buffer[char_count++] = c;
            c = getchar();
        }
        buffer[char_count] = 0x00;
        valid_choice = 1;
    } while (valid_choice == 0);  
}
// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser


typedef struct  {
    int winner;
    int loser;
}
pair;

typedef struct {
char data[MAXBUFFERSIZE];
} string;

// Array of candidates

string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name1, int ranks[]); 
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);


int main(int argc, char *argv[])  {
    // Check for invalid usage
    if (argc < 2) {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX) {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)  {
      //  candidates[i].data =  argv[i + 1];
        strcpy(candidates[i].data, argv[i + 1]);
    }
    printf("candidates : \n");
    for (int  i = 0; i < candidate_count; i++) {
        printf(" %i: %s\n", i + 1, candidates[i].data);
    }
    

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++) {
        for (int j = 0; j < candidate_count; j++)  {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count;
    printf("Number of voters: " );
    getAnInput();
    voter_count = (int)atoi(buffer);
    if (atoi(buffer) <= 0) {
        printf("Invalid Input ! Type in a positive number... ");
        return 4;
    }

    // Query for votes
    for (int i = 0; i < voter_count; i++)  {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++) {
            printf("Rank %i: ", j + 1);
            getAnInput();
            string name;
            strcpy(name.data, buffer);
            if (!vote(j, name, ranks))  {
                printf("Invalid vote 3.\n");
                return 3;
            } 
        }
        record_preferences(ranks);
        printf("\n"); 
    }
    add_pairs(); 
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name1, int ranks[])  {
    for (size_t i = 0; i < candidate_count; i++)  {
        if (strcmp(name1.data, candidates[i].data) == 0) {
            ranks[rank] = i;
            return true;
        }   
    } 
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])  {
    int i = 0;
    while (i < candidate_count * (candidate_count - 1) /2 - 1) {
        int j = i + 1;
        while (j < candidate_count) {
            preferences[ranks[i]][ranks[j]]++;
            j++;
        }
        i++;
    }
    return;
}

// Record pairs of candidates where one is preferred over the other

void add_pairs(void)  {
    int i = 0;
    int count = candidate_count * (candidate_count - 1) /2 - 1;
    for (size_t i = 0; i < count; i++)  {
        for (size_t j = i + 1; j <= candidate_count ; j++) {
            if (preferences[i][j] != preferences[j][i] && i != j)  {
                if (preferences[i][j] > preferences[j][i]) {
                    pairs[pair_count].winner = i;
                    pairs[pair_count].loser = j;
                    pair_count++;
                } else {
                    pairs[pair_count].winner = j;
                    pairs[pair_count].loser = i;
                    pair_count++;
                } 
            }
        }
    } 
    return;
}


// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)  {
    int sorted  = 0;
    while(!sorted) {
        sorted = 1;
        for (size_t i = 0; i < pair_count - 1; i++) {
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i + 1].winner][pairs[i + 1].loser])  {
                sorted = 0;
                pair temp = pairs[i];
                pairs[i] = pairs[i + 1];
                pairs[i + 1] = temp;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)  {
    for (size_t i = 0; i < pair_count; i++) {
        for (size_t j = 0; j < pair_count; j++) {
            if (i == pair_count - 1 && pairs[0].winner == pairs[i].loser) {
                break;
            } else if (pairs[i].winner != pairs[j].loser)   {
                locked[pairs[i].winner][pairs[i].loser] = true;
            }
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)  {
    for (size_t i = 0; i < pair_count; i++)   {
        for (size_t j = 0; j < candidate_count; j++)  {
            for (size_t k = 0; k < candidate_count; k++)  {
                if (locked[pairs[i].winner][pairs[i].loser])   {
                   printf(" WINNER :  %s \n", candidates[pairs[i].winner].data);
                   k = candidate_count;
                   j = candidate_count;
                }   
            }       
        }
        if (pairs[i].winner != pairs[i + 1].winner || pairs[i].loser != pairs[i + 1].loser)  {
            break;
        }
    }
    return;
}