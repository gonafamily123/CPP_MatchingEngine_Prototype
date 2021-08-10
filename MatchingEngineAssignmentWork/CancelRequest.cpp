#include "CancelRequest.h"

std::ostream& operator<<(std::ostream& os, const CancelRequest& cReq) {
    os <<
        cReq.getClOrdID() << ":" <<
        cReq.getMsgType() << ":" <<
        cReq.getSide() << ":" <<
        cReq.getOrigClOrdID() ;

    return os;
}