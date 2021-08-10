#include "MatchingEngine.h"
#include <memory>
#include <list>
#include <memory>
#include "Response.h"

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


void MatchingEngine::runMatchinglogic(NewOrderRequest req) {

	// Prepare a key for hashmap using request message
	auto sp2 = getOrderEntryKey(req);
	
	// If no other problem - ack the order..
	Response newAck(req.getClOrdID(), ResponseType::NEW_ACK, 
		sp2->getSeqNo(), req.getOrdQty(), req.getOrdPrice(), req.getOrdType());

	// Store ack message
	lastMatchingResults.push_back(newAck);

	char side = sp2->getSide();
	std::map<OrderBookEntry, NewOrderRequest, BookComparator>::iterator iters, itere;

	// If it is a buy order - look for match in sell book ( and vice versa )
	if (side == 'B') {
		iters = sellBookEntries.begin();
		itere = sellBookEntries.end();
	} 
	else if (side == 'S') {
		iters = buyBookEntries.begin();
		itere = buyBookEntries.end();
	}

	while (iters != itere) {
		auto fkey = iters->first;
		auto sdata = iters->second;

		
		if (fkey.getOrdPrice() == 0 || sp2->getOrdPrice() == 0 ||
			((side == 'B' && sp2->getOrdPrice() >= fkey.getOrdPrice()) ||
				((side == 'S' && sp2->getOrdPrice() <= fkey.getOrdPrice()))))
		{
			// Match found  - now check the category of the match found.

			if (sdata.getOrdQty() == req.getOrdQty()) {
				// 1. Exact Qty match case 
				std::cout << "Perfect - Exact Qty found !" << std::endl;
				sendFillMessages(sdata, fkey.getSeqNo(), req, sp2->getSeqNo(), req.getOrdQty());
				if (side == 'B') {
					sellBookEntries.erase(iters++);
				}
				else if (side == 'S') {
					buyBookEntries.erase(iters++);
				}
				req.setOrdQty(0);

				break;
			} else if (sdata.getOrdQty() > req.getOrdQty()) {
				// 2 . New Request matched with bigger order in order book.
				std::cout << "Order Book Qty is more!" << std::endl;
				sendFillMessages(sdata, fkey.getSeqNo(), req,sp2->getSeqNo(), req.getOrdQty());
				req.setOrdQty(0);
				sdata.setOrdQty(sdata.getOrdQty() - req.getOrdQty());
				(*iters).second.setOrdQty(sdata.getOrdQty() - req.getOrdQty());

				break;
			}
			else if (sdata.getOrdQty() < req.getOrdQty()) {
				// 3 . New Request is bigger than order matched in order book.
				// Keep looping to find if any more orders match in the orderbook.
				std::cout << "Incoming request Qty is more!" << std::endl;
				sendFillMessages(sdata,fkey.getSeqNo(), req, sp2->getSeqNo(), sdata.getOrdQty());
				req.setOrdQty(req.getOrdQty() - sdata.getOrdQty());

				if (side == 'B') {
					sellBookEntries.erase(iters++);
				}
				else if (side == 'S') {
					buyBookEntries.erase(iters++);
				}
			}
		}else {
			// If the top of the order book does not match no point 
			// iterating the rest of the orders.
			break;
		}

	}

	// Even after matching if any quantity is left from the new order then add it to order book.
	if (req.getOrdQty() > 0) {

		if (req.getSide() == 'B') {
			buyBookEntries.insert(std::pair<OrderBookEntry, NewOrderRequest>(*sp2, req));
		}
		else if (req.getSide() == 'S') {
			sellBookEntries.insert(std::pair<OrderBookEntry, NewOrderRequest>(*sp2, req));
		}

	}

}

bool MatchingEngine::acceptNew(NewOrderRequest newRequest) {

	//1. Create Order Entry Key From Request
	//2. Add them to order book
	//3. Check they are added appropriately by printing
	//4. Then run matching logic.

	if (handleNewOrderValidations(newRequest)) {
		runMatchinglogic(newRequest);
		return true;
	}
	return false;
}


