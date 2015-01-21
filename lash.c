#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>

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

char *__findenv(const char *name, int *offset) {
    // lifted from libc
    // http://mirror.fsf.org/pmon2000/3.x/src/sdk/libc/stdlib/getenv.c

    extern char **environ;
    register int len, i;
    register const char *np;
    register char **p, *cp;

    if (name == NULL || environ == NULL)
        return (NULL);
    for (np = name; *np && *np != '='; ++np)
        ;
    len = np - name;
    for (p = environ; (cp = *p) != NULL; ++p) {
        for (np = name, i = len; i && *cp; i--)
            if (*cp++ != *np++)
                break;
        if (i == 0 && *cp++ == '=') {
            *offset = p - environ;
            return (cp);
        }
    }
    return (NULL); 
}

char *getenv(const char *name) {
    // lifted from libc
    // http://mirror.fsf.org/pmon2000/3.x/src/sdk/libc/stdlib/getenv.c
    
    int offset;
    return(__findenv(name, &offset));
}

char *cmd_path(const char *path, const char *cmd) {
    // Check if a cmd exists in path. path is split by semicolon
    unsigned int start = 0;
    unsigned int end = 0;

    char full_path[1024];
    char path_dir[1024];

    while (end < strlen(path)) {
        // Increment end until we find a semicolon
        while(path[end] != ':') {
            end++;
        }

        // Copy string from index start to index end into full_path
        unsigned int i;
        for (i = start; i < end; i++) {
            path_dir[i] = path[start];
            start++;
        }

        // Increment start and end to skip over the semicolon
        start++;
        end++;

        // Add the cmd onto full path
        path_dir[i] = '\0';

        sprintf(full_path, "%s/%s", path_dir, cmd);
        printf("%s\n", full_path);
        fflush(STDOUT);
    }

    return NULL;
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
        char *path = getenv("PATH");
        cmd_path(path, cmd);
        char env[1][4096];
        sprintf(env[0], "PATH=%s", path);
        execle(cmd, cmd, env);
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
