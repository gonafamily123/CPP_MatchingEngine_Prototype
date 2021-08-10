#include "OrderBookEntry.h"

// Sequence Number
int OrderBookEntry::sm_NextID = 0;

std::ostream& operator<<(std::ostream& os, const OrderBookEntry& ob) {
    os <<
    ob.getSide()        << ":" <<
    ob.getOrdPrice()    << ":" <<
    ob.getSeqNo()       << ":" << 
    ob.getClOrdID()     << ":" << 
    ob.getNextID()      ;
    return os;
}

