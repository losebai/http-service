

class EventHandler
{
public:

    ~EventHandler() = default;
    virtual void handleRead() = 0;
    virtual void handleWrite() = 0;
    virtual int getFd() = 0;
};