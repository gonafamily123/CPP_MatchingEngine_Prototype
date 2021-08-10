#ifndef ORDERBOOKENTRY_H
#define ORDERBOOKENTRY_H
#include <iostream>

/*
**  Main OrderBook in the matching Engine - this orderbook will always be maintained in sorted order.
**  To support time priority - SeqNo is used to identify which orders arrived first.
** 
**  ClOrdID - will be used when cancelling order
**  OrderPrice - Is the key for maintaining sorted order book.
**  Side - Is necessary as buy and sell order books are kept separate
**  SeqNo - Simple incremented value indicating which order arrived first again used for sorting and time priority.
**   
*/

class OrderBookEntry {

public:

	OrderBookEntry() = delete;

	OrderBookEntry(std::string clid, int ordPrice, char side) :
		m_ClOrdID(clid), 
		m_OrdPrice(ordPrice), 
		m_Side(side), 
		m_SeqNo(++sm_NextID) {
	}

	OrderBookEntry(const OrderBookEntry& ob) : 
		m_ClOrdID(ob.m_ClOrdID), 
		m_OrdPrice(ob.m_OrdPrice), 
		m_Side(ob.m_Side), 
		m_SeqNo(ob.m_SeqNo) {
		// Note in copy constructor we dont increment sequence number
		// We want to increment a sequence number only for new orders from client.
	}

	
	friend std::ostream& operator<<(std::ostream& os, const OrderBookEntry& ob);

	int getOrdPrice() const {
		return m_OrdPrice;
	}

	int getSeqNo() const {
		return m_SeqNo;
	}

	char getSide() const {
		return m_Side;
	}

	std::string getClOrdID() const {
		return m_ClOrdID;
	}

	int getNextID() const{
		return sm_NextID;
	}

private:
	static int sm_NextID;
	const int  m_SeqNo;
	const int  m_OrdPrice;
	const char m_Side;
	const std::string m_ClOrdID;
};

#endif 
