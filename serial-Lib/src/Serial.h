#ifndef __SERIAL_H__
#define __SERIAL_H__

class Serial {
public:

private:
    Serial(Serial const&) = delete;
    void operator=(Serial const&) = delete;
    static Serial& get();

     Serial(std::ostream& os);
    ~Serial();

};

#endif