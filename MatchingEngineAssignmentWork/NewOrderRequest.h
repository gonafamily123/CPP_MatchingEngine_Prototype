#ifndef NEWORDERREQUEST_H
#define NEWORDERREQUEST_H

#include <iostream>
#include "Symbol.h"
////#include "OrderBookEntry.h"
////#include "SimpleMessage.h"

class NewOrderRequest {

public:

	// TBD : Refactor Constructor is too many params..
	
	NewOrderRequest(std::string clOrdID1, std::string clientID1, std::string exchangeID,
		char msgType1, char side1, std::string ricCode, int px1, int qty1, char type1) :

		m_ClOrdID(clOrdID1), 
		m_ClientID(clientID1), 
		m_ExchangeID(exchangeID),
		m_MsgType(msgType1), 
		m_Side(side1),
		m_RicCode(ricCode), 
		m_OrdPrice(px1), 
		m_OrdQty(qty1), 
		m_OrdType(type1) {

		/*
		std::cout << "Creating .." << std::endl;
		std::cout << "\t" << clOrdID1 << "\t" << msgType1 << "\t" << side1 << "\t" << m_RicCode << "\t" << px1 << "\t" <<
			qty1 << "\t" << type1 << "\t" << std::endl;
		*/
	}

	////NewOrderRequest(const NewOrderRequest& b) = default;

	/*
	NewOrderRequest& operator=(const NewOrderRequest& other) {
		std::cout << "Operator = called" << std::endl;
		// We are only allowed to change Qty;
		m_OrdQty = other.m_OrdQty;
		return *this;
	}*/

	friend std::ostream& operator<<(std::ostream& os, const NewOrderRequest& num);

	std::string getClOrdID() const {
		return m_ClOrdID;
	}

	std::string getClientID() const {
		return m_ClientID;
	}

	std::string getExchangeID() const {
		return m_ExchangeID;
	}
	
	char getMsgType() const {
		return m_MsgType;
	}

	int getOrdPrice() const {
		return m_OrdPrice;
	}

	void setOrdQty(int qty) {
		m_OrdQty = qty;
	}

	int getOrdQty() const {
		return m_OrdQty;
	}

	char getOrdType() const {
		return m_OrdType;
	}

	char getSide() const {
		return m_Side;
	}

	std::string getRicCode() const {
		return m_RicCode;
	}

private:
	const std::string m_ClOrdID;
	const std::string m_ClientID;
	const std::string m_ExchangeID;
	const char m_MsgType;
	const char m_Side;
	const std::string m_RicCode;
	const int m_OrdPrice;
	int m_OrdQty;					// This is only field amendable internally 
	const char m_OrdType;
};

#endif 

