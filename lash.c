#include <fcntl.h>
#include <stdbool.h>

#define STDIN 0
#define STDOUT 1

bool exit_shell = false;

unsigned int strlen(const char *str) {
    unsigned int i = 0;
    while (*str != '\0') {
        i++;
        str++;
    }
    return i;
}

void say(const char *str) {
    write(STDOUT, str, strlen(str));
}

bool streq(const char *str1, const char *str2) {
    while (*str1 != '\0') {
        if (*str1 != *str2) {
            return false;
        }

        str1++;
        str2++;
    }

    // str1 is now == \0
    return (*str2 == '\0');
}

bool str_startswith(const char *str, const char *match) {
    unsigned int i = 0;
    unsigned int mlen = strlen(match);
    while (i < mlen) {
        if (*str != *match) {
            return false;
        }

        i++;
        str++;
        match++;
    }

    return true;
}

void process(const char *input) {
    if (streq(input, "help")) {
        say("You asked for help. What a n00b.\n");
    } else if ((streq(input, "exit")) || (streq(input, "quit"))) {
        exit_shell = true;
    } else if (str_startswith(input, "touch")) {
        say("Would create file\n");
    } else {
        say("Unknown command!\n");
    }
}

void prompt() {
    say("lash> ");
    
    char inbuf[1024];
    unsigned int bufpos = 0;
    char c = '\0';
    
    while (c != '\n') {
        // Read one character
        read(STDIN, &c, 1);

        // Put it in the input buffer and increment the pointer
        inbuf[bufpos] = c;
        bufpos++;
    }

    // Terminate user input (overwriting newline)
    inbuf[bufpos-1] = '\0';

    process(inbuf);
}

int main(void) {
    while (!exit_shell) {
        prompt();
    }

    return 0;
}
