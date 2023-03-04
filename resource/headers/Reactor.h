

class EventHandler
{
public:

    ~EventHandler() = default;
    virtual ssize_t handleRead() = 0;
    virtual ssize_t handleWrite() = 0;
    virtual int getFd() = 0;
};