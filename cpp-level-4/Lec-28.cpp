// Delegating constructor
// how to leverage multiple constructors to avoid repeating yourself. The DRY principle (Don't Repeat Yourself) is a good guideline for avoiding errors.
// Often times, it can be tempting to write helper functions in classes (e.g. init) when you have multiple constructors, but those could be accidentally used by users of your interface. Instead, you can prefer to use constructors to provide a more consistent way to instantiate objects.

#include <iostream>
#include <string>

struct Entity
{
    Entity() : name{}, x{0}, y{0}, collection{nullptr}
    {
        init();
    }
    Entity(std::string _name) : name{_name}, x{0}, y{0}, collection{nullptr}
    {
        init();
        name = name;
    }
    void init()
    {
        name = std::to_string(x) + std::to_string(y);

        ////compute some algo add our enety to some data structure
    }
    std::string name;
    int x{0};
    int y{0};
    int *collection{nullptr};
};

int main()
{
    Entity e{"aman"};
    e.init();
    std::cout << e.name << std::endl;
    std::cout << e.x << std::endl;
    std::cout << e.y << std::endl;
    std::cout << e.collection << std::endl;

    return 0;
}