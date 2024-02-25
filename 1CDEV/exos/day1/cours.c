#include <stdio.h>

void urlYoutube();
void timestamp();

int main()
{
    urlYoutube();
    timestamp();
    return 0;
}

void urlYoutube() {
    printf("https://www.youtube.com/watch?v=%cWAOx%cOmR%d\n", 51, 0x4B, 'Z');
}

void timestamp() {
    printf("Timestamp : %s\n", __TIMESTAMP__);
}