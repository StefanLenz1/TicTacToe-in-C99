// Benutzung: ./tictactoe [easy, hard]
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct MoveStruct
{
    int move;
    int score;
} moveStruct;

#define emptyField 0
#define comField 1
#define playerField 2

int (*setDifficulty(char *difficulty))(int);
int getMovePlayer();
int getMoveComEasy(int recentMove);

//Minimax functions
int getMoveComHard(int recentMove);
moveStruct findBestMoveMinimax(int recentMove, bool currentPlayer);
moveStruct minOrMaxMinimax(bool currentPlayer, moveStruct scores[], int amountRemainingFields);

int checkFullFieldMinimax(int recentMove);
int *getRemainingFields();
bool checkWinner(int recentMove, int player);
bool checkInput(char input[]);
void displayGraph();

int tictactoeField[9] = {}; // Jedes Element zu 0 initiieren. 0 ist Leer, 1 ist O and 2 ist X, ? ist ein Error
/* Tictactoe Feld
    012
    345
    678
*/

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Falsche Anzahl an Argumente (Programm Funktionsweise: ./tictactoe [easy, hard])\n");
        return 1;
    }
    int (*getMoveCom_ptr)(int) = setDifficulty(argv[1]); // Function Pointer um Schwierigkeit bei jeden Zug umzustellen
    if (getMoveCom_ptr == NULL)
    {
        printf("Nicht vorhandener Spielmodus (Programm Funktionsweise: ./tictactoe [easy, hard])\n");
        return 2;
    }

    srand(time(NULL));
    int movePlayer, moveCom;
    char winner;
    displayGraph();

    while (true)
    {
        movePlayer = getMovePlayer();
        tictactoeField[movePlayer] = playerField;
        if (checkWinner(movePlayer, playerField))
        {
            winner = 'X';
            break;
        }

        int *remainingFieldsPtr = getRemainingFields();
        int amountRemainingFields = *remainingFieldsPtr;
        free(remainingFieldsPtr);

        if (amountRemainingFields == 0) // Das Feld ist voll
        {
            winner = '-';
            break;
        }

        //Delay nach Spielerzug
        /*displayGraph();
        unsigned int delay = time(0) + 1; //Com Zug wartet
        while (time(0) < delay);*/

        moveCom = getMoveCom_ptr(movePlayer);
        tictactoeField[moveCom] = comField;
        if (checkWinner(moveCom, comField))
        {
            winner = 'O';
            break;
        }
        displayGraph();
    }
    displayGraph();
    printf("\nWinner: %c\n", winner);
}

int (*setDifficulty(char *difficulty))(int)
{
    if (strcmp(difficulty, "easy") == 0)
    {
        return getMoveComEasy;
    }
    else if (strcmp(difficulty, "hard") == 0)
    {
        return getMoveComHard;
    }
    else
    {
        return NULL;
    }
}

int getMovePlayer()
{
    char input[3];

    do
    {
        printf("Koordinate eingeben (z.B. B3): ");
        scanf("%2s", input);
        putchar('\n');
    } while (checkInput(input));

    input[0] = toupper(input[0]);
    return (input[0] - 'A') * 3 + input[1] - '1'; // hash spielzug z.B. B2 -> 5
}

int getMoveComEasy(int recentMove) // Feldauswahl durch reinen Zufall
{
    int *remainingFieldsPtr = getRemainingFields();
    int randomNumber = rand() % *remainingFieldsPtr; // Zufällige Zahl zwischen: 0 und "remaining fields"
    int move = remainingFieldsPtr[randomNumber + 1];

    free(remainingFieldsPtr);
    return move;
}

int getMoveComHard(int recentMove) // Feldauswahl durch Minimax
{
    moveStruct move = findBestMoveMinimax(recentMove, true);
    return move.move;
}

moveStruct findBestMoveMinimax(int recentMove, bool currentPlayer)
{
    int *remainingFieldsPtr = getRemainingFields();
    int amountRemainingFields = *remainingFieldsPtr;
    moveStruct move;
    move.move = recentMove;

    // Testet ob Spiel vorbei ist
    int result = checkFullFieldMinimax(recentMove);
    if (result != 0)
    {
        move.score = result * (amountRemainingFields + 1);
    }
    else if (amountRemainingFields == 0)
    {
        move.score = 0;
    }
    else //Wenn Spiel nicht vorbei ist
    {
        moveStruct scores[amountRemainingFields];
        for (int i = 0; i < amountRemainingFields; i++) // Scores von allen Feldern einsammeln
        {
            scores[i].move = remainingFieldsPtr[i + 1];
            int currentPlayerField;
            (currentPlayer) ? (currentPlayerField = comField) : (currentPlayerField = playerField);
            tictactoeField[scores[i].move] = currentPlayerField; // Feld wird temporär belegt
            moveStruct tempMove = findBestMoveMinimax(scores[i].move, !currentPlayer);
            scores[i].score = tempMove.score;
            tictactoeField[scores[i].move] = emptyField; // Feld wird zurückgesetzt
        }
        move = minOrMaxMinimax(currentPlayer, scores, amountRemainingFields); // Move mit höchsten Score wird heraussortiert
    }

    free(remainingFieldsPtr);
    return move;
}

