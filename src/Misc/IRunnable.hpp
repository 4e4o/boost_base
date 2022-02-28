#ifndef IRUNNABLE_H
#define IRUNNABLE_H

class IRunnable {
public:
    virtual ~IRunnable() { }

    virtual void start() = 0;
    virtual void stop() = 0;
};

#endif // IRUNNABLE_H
