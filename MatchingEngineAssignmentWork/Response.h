#ifndef RESPONSE_H
#define RESPONSE_H
#include <iostream>
#include "CancelRequest.h"
#include "NewOrderRequest.h"

/*
** Basic class - to capture all possible responses
** To be refactored
** Note : For execution we choose fill price based on which order was provider and which order was taker during matching.
** Although matching engine decides this we just capture here.
**
** ExchangeExecID - represents the seqno of the orders in the matching engine and is used on all responses
** 
*/

enum ResponseType {
	NEW_ACK,
	NEW_REJECT,
	CANCEL_ACK,
	CANCEL_REJECT,
	PARTIAL_FILL,
	FILL
};

class Response {

public:

	// Common Constructor : NEW_ACK , NEW_REJECT
	// TBD : Refactor
	Response(std::string reqClordID, ResponseType response, int exchExecID,
		int ordQty, int ordPrice, char ordType) :
	
		m_ClOrdID(reqClordID), 
		m_ResponseType(response),
		m_ExchangeExecutionID(exchExecID),
		m_OrdQty (ordQty),
		m_OrdPrice (ordPrice),
		m_OrdType (ordType) {

		m_LastPrice=0;
		m_LastQty=0;
		m_LiquidityProvider=false;
		m_LastMkt="";
		m_TextReason="";
	}

	// Constructor for CANCEL_ACK, CANCEL_REJECT
	Response(std::string reqClordID, ResponseType response, int exchExecID) :
	
		m_ClOrdID(reqClordID),
		m_ResponseType(response),
		m_ExchangeExecutionID(exchExecID) {

		m_OrdQty = 0;
		m_OrdPrice = 0;
		m_OrdType = 'L';
		m_LastPrice = 0;
		m_LastQty = 0;
		m_LiquidityProvider = false;
		m_LastMkt = "";
		m_TextReason = "";
	}

	// Constructor for PARTIAL_FILL, FILL
	Response(std::string reqClordID, ResponseType response, int exchExecID,
	    int lastPrice, int lastQty, std::string lastMkt, 
		bool liquidityProvider) :

		m_ClOrdID(reqClordID),
		m_ResponseType(response),
		m_ExchangeExecutionID(exchExecID),
		m_LastPrice (lastPrice),
		m_LastQty (lastQty),
		m_LastMkt (lastMkt),
		m_LiquidityProvider(liquidityProvider) {

		m_OrdQty = 0;
		m_OrdPrice = 0;
		m_OrdType = 'L';

	}

	Response(const Response& b) = default;

	Response& operator=(const Response& other) = default;

	friend std::ostream& operator<<(std::ostream& os, const Response& res);

	// 
	void setTextReason(std::string textReason) {
		m_TextReason = textReason;
	}

private:
	const std::string  m_ClOrdID;
	const ResponseType m_ResponseType;
	const int          m_ExchangeExecutionID;   // This is the original Seqno of the order in order book

	int			m_OrdPrice;
	int			m_OrdQty;
	char		m_OrdType;
	
 	int			m_LastPrice;					// Only on fills 
	int			m_LastQty;						// Only on fills
	bool		m_LiquidityProvider;			// Only on fills - To indicate Taker or Provider
	std::string m_LastMkt;						// Only on fills  

	std::string m_TextReason;					// Details
};

#endif 