bool MatchingEngine::acceptCancel(const CancelRequest& cancelRequest) {

	std::map<OrderBookEntry, NewOrderRequest, BookComparator>::iterator iters, itere;
	std::cout << "Searching for " << cancelRequest.getOrigClOrdID() << "\t" << cancelRequest.getSide() << "\t" << cancelRequest.getClOrdID()
		<< "\t" << std::endl;
	char side = cancelRequest.getSide();
	if (side == 'B') {
		iters = buyBookEntries.begin();
		itere = buyBookEntries.end();

	}
	else if (side == 'S') {
		iters = sellBookEntries.begin();
		itere = sellBookEntries.end();
	}

	while (iters != itere) {
		auto fkey = iters->first;
		std::cout << "Comparing with " << fkey << "\t" << cancelRequest.getOrigClOrdID() << std::endl;
		if (fkey.getClOrdID() == cancelRequest.getOrigClOrdID()) {
			if (side == 'B') {
				buyBookEntries.erase(iters++);
			}
			else if (side == 'S') {
				sellBookEntries.erase(iters++);
			}
			// We dont expect more than one matching case ..
			generateCancelAck(cancelRequest, fkey.getSeqNo());
			return true;
		}
		++iters;
	}
	generateCancelReject(cancelRequest);
	return false;
}

void MatchingEngine::showMatchingEngineState(std::ostream& os) {

	for (auto const& f : buyBookEntries) {
		os << ":BUY BOOK:" << f.first << "-->" << f.second << ":\n";
	}

	for (auto const& f : sellBookEntries) {
		os << ":SELL BOOK:" << f.first << "-->" << f.second << ":\n";
	}

}



bool MatchingEngine::handleNewOrderValidations(const NewOrderRequest& newRequest) {

	// Any possible validations and rejects..
	std::string textReason;
	bool isRejected = false;

	if (newRequest.getRicCode() != this->getSymbol().getRicCode()) {
		textReason = "Invalid Symbol;";
		isRejected = true;
	}else if (newRequest.getOrdQty() % this->getSymbol().getLotSize() != 0) {
		textReason = "Invalid LotSize;";
		isRejected = true;
	} else if (newRequest.getOrdPrice() > 0) {
		if (newRequest.getOrdPrice() > this->getSymbol().getHigh() ||
			newRequest.getOrdPrice() < this->getSymbol().getLow()) {
			textReason += "Invalid Price - Cannot be more than symbol high-Low limit;";
			isRejected = true;
		}
	}

	if (isRejected) {
		Response rejectAck(newRequest.getClOrdID(), ResponseType::NEW_REJECT,
			0, newRequest.getOrdQty(), newRequest.getOrdPrice(), newRequest.getOrdType());

		rejectAck.setTextReason(textReason);
		lastMatchingResults.push_back(rejectAck);
		return false;
	}

	
	return true;
}

void MatchingEngine::generateCancelAck(const CancelRequest& cancelRequest, int seqNo) {

	Response res(cancelRequest.getClOrdID(), ResponseType::CANCEL_ACK, seqNo);
	res.setTextReason("");
	lastMatchingResults.push_back(res);

}

void MatchingEngine::generateCancelReject(const CancelRequest &cancelRequest) {
	Response res(cancelRequest.getClOrdID(), ResponseType::CANCEL_REJECT, 0);
	res.setTextReason("Unknown ClOrdID");
	lastMatchingResults.push_back(res);
}

void MatchingEngine::sendFillMessages(NewOrderRequest provider, int pSeqNo, NewOrderRequest taker, int tSeqNo, int lastQty)
{

	// Logic to calculate lastPrice..
	int lastPrice;
	if (provider.getOrdPrice() > 0) {
		lastPrice = provider.getOrdPrice();
	}
	else if (provider.getOrdPrice() == 0 && provider.getSide() == 'S') {
		lastPrice = this->getSymbol().getLow();
	}
	else if (provider.getOrdPrice() == 0 && provider.getSide() == 'B') {
		lastPrice = this->getSymbol().getHigh();
	}

	// Check fully filled or partial fill
	ResponseType pResponse = provider.getOrdQty() == lastQty ? ResponseType::FILL : ResponseType::PARTIAL_FILL;
	ResponseType tResponse = taker.getOrdQty() == lastQty ? ResponseType::FILL : ResponseType::PARTIAL_FILL;

	
	// Create fills and add then to list of open responses ( lastMatchingResults );
	// Also update LiquidityProvider and other relevant fill fields

	Response fill1(provider.getClOrdID(), pResponse, pSeqNo, lastPrice, lastQty, this->getLastMkt(), true);
	fill1.setTextReason("");
	lastMatchingResults.push_back(fill1);
	Response fill2(taker.getClOrdID(), tResponse, tSeqNo, lastPrice, lastQty, this->getLastMkt(), false);
	fill1.setTextReason("");
	lastMatchingResults.push_back(fill2);

}

std::shared_ptr<OrderBookEntry> MatchingEngine::getOrderEntryKey(NewOrderRequest req) {
	auto sp1 = std::make_shared<OrderBookEntry>(req.getClOrdID(), req.getOrdPrice(), req.getSide());
	return sp1;
}

