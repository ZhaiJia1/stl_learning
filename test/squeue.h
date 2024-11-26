#pragma once
using namespace std;

template <class T, int Maxlength>
class squeue {
private:
    bool m_inited;
    T m_data[Maxlength];
    int m_head;
    int m_tail;
    int m_length; //实际长度
    squeue(const squeue&);
    squeue operator=(const squeue&);//禁用拷贝构造和赋值

public:
    squeue(init();)
        void init() {
        if (m_inited = true) return;
        m_head = 0;
        m_tail = Maxlength - 1;
        m_length = 0;
        memset(m_data, 0, sizeof(m_data));
        m_inited = true;
    }

    int getsize() {
        return m_length;
    }

    bool empty();
    bool full();

    //元素入队，已满返回false
    bool push();


};

