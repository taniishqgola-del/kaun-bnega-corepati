#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define MAX_QUES_LEN 300
#define MAX_OPTION_LEN 100

typedef struct {
    char text[MAX_QUES_LEN];
    char options[4][MAX_OPTION_LEN];
    char correct_option;
    int timeout;
    int prize_money;
} Question;

// function declarations
int read_questions(char* file_name, Question** questions);
void print_question(Question q);
void play_game(Question* questions, int n);
int use_lifeline(Question* q, int* lifeline);

// ---------------- MAIN ----------------
int main() {
    srand(time(NULL));

    printf("\n\t\t🎮 KAUN BANEGA CROREPATI 🎮\n");

    Question* questions;
    int total = read_questions("questions.txt", &questions);

    play_game(questions, total);

    return 0;
}

// ---------------- GAME ----------------
void play_game(Question* questions, int n) {
    int money = 0;
    int lifeline[2] = {1,1};

    for(int i = 0; i < n; i++) {

        print_question(questions[i]);

        int time_left = questions[i].timeout;
        char ch = 0;

        // timer loop
        while(time_left > 0) {
            printf("\r⏳ Time left: %d sec ", time_left);
            Sleep(1000);
            time_left--;

            if(_kbhit()) {
                ch = _getch();
                break;
            }
        }

        if(time_left == 0) {
            printf("\n❌ Time Out!\n");
            break;
        }

        ch = toupper(ch);

        // lifeline
        if(ch == 'L') {
            int val = use_lifeline(&questions[i], lifeline);
            if(val != 2) i--;   // repeat question
            continue;
        }

        // check answer
        if(ch == questions[i].correct_option) {
            printf("\n✅ Correct!\n");
            money = questions[i].prize_money;
            printf("💰 Won: Rs %d\n", money);
        } else {
            printf("\n❌ Wrong! Correct answer: %c\n", questions[i].correct_option);
            break;
        }
    }

    printf("\n🏁 Game Over! Total Winning: Rs %d\n", money);
}

// ---------------- PRINT ----------------
void print_question(Question q) {
    printf("\n\n%s", q.text);

    for(int i = 0; i < 4; i++) {
        if(q.options[i][0] != '\0') {
            printf("%c. %s", 'A'+i, q.options[i]);
        }
    }

    printf("\nEnter A/B/C/D or L (lifeline): ");
}

// ---------------- LIFELINE ----------------
int use_lifeline(Question* q, int* lifeline) {

    printf("\n\n1. 50-50\n2. Skip\nChoose: ");
    char ch = _getch();

    if(ch == '1' && lifeline[0]) {
        lifeline[0] = 0;

        int removed = 0;
        while(removed < 2) {
            int r = rand() % 4;
            if((r+'A') != q->correct_option && q->options[r][0] != '\0') {
                q->options[r][0] = '\0';
                removed++;
            }
        }
        return 1;
    }

    if(ch == '2' && lifeline[1]) {
        lifeline[1] = 0;
        return 2;
    }

    return 0;
}

// ---------------- FILE READ ----------------
int read_questions(char* file_name, Question** questions) {

    FILE* file = fopen(file_name, "r");

    if(file == NULL) {
        printf("\n❌ Cannot open questions file\n");
        exit(0);
    }

    char line[MAX_QUES_LEN];
    int count = 0;

    while(fgets(line, MAX_QUES_LEN, file))
        count++;

    int total = count / 8;

    *questions = (Question*) malloc(total * sizeof(Question));

    rewind(file);

    for(int i = 0; i < total; i++) {

        fgets((*questions)[i].text, MAX_QUES_LEN, file);

        for(int j = 0; j < 4; j++)
            fgets((*questions)[i].options[j], MAX_OPTION_LEN, file);

        char opt[10];
        fgets(opt, 10, file);
        (*questions)[i].correct_option = opt[0];

        char t[10];
        fgets(t, 10, file);
        (*questions)[i].timeout = atoi(t);

        char p[10];
        fgets(p, 10, file);
        (*questions)[i].prize_money = atoi(p);
    }

    fclose(file);
    return total;
}
