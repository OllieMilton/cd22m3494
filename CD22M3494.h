#ifndef CD22M3494_H
#define CD22M3494_H

#include <mbed.h>
#include <map>
#include <string>

/*
X ADDRESS 
AX3 AX2 AX1 AX0 X SWITCH 
 0   0   0   0    X0
 0   0   0   1    X1
 0   0   1   0    X2
 0   0   1   1    X3
 0   1   0   0    X4
 0   1   0   1    X5
 0   1   1   0    X12
 0   1   1   1    X13
 1   0   0   0    X6
 1   0   0   1    X7
 1   0   1   0    X8
 1   0   1   1    X9
 1   1   0   0    X10
 1   1   0   1    X11
 1   1   1   0    X14
 1   1   1   1    X15
*/

#define X0 0x00
#define X1 0x01
#define X2 0x02
#define X3 0x03
#define X4 0x04
#define X5 0x05
#define X12 0x06
#define X13 0x07
#define X6 0x08
#define X7 0x09
#define X8 0x0A
#define X9 0x0B
#define X10 0x0C
#define X11 0x0D
#define X14 0x0E
#define X15 0x0F

/*
Y ADDRESS
AY2 AY1 AY0 Y SWITCH
 0   0   0    Y0
 0   0   1    Y1
 0   1   0    Y2
 0   1   1    Y3
 1   0   0    Y4
 1   0   1    Y5
 1   1   0    Y6
 1   1   1    Y7
*/

#define Y0 0x0
#define Y1 0x1
#define Y2 0x2
#define Y3 0x3
#define Y4 0x4
#define Y5 0x5
#define Y6 0x6
#define Y7 0x7

#define MAX_X 0x0F
#define MAX_Y 0x7

class CD22M3494 {

public:

    CD22M3494(PinName x0, PinName x1, PinName x2, PinName x3, PinName y0, PinName y1, PinName y2, PinName d, PinName str, PinName rs) {
        xbus = new BusOut(x0, x1, x2, x3);
        ybus = new BusOut(y0, y1, y2);
        data = new DigitalOut(d);
        strobe = new DigitalOut(str);
        reset = new DigitalOut(rs);
        data->write(0);
        strobe->write(0);
        reset->write(0);
    }
    
    ~CD22M3494() {
        delete xbus;
        delete ybus;
        delete data;
        delete strobe;
        delete reset;
        associationTable.clear();
        delete &associationTable;
    }
    
    bool crossPointConnect(unsigned short x, unsigned short y) {
        if (x <= MAX_X && y <= MAX_Y) {
            data->write(1);
            xbus->write(x);
            ybus->write(y);
            // let the data busses settle before setting strobe high
            wait_us(2);
            strobe->write(1);
            // strobe must be high for at least 20 ns
            wait_us(1);
            strobe->write(0);       
            return true; 
        }
        return false;
    }

    bool crossPointDisconnect(unsigned short x, unsigned short y) {
        if (x <= MAX_X && y <= MAX_Y) {
            data->write(0);
            xbus->write(x);
            ybus->write(y);
            // let the data busses settle before setting strobe high
            wait_us(2);
            strobe->write(1);
            // strobe must be high for at least 20 ns
            wait_us(1);
            strobe->write(0);
            return true;        
        }
        return false;
    }
    
    void associate(string name, unsigned short xp1, unsigned short xp2) {
        unsigned short *xps = new unsigned short[2];
        xps[0] = xp1;
        xps[1] = xp2;
        associationTable[name] = xps;
    }
    
    bool routeAssociations(string src, string dst) {
        if (associationTable.count(src) && associationTable.count(dst)) {
            crossPointConnect(associationTable[src][0], associationTable[dst][0]);
            crossPointConnect(associationTable[src][1], associationTable[dst][1]);
            return true;
        }
        return false;    
    }
    
    bool unRouteAssociations(string src, string dst) {
        if (associationTable.count(src) && associationTable.count(dst)) {
            crossPointDisconnect(associationTable[src][0], associationTable[dst][0]);
            crossPointDisconnect(associationTable[src][1], associationTable[dst][1]);
            return true;
        }
        return false;            
    }
    
private:
    BusOut *xbus;
    BusOut *ybus;
    DigitalOut *data;
    DigitalOut *strobe;
    DigitalOut *reset;      
    map<string, unsigned short*> associationTable; 
};


#endif