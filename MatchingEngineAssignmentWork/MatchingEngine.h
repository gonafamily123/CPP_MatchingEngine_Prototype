#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H

#include "Symbol.h"
#include "OrderBookEntry.h"
#include "NewOrderRequest.h"
#include "CancelRequest.h"
#include "Response.h"
#include <map>
#include <queue>
#include <memory>

/*
*  Main Class MatchingEngine
*  a. One instance Per Symbol 1:1 
*  b. Accepts new requests ( maintains in the sorted manner in the map )
*  c. Accepts cancel requests
*  d. Matching logic will be triggered only on new request
*  e. Generates various responses - NEW_ACK, NEW_REJECT, CANCEL_ACK, CANCEL_REJECT, PARTIAL_FILL ,FILL
*  f. Produces fills will relevant fields populapted uses logic to calculate lastPrice and Liquidity Provider.
*  g. Does basic validation on new orders and rejects if validation failed
*/

class MatchingEngine {

public:
	MatchingEngine(Symbol symbol, std::string lastMkt) : m_Symbol(symbol), m_LastMkt(lastMkt) {

	}


	// To send a new order and this also triggers matching of the books
	bool acceptNew(NewOrderRequest newBook);

	// To cancel an existing order.
	bool acceptCancel(const CancelRequest& cancelBook);

	// Prints the buy and sell orderbook in sorted manner.
	void showMatchingEngineState(std::ostream& os);

	// Publishes all responses for verification
	const std::deque <Response>& getLastMatchingResults() {
		return lastMatchingResults;
	}

	// This is mainly for testing to clear the deque of all outstanding responses
	void clearLastMatchingResults() {
		lastMatchingResults.clear();
	}

	Symbol getSymbol() {
		return m_Symbol;
	}

	std::string getLastMkt() {
		return m_LastMkt;
	}

private:
	void runMatchinglogic(NewOrderRequest req);

	void generateCancelAck(const CancelRequest& cancelRequest , int seqNo);
	
	void generateCancelReject(const CancelRequest& cancelRequest);
	
	bool handleNewOrderValidations(const NewOrderRequest& newRequest);
	
	void sendFillMessages(NewOrderRequest b1, int seqNo1, NewOrderRequest b2, int seqNo2, int fillQty);
	
	std::shared_ptr<OrderBookEntry> getOrderEntryKey(NewOrderRequest req);

	class BookComparator {
	public:

		bool operator()(const OrderBookEntry& lhs, const OrderBookEntry& rhs) const
		{
	
			// If both are Mkt , sort based on arrival sequence
			if (lhs.getOrdPrice() == 0 && rhs.getOrdPrice() == 0) {
				return lhs.getSeqNo() < rhs.getSeqNo();
			}

			// lhs is lmt and rhs is mkt
			if (lhs.getOrdPrice() > 0 && rhs.getOrdPrice() == 0) {
				return false;
			}

			// lhs is mkt and rhs is lmt 
			if (lhs.getOrdPrice() == 0 && rhs.getOrdPrice() > 0) {
				return true;
			}

			if (lhs.getSide() == 'B') {
				
				if (lhs.getOrdPrice() < rhs.getOrdPrice())
					return false;
				else if (lhs.getOrdPrice() > rhs.getOrdPrice())
					return true;

			}
			else {
				if (lhs.getOrdPrice() < rhs.getOrdPrice())
					return true;
				else if (lhs.getOrdPrice() > rhs.getOrdPrice())
					return false;
			}
			return lhs.getSeqNo() < rhs.getSeqNo();

		}
	};

private:
	Symbol m_Symbol;
	std::string m_LastMkt;
	std::deque <Response> lastMatchingResults;
	std::map < OrderBookEntry, NewOrderRequest, BookComparator> buyBookEntries;
	std::map < OrderBookEntry, NewOrderRequest, BookComparator> sellBookEntries;

};

#endif // !MATCHINGENGINE_H
