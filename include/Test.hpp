class Test
{
public:
    size_t size;

    template <typename T>
    Test(T *test) : size(sizeof(test)) {};
    ~Test() = default;
};