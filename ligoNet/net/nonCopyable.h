#ifndef __DONKEY_NONCOPYABLE_H__
#define __DONKEY_NONCOPYABLE_H__

namespace donkey
{
class Noncopyable
{
public:
    Noncopyable() = default;
    virtual ~Noncopyable() = default;

private:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
    Noncopyable(const Noncopyable&&) = delete;
    Noncopyable& operator=(const Noncopyable&&) = delete;
};
}

#endif