// pimpl
// pointer to implementataion
// why???
#include <string>
#include <iostream>
#include <memory>
class Person
{
public:
    Person(std::string s);
    ~Person();
    std::string GetAttributes();

private:
    struct pImplPerson;
    //pImplPerson *m_pImpl;
    std::unique_ptr<pImplPerson> m_pImplUnique; // unique pointer to manage memory automatically
};

struct Person::pImplPerson
{
public:
    pImplPerson(std::string name, std::string strength, std::string speed)
        : m_name(name), m_strength(strength), m_speed(speed) {}

    std::string m_name;
    std::string m_strength;
    std::string m_speed;
};

Person::Person(std::string s)
{
    //m_pImpl = new pImplPerson(s, "strong", "fast");
    m_pImplUnique = std::make_unique<pImplPerson>(s, "strong", "fast");
    // m_pImpl->m_name = s;
    // m_pImpl->m_strength = "strong";
    // m_pImpl->m_speed = "fast";
}

Person::~Person()
{
    
}
// why geeter or setter dont need person::?
std::string Person::GetAttributes()
{
    return m_pImplUnique->m_name + " is " + m_pImplUnique->m_strength + " and " + m_pImplUnique->m_speed;
}

int main()
{
    Person p("John");
    std::cout << p.GetAttributes() << std::endl;
    return 0;
}