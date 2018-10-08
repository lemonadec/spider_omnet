//
// Generated file, do not edit! Created by nedtool 5.4 from simpleMsg.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __SIMPLEMSG_M_H
#define __SIMPLEMSG_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>simpleMsg.msg:16</tt> by nedtool.
 * <pre>
 * //
 * // Same as TictocMsg11
 * //
 * message simpleMsg
 * {
 *     int source;
 *     int destination;
 *     int hopCount = 0;
 *     int size;
 * }
 * </pre>
 */
class simpleMsg : public ::omnetpp::cMessage
{
  protected:
    int source;
    int destination;
    int hopCount;
    int size;

  private:
    void copy(const simpleMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const simpleMsg&);

  public:
    simpleMsg(const char *name=nullptr, short kind=0);
    simpleMsg(const simpleMsg& other);
    virtual ~simpleMsg();
    simpleMsg& operator=(const simpleMsg& other);
    virtual simpleMsg *dup() const override {return new simpleMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSource() const;
    virtual void setSource(int source);
    virtual int getDestination() const;
    virtual void setDestination(int destination);
    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);
    virtual int getSize() const;
    virtual void setSize(int size);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const simpleMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, simpleMsg& obj) {obj.parsimUnpack(b);}


#endif // ifndef __SIMPLEMSG_M_H

