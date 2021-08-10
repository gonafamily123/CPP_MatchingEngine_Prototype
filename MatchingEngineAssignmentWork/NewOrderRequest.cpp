#include "NewOrderRequest.h"

std::ostream& operator<<(std::ostream& os, const NewOrderRequest& req) {
	return os << 
		req.getClOrdID()	<< ":" <<
		req.getClientID()	<< ":" <<
		req.getExchangeID() << ":" <<
		req.getMsgType()	<< ":" <<
		req.getSide()		<< ":" <<
		req.getRicCode()	<< ":" <<
		req.getOrdPrice()	<< ":" << 
		req.getOrdQty()		<< ":" <<
		req.getOrdType() ;
}