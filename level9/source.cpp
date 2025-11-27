#include <iostream>
#include <cstring>
#include <cstdlib>

class N {
public:
    int value = 0;
    char annotation[100];

    N(int v) {
        this->value = v;
    }

    int operator+(N &other) {
        return this->value + other.value;
    }

    int operator-(N &other) {
        return this->value - other.value;
    }

    void setAnnotation(const char *str) {
        size_t len = strlen(str);
        memcpy(this->annotation, str, len);
    }
};


int main(int argc, char **argv)
{
    if (argc < 2) {
        exit(1);
    }
    N *a = new N(5);
    N *b = new N(6);

    a->setAnnotation(argv[1]);
    int result = (*reinterpret_cast<int (**)(N*, N*)>(*reinterpret_cast<void **>(b)))(b, a);
    std::cout << "Result = " << result << std::endl;
    delete a;
    delete b;
    return 0;
}
