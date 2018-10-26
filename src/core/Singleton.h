#ifndef SINGLETON_H
#define SINGLETON_H

namespace gplay {

/**
 * Singleton template.
 *
 * Use this to create a singleton class.
 * ex : typedef Singleton<MyClass> NewName;
 *
 */
template <class T>
class Singleton
{
public:
    static T * getInstance()
    {
        static T instance;
        return &instance;
    }

protected:
    Singleton() {}
    ~Singleton() {}

private:
    Singleton(const Singleton &) {}
    Singleton &operator=(const Singleton &) { return *this; }
};

}

#endif // SINGLETON_H
