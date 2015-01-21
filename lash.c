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

void exec_cmd(const char *cmd) {
    pid_t process;
    process = fork();

    if (process < 0 )  {
        say("Couldn't fork!\n");
        exit_shell = true;        
    } else if (process == 0) {
        // We are child process so exec. First arg is path to binary and then
        // path we used to execute it
        execl(cmd, cmd, (char *)0);
    } else {
        // We are parent so wait for our child to complete 
        int child_status = 0;
        wait(&child_status);
        if (child_status == -1) {
            say("Error when executing child!\n");
            exit_shell = true;       
        }
    }
}

void process(const char *input) {
    if (streq(input, "help")) {
        say("You asked for help. What a n00b.\n");
    } else if ((streq(input, "exit")) || (streq(input, "quit"))) {
        exit_shell = true;
    } else if (str_startswith(input, "touch")) {
        say("Would create file\n");
    } else {
        // Assume it's an external program if it's not one of our keywords
        exec_cmd(input);
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
