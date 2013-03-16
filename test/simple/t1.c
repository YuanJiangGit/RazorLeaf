
void A()
{
    int i;
    int count = 10;
    int sum = 0;
    for (i = 0; i < count; i++) {
        sum += i;
    }
}

int B()
{
#define ARRAY_SIZE 10
    int i;
    int a[ARRAY_SIZE];
    for (i = 0; i < ARRAY_SIZE; i++) {
        a[i] = i*i;
    }
    return 0;
}

void C() 
{
    int i=0;
    int sum = 0;
    sum = 10;
    while(i<10) {
        sum += i;
    }
}

int main()
{

    return 0;
}
