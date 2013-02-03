#include <stdio.h>

int main(int argc, const char *argv[])
{
    int i = 0;
    int c = 0;
    while (i < 11) {
        c += i;
        i ++;
    }

    if (c > 100) {
        c = 100;
    }

    return 0;
}

