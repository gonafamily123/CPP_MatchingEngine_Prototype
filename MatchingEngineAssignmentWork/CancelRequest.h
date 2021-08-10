#ifndef CANCELREQUEST_H
#define CANCELREQUEST_H
#include <iostream>

/*
**  Simple class to create cancel Request
**  With bare minimum fields
**  To cancel order from order book we need Side, OrigClOrdID 
*/

class CancelRequest {

public:
	CancelRequest(std::string origClOrdID1, char side) :
		m_ClOrdID(origClOrdID1 + ".1"),   // Generate a new ClOrdID based on old ClOrdID
		m_Side(side), 
		m_MsgType('F'),                   // Hardcoded for now
		m_OrigClOrdID(origClOrdID1) {

	}

	CancelRequest(const CancelRequest& b) = delete;
	CancelRequest& operator=(const CancelRequest& other) = delete;

	friend std::ostream& operator<<(std::ostream& os, const CancelRequest& cReq);


	std::string getClOrdID() const {
		return m_ClOrdID;
	}

	std::string getOrigClOrdID() const {
		return m_OrigClOrdID;
	}

	char getSide() const {
		return m_Side;
	}

	char getMsgType() const{
		return m_MsgType;
	}

private:
	const std::string m_ClOrdID;
	const char m_MsgType; // Cxl 
	const char m_Side;
	const std::string m_OrigClOrdID;
};

#endif 
