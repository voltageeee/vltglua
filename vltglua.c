#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int waiting_for_end = 0;
int line_num = 1;

// string_replace replaces FIRST occurancies of "what" with "with" in "str"
void string_replace(char* str, char* what, char* with) {
    char *pos, temp[1000];
    int index = 0;
    int owlen;
    int found = 0;

    owlen = strlen(what);


    while ((pos = strstr(str, what)) != NULL && !found) {
        strcpy(temp, str);
        index = pos - str;
        str[index] = '\0';
        strcat(str, with);
        strcat(str, temp + index + owlen);
        found = 1;
    }
}

// handle_inc_dec handles increment and decrement instructions such as a++, a--, a += 5, a -= 5
// types: 0 = ++, 1 = --, 2 = +=, 3 = -=
void handle_inc_dec(char* str, int type) {
    char tmp_buf[64] = {0};
    char strtmp[128] = {0};
    char strtmp2[255] = {0};

    int var_len = 0;
    while (str[var_len] != '+' && str[var_len] != '-' && str[var_len] != '\0') {
        tmp_buf[var_len] = str[var_len];
        var_len++;
    }
    tmp_buf[var_len] = '\0';

    char* eq_ptr = strchr(str, '=');
    if ((type == 2 || type == 3) && eq_ptr != NULL) {
        strcpy(strtmp, eq_ptr + 1);
        strtmp[strcspn(strtmp, "\r\n")] = '\0';

        if (type == 2) {
            sprintf(strtmp2, "%s = %s + %s\n", tmp_buf, tmp_buf, strtmp);
        } else {
            sprintf(strtmp2, "%s = %s - %s\n", tmp_buf, tmp_buf, strtmp);
        }
        strcpy(str, strtmp2);
        return;
    }

    switch (type) {
        case 0:
            sprintf(str, "%s = %s + 1\n", tmp_buf, tmp_buf);
            break;
        case 1:
            sprintf(str, "%s = %s - 1\n", tmp_buf, tmp_buf);
            break;
    }
}

void process_vltglua(char* str) {
    int has_g_function = strstr(str, "g_func") != NULL;
    int has_l_function = strstr(str, "l_func") != NULL;
    int has_closing_brace = strstr(str, "}") != NULL;
    int has_opening_brace = strstr(str, "{") != NULL;
    int has_a_statement = strstr(str, "if") != NULL || strstr(str, "else") != NULL || strstr (str, "elseif") != NULL;
    
    if (strstr(str, "++")) {
        handle_inc_dec(str, 0);
        return;
    } else if (strstr(str, "--")) {
        handle_inc_dec(str, 1);
        return;
    } else if (strstr(str, "+=")) {
        handle_inc_dec(str, 2);
        return;
    } else if (strstr(str, "-=")) {
        handle_inc_dec(str, 3);
        return;
    }

    if (has_g_function && !waiting_for_end) {
        waiting_for_end = 1;
    } else if (has_g_function && waiting_for_end) {
        printf("unexpected function declaration at %d, } expected to close a previous function", line_num);
        exit(1);
    }

    if (has_l_function && waiting_for_end) {
        printf("unexpected function declaration at %d, } expected to close a previous function", line_num);
        exit(1);
    } else if (has_l_function && !waiting_for_end) {
        waiting_for_end = 1;
    }

    if (has_opening_brace && has_g_function || has_l_function) {
        string_replace(str, "{", "");
    }

    if (has_opening_brace && has_a_statement) {
        string_replace(str, "{", "then");
    }

    string_replace(str, "l_var", "local");
    string_replace(str, "g_var", "");
    string_replace(str, "g_func", "function");
    string_replace(str, "l_func", "local function");
    string_replace(str, "}", "end");
}

int main(int argv, char **argc) {
    if (argv != 2) {
        printf("expected one and only argument, a lua file name. example: ./vltglua file.lua\n");
        return 1;
    }

    FILE* fileptr;
    fileptr = fopen(argc[1], "r");
    if (fileptr == NULL) {
        printf("expected a valid file, but fileptr == null!\n");
        return 1;
    }

    FILE* outptr;
    outptr = fopen("out.lua", "w");
    if (outptr == NULL) {
        printf("failed to create out.lua file, outptr == null!\n");
        return 1;
    }

    char buf[255];
    while (fgets(buf, 255, fileptr)) {
        process_vltglua(buf);
        fputs(buf, outptr);
        memset(buf, 0, 255);
        line_num++;
    }

    printf("done! wrote changes to out.lua\n");
}