#ifndef CD22M3494_H
#define CD22M3494_H

#include <mbed.h>
#include <map>
#include <string>

#define LOG_LEVEL_INFO
#include "logger.h"

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
#define X0 "X0"
#define X1 "X1"
#define X2 "X2"
#define X3 "X3"
#define X4 "X4"
#define X5 "X5"
#define X6 "X6"
#define X7 "X7"
#define X8 "X8"
#define X9 "X9"
#define X10 "X10"
#define X11 "X11"
#define X12 "X12"
#define X13 "X13"
#define X14 "X14"
#define X15 "X15"


#define X0I 0x00
#define X1I 0x01
#define X2I 0x02
#define X3I 0x03
#define X4I 0x04
#define X5I 0x05
#define X12I 0x06
#define X13I 0x07
#define X6I 0x08
#define X7I 0x09
#define X8I 0x0A
#define X9I 0x0B
#define X10I 0x0C
#define X11I 0x0D
#define X14I 0x0E
#define X15I 0x0F

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

#define Y0I 0x00
#define Y1I 0x01
#define Y2I 0x02
#define Y3I 0x03
#define Y4I 0x04
#define Y5I 0x05
#define Y6I 0x06
#define Y7I 0x07

#define Y0 "Y0"
#define Y1 "Y1"
#define Y2 "Y2"
#define Y3 "Y3"
#define Y4 "Y4"
#define Y5 "Y5"
#define Y6 "Y6"
#define Y7 "Y7"

#define MAX_X 0x0F
#define MAX_Y 0x07

struct Association {
    string name;
    string xp1;
    string xp2;
};
    
/**A driver for the CD22M3494 crosspoint switch matrix.
 */
class CD22M3494 {

public:

    /**Constructs a new CD22M3494 object
     *
     * @param x0 the pin name for the first bit of the x axis address
     * @param x1 the pin name for the second bit of the x axis address
     * @param x2 the pin name for the thrid bit of the x axis address
     * @param x3 the pin name for the forth bit of the x axis address
     * @param y0 the pin name for the first bit of the y axis address
     * @param y1 the pin name for the second bit of the y axis address
     * @param y2 the pin name for the thrid bit of the y axis address
     * @param d the pin name for the data bus
     * @param str the pin name for the strobe bus
     * @param rs the pin name for reset.
     */
    CD22M3494(PinName x0, PinName x1, PinName x2, PinName x3, PinName y0, PinName y1, PinName y2, PinName d, PinName str, PinName rs) {
        xbus = new BusOut(x0, x1, x2, x3);
        ybus = new BusOut(y0, y1, y2);
        data = new DigitalOut(d);
        strobe = new DigitalOut(str);
        reset = new DigitalOut(rs);
        data->write(0);
        strobe->write(0);
        reset->write(0);
        crosspointLookup[X0] = X0I;
        crosspointLookup[X1] = X1I;
        crosspointLookup[X2] = X2I;
        crosspointLookup[X3] = X3I;
        crosspointLookup[X4] = X4I;
        crosspointLookup[X5] = X5I;
        crosspointLookup[X6] = X6I;
        crosspointLookup[X7] = X7I;
        crosspointLookup[X8] = X8I;
        crosspointLookup[X9] = X9I;
        crosspointLookup[X10] = X10I;
        crosspointLookup[X11] = X11I;
        crosspointLookup[X12] = X12I;
        crosspointLookup[X13] = X13I;
        crosspointLookup[X14] = X14I;
        crosspointLookup[X15] = X15I;
        crosspointLookup[Y0] = Y0I;
        crosspointLookup[Y1] = Y1I;
        crosspointLookup[Y2] = Y2I;
        crosspointLookup[Y3] = Y3I;
        crosspointLookup[Y4] = Y4I;
        crosspointLookup[Y5] = Y5I;
        crosspointLookup[Y6] = Y6I;
        crosspointLookup[Y7] = Y7I;
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

    /**Closes the switch at the given cross point.
     *
     * @param x the x axis
     * @param y the y axis
     */
    bool crossPointConnect(unsigned short x, unsigned short y) {
        INFO("Got x point connect for x %d, y %d", x, y);
        if (x <= MAX_X && y <= MAX_Y) {
            INFO("setting data high...");
            data->write(1);
            INFO("writing x...");
            xbus->write(x);
            INFO("writing y...");
            ybus->write(y);
            // let the data busses settle before setting obe high
            wait_us(2);
            strobe->write(1);
            // obe must be high for at least 20 ns
            wait_us(1);
            strobe->write(0); 
            INFO("Done");
            return true; 
        }
        INFO("Error");
        return false;
    }

    /**Opens the switch at the given cross point.
     *
     * @param x the x axis
     * @param y the y axis
     */
    bool crossPointDisconnect(unsigned short x, unsigned short y) {
        INFO("Got x point disconnect for x %d, y %d", x, y);
        if (x <= MAX_X && y <= MAX_Y) {
            INFO("setting data high...");
            data->write(0);
            INFO("writing x...");
            xbus->write(x);
            INFO("writing y...");
            ybus->write(y);
            // let the data busses settle before setting obe high
            wait_us(2);
            strobe->write(1);
            // obe must be high for at least 20 ns
            wait_us(1);
            strobe->write(0);
            INFO("Done");
            return true;        
        }
        INFO("Error");
        return false;
    }
    
    /**Associated the two given crosspoint with the given name, note, both crosspoint MUST be on the same axis.
     *
     * @param name the name of the association
     * @param xp1 the first crosspoint to associate with the given name
     * @param xp2 the second crosspoint to associate with the given name
     */
    bool associate(string name, string xp1, string xp2) {
        if (crosspointLookup.count(xp1) && crosspointLookup.count(xp2)) {
            Association *assoc = new Association();
            assoc->xp1 = xp1;
            assoc->xp2 = xp2;
            associationTable[name] = assoc;
            return true;
        }
        return false;
    }
    
    /**Connects all the crosspoints for the given associations.
     * 
     * @param src the source association
     * @param dst the destination to connect the source to.
     */
    bool routeAssociations(string src, string dst) {
        if (associationTable.count(src) && associationTable.count(dst)) {
            crossPointConnect(crosspointLookup[associationTable[src]->xp1], crosspointLookup[associationTable[dst]->xp1]);
            crossPointConnect(crosspointLookup[associationTable[src]->xp2], crosspointLookup[associationTable[dst]->xp2]);
            return true;
        }
        return false;    
    }
    
    /**Disconnects all the crosspoints for the given associations.
     * 
     * @param src the source association
     * @param dst the destination to disconnect from the source.
     */
    bool unRouteAssociations(string src, string dst) {
        if (associationTable.count(src) && associationTable.count(dst)) {
            crossPointDisconnect(crosspointLookup[associationTable[src]->xp1], crosspointLookup[associationTable[dst]->xp1]);
            crossPointDisconnect(crosspointLookup[associationTable[src]->xp2], crosspointLookup[associationTable[dst]->xp2]);
            return true;
        }
        return false;            
    }
    
    Association* getAssociation(string name) {
        return associationTable[name];
    }
            
private:
    BusOut *xbus;
    BusOut *ybus;
    DigitalOut *data;
    DigitalOut *strobe;
    DigitalOut *reset;    
    map<string, Association*> associationTable;
    map<string, unsigned short> crosspointLookup; 
};


#endif