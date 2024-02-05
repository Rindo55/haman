#include "civetweb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_ATTEMPTS 6
#define MAX_WORDS 5
#define MAX_WORD_LENGTH 20

static const char *html_start = "<html><body>";
static const char *html_end = "</body></html>";

char words[MAX_WORDS][MAX_WORD_LENGTH] = {"apple", "banana", "orange", "strawberry", "mango"};
char hints[MAX_WORDS][MAX_WORD_LENGTH] = {"Gravity discovery", "Minions favourite", "A citrus fruit", "A sweet red fruit", "The king of fruits"};
int hintsUsed[MAX_WORDS] = {0};

char wordToGuess[MAX_WORD_LENGTH];
int wordLength;
int guessedLetters[MAX_WORD_LENGTH];
int attempts;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void initializeGame() {
    srand(time(NULL));
    int randomIndex = rand() % MAX_WORDS;

    strcpy(wordToGuess, words[randomIndex]);
    hintsUsed[randomIndex] = 1;

    wordLength = strlen(wordToGuess);
    memset(guessedLetters, 0, sizeof(guessedLetters));

    attempts = 0;
}

void printHangman(struct mg_connection *conn) {
    mg_printf_data(conn, "<pre>");
    switch (attempts) {
        case 0:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
        case 1:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, " O   |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
        case 2:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, " O   |\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
        case 3:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, " O   |\n");
            mg_printf_data(conn, "/|   |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
        case 4:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, " O   |\n");
            mg_printf_data(conn, "/|\\ |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
        case 5:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, " O   |\n");
            mg_printf_data(conn, "/|\\ |\n");
            mg_printf_data(conn, "/    |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
        case 6:
            mg_printf_data(conn, " -----\n");
            mg_printf_data(conn, " |   |\n");
            mg_printf_data(conn, " O   |\n");
            mg_printf_data(conn, "/|\\ |\n");
            mg_printf_data(conn, "/ \\ |\n");
            mg_printf_data(conn, "     |\n");
            mg_printf_data(conn, "-------\n");
            break;
    }
    mg_printf_data(conn, "</pre>");
}

void printWordStatus(struct mg_connection *conn) {
    mg_printf_data(conn, "<p>Word: ");
    for (int i = 0; i < wordLength; i++) {
        if (guessedLetters[i]) {
            mg_printf_data(conn, "%c ", wordToGuess[i]);
        } else {
            mg_printf_data(conn, "_ ");
        }
    }
    mg_printf_data(conn, "</p>");
}

static int handle_request(struct mg_connection *conn) {
    const struct mg_request_info *request_info = mg_get_request_info(conn);

    if (strcmp(request_info->uri, "/") == 0) {
        if (request_info->request_method && strcmp(request_info->request_method, "POST") == 0) {
            // Handle form submission (user's guess)
            const char *guess = mg_get_var(conn, "guess", NULL);

            int found = 0;
            if (guess && strlen(guess) == 1) {
                for (int i = 0; i < wordLength; i++) {
                    if (wordToGuess[i] == guess[0]) {
                        guessedLetters[i] = 1;
                        found = 1;
                    }
                }
            }

            if (!found) {
                attempts++;
            }
        }

        // Display Hangman game
        mg_send_head(conn, 200, 0, "Content-Type: text/html");
        mg_printf_data(conn, "%s", html_start);

        printHangman(conn);
        printWordStatus(conn);

        // Display the form for user input
        mg_printf_data(conn, "<form method=\"post\" action=\"/\">");
        mg_printf_data(conn, "<label for=\"guess\">Enter your guess:</label>");
        mg_printf_data(conn, "<input type=\"text\" name=\"guess\" maxlength=\"1\">");
        mg_printf_data(conn, "<input type=\"submit\" value=\"Submit\">");
        mg_printf_data(conn, "</form>");

        mg_printf_data(conn, "%s", html_end);

        return 1;
    }

    return 0; // Return 0 to indicate Civetweb should handle the request
}

int main(void) {
    struct mg_server *server = mg_create_server(NULL, NULL);
    mg_set_request_handler(server, "/", handle_request);

    initializeGame();

    printf("Starting Civetweb on port 8080...\n");
    while (1) {
        mg_poll_server(server, 1000);
    }

    mg_destroy_server(&server);
    return 0;
}
