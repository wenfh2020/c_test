/* gcc test.c -o test && ./test */
#include <stdio.h>

int get_last_str_len(char* p) {
    char *p1, *p2, *cur;
    cur = p;

    while (*cur != '\0') {
        /* find not of. */
        while (*cur == ' ') {
            cur++;
        }
        p1 = cur;

        /* find of. */
        while (*cur != ' ' && *cur != '\0') {
            cur++;
        }
        p2 = (*cur != '\0') ? cur++ : cur;
    }

    return p2 - p1;
}

int main() {
    char s[] = "1 23    456 ";
    printf("%d\n", get_last_str_len(s));
    return 0;
}