moveStruct minOrMaxMinimax(bool currentPlayer, moveStruct scores[], int amountRemainingFields)
{
    if (currentPlayer) // Höchster Score wird gesucht
    {
        moveStruct max = scores[0];
        for (int i = 1; i < amountRemainingFields; i++)
        {
            if (scores[i].score > max.score)
            {
                max = scores[i];
            }
        }
        return max;
    }
    else // Niedrigster Score wird gesucht
    {
        moveStruct min = scores[0];
        for (int i = 1; i < amountRemainingFields; i++)
        {
            if (scores[i].score < min.score)
            {
                min = scores[i];
            }
        }
        return min;
    }
}

int checkFullFieldMinimax(int recentMove)
{
    if (checkWinner(recentMove, comField))
    {
        return 1;
    }
    else if (checkWinner(recentMove, playerField))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int *getRemainingFields()
{
    int countFreeFields = 0;

    for (int i = 0; i < 9; i++)
    {
        if (tictactoeField[i] == 0)
        {
            countFreeFields++;
        }
    }

    int *remainingFields = malloc((countFreeFields + 1) * sizeof(int)); // Wert des Pointers ist Anzahl verbleibender Felder z.B. 4,0,1,2,6,7
    if (remainingFields == NULL)
    {
        printf("Stack Overflow\n");
        exit(-1);
    }
    *remainingFields = countFreeFields;

    for (int i = 0, fillArrayCount = 1; i < 9; i++)
    {
        if (tictactoeField[i] == 0)
        {
            remainingFields[fillArrayCount] = i; // Jedes drauffolgendes Element ist ein freies Feld
            fillArrayCount++;
        }
    }
    return remainingFields;
}

bool checkWinner(int recentMove, int player)
{
    int collumn = recentMove / 3;
    int row = recentMove % 3;
    int checkTictactoe[3][3] = {};

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            checkTictactoe[i][j] = tictactoeField[3 * i + j];
        }
    }

    if (checkTictactoe[0][row] == player && checkTictactoe[1][row] == player && checkTictactoe[2][row] == player) // reihen
    {
        return true;
    }
    if (checkTictactoe[collumn][0] == player && checkTictactoe[collumn][1] == player && checkTictactoe[collumn][2] == player) // spalten
    {
        return true;
    }
    if (checkTictactoe[0][0] == player && checkTictactoe[1][1] == player && checkTictactoe[2][2] == player) // 1. diagonale
    {
        return true;
    }
    if (checkTictactoe[2][0] == player && checkTictactoe[1][1] == player && checkTictactoe[0][2] == player) // 2. diagonale
    {
        return true;
    }

    return false;
}

bool checkInput(char input[])
{
    int *remainingFieldsPtr = getRemainingFields();
    input[0] = toupper(input[0]);

    if (!isalpha(input[0]) || !isdigit(input[1]) || (input[1] > '3') || input[1] == '0' || input[0] > 'C')
    {
        displayGraph();
        printf("Ungültiger Eingabe\n");

        free(remainingFieldsPtr);
        return true;
    }
    int inputHash = (input[0] - 'A') * 3 + input[1] - '1';
    for (int i = 1; i < *remainingFieldsPtr + 1; i++) // Testet ob Feld belegt ist
    {
        if (inputHash == remainingFieldsPtr[i]) // Testet ob Feld Teil von freien Feldern ist
        {
            free(remainingFieldsPtr);
            return false; // Feld ist gültig
        }
    }

    displayGraph();
    printf("Belegtes Feld\n");

    free(remainingFieldsPtr);
    return true;
}

void displayGraph()
{
    for (int i = 0; i < 15; i++)
        putchar('\n');
    printf("Du bist: X\n\n");
    printf("  123\n"); // spalten nummerieren
    putchar(' ');
    for (int i = 0; i < 5; i++)
        putchar('#');
    putchar('\n');
    for (int i = 0; i < 3; i++)
    {
        switch (i)
        { // reihen beschriften
        case 0:
            putchar('A');
            break;
        case 1:
            putchar('B');
            break;
        case 2:
            putchar('C');
            break;
        }
        putchar('#');
        for (int j = 0; j < 3; j++)
        {
            switch (tictactoeField[3 * i + j])
            {
            case 0:
                putchar(' ');
                break;
            case 1:
                putchar('O');
                break;
            case 2:
                putchar('X');
                break;
            default:
                putchar('?');
                break;
            }
        }
        putchar('#');
        putchar('\n');
    }
    putchar(' ');
    for (int i = 0; i < 5; i++)
        putchar('#');
    putchar('\n');
}