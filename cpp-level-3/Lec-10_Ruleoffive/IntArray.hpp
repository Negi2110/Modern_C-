#include <string>
class IntArray
{
public:
    IntArray(std::string);
    ~IntArray();
    IntArray(const IntArray &rhs);

    IntArray &operator=(const IntArray &rhs);

    IntArray(IntArray&& source);
    IntArray &operator=(IntArray &&source);

private:
    std::string m_name;
    int *m_data;
};