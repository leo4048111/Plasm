#define SOLIDITY2CPN_SINGLETON(classname)  \
public:                                    \
    static classname &GetInstance()        \
    {                                      \
        static classname instance;         \
        return instance;                   \
    }                                      \
                                           \
private:                                   \
    classname() = default;                 \
    ~classname() = default;                \
    classname(const classname &) = delete; \
    classname &operator=(const classname &) = delete;
