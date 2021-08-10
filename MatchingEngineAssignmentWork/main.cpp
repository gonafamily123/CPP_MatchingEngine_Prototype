#include <iostream>
#include "Symbol.h"

#include "MatchingEngine.h"
#include "NewOrderRequest.h"
#include "CancelRequest.h"
#include <sstream>

using namespace std;

/*
** main program with basic tests to test the matching engine and the responses after matching
** This is very basic sanity checks to prove matching engine is working 
*/

bool Test_1a_SanityInitTests() {
	std::string ric = "6758.T";
	
	{
		NewOrderRequest req1("c1", "BANK1", "TOSTNET", 'D', 'B', ric, 0, 1000, 'M');
		ostringstream s1;
		s1 << req1;
		if (!(s1.str() == "c1:BANK1:TOSTNET:D:B:6758.T:0:1000:M"))
			return false;
	}
	
	{
		NewOrderRequest req2("c2", "BANK1", "TOSTNET", 'D', 'S', ric, 420, 1000, 'L');
		ostringstream s2;
		s2 << req2;
		if (!(s2.str() == "c2:BANK1:TOSTNET:D:S:6758.T:420:1000:L"))
			return false;
	}

	{
		CancelRequest cxl1("c1", 'B');
		ostringstream s3;
		s3 << cxl1;
		if (!(s3.str() == "c1.1:F:B:c1"))
			return false;
	}

	{
		CancelRequest cxl2("c2", 'S');
		ostringstream s4;
		s4 << cxl2;
		if (!(s4.str() == "c2.1:F:S:c2"))
			return false;
	}

	return true;

}


bool Test_1b_HandleNewOrder()
{
	Symbol g_Symbol("6758.T", 100, 12000, 10000);
	MatchingEngine g_MatchingEngine(g_Symbol, "TOSTNET");
	
	std::string ric = "6758.T";

	// New Order and Ack
	{    
		NewOrderRequest req1("c1", "BANK1", "TOSTNET", 'D', 'B', ric, 0, 1000, 'M');
		g_MatchingEngine.acceptNew(req1);

		ostringstream s1;
		g_MatchingEngine.showMatchingEngineState(s1);
		// Confirm 1 order in matchingengine.
		if (!(s1.str() == ":BUY BOOK:B:0:1:c1:1-->c1:BANK1:TOSTNET:D:B:6758.T:0:1000:M:\n"))
			return false;

		// Confirm 1 new order ack response
		auto results = g_MatchingEngine.getLastMatchingResults();
		ostringstream s2;
		for (const auto& responses : results) {
			s2 << responses ;
		}
		if (!(s2.str() == "c1:NEW_ACK:0:1000:M:0:0::0:"))
			return false;
	}
	
	return true;
}

bool Test_1c_HandleNewOrderCancel()
{
	Symbol g_Symbol("6758.T", 100, 12000, 10000);
	MatchingEngine g_MatchingEngine(g_Symbol, "TOSTNET");

	std::string ric = "6758.T";
	// 1. New Order , Ack , 
	// 2. Cancel , Cancel Ack , 
	// 3. Cancel again and get Cancel reject 2nd time (Since order is already cancelled above )

	{
		//1. New Order and clear results
		NewOrderRequest req1("c1", "BANK1", "TOSTNET", 'D', 'B', ric, 0, 1000, 'M');
		g_MatchingEngine.acceptNew(req1);
		g_MatchingEngine.clearLastMatchingResults();

		//2. Cancel the previous new order 
		CancelRequest cxlReq1("c1", 'B');  // Specify ClOrdID , Side for Cancel Request..
		g_MatchingEngine.acceptCancel(cxlReq1);
		
		ostringstream s1;
		g_MatchingEngine.showMatchingEngineState(s1);

		//2a. Confirm matchingengine is empty after cancellation.
		if (!(s1.str() == "")){
			cout << "Expected empty matching state after cancellation ." << endl;
			return false;
		}

		//2b. Check for outstanding responses / Check the cancel request responses
		auto results = g_MatchingEngine.getLastMatchingResults();
		ostringstream s2;
		for (const auto& responses : results) {
			s2 << responses;
		}

		
		if (!(s2.str() == "c1.1:CANCEL_ACK:0:0:L:0:0::0:")) {             
			cout << "Expected cancel ack in outstanding messages after cancellation" << endl;
			cout <<"[" << s2.str() << "]" <<  endl;
			return false;
		}

		g_MatchingEngine.clearLastMatchingResults();

		//3. Cancel second time - order already cancelled above  
		CancelRequest cxlReq2("c1", 'B');  // Specify ClOrdID , Side for Cancel Request..
		g_MatchingEngine.acceptCancel(cxlReq2);

		ostringstream s3;
		g_MatchingEngine.showMatchingEngineState(s3);

		//3a. Confirm matchingengine is empty after cancellation.
		if (!(s3.str() == "")) {
			cout << "Even second time cancellation is expected to have no impact on matching engine state" << endl;
			return false;
		}

		
		//3b. Check for outstanding responses / Check the cancel request responses
		auto results2 = g_MatchingEngine.getLastMatchingResults();
		ostringstream s4;
		
		for (const auto& responses : results2) {
			s4 << responses;
		}
		if (!(s4.str() == "c1.1:CANCEL_REJECT:0:0:L:0:0::0:Unknown ClOrdID")) {
			cout << "Expected CANCEL_REJECT since the order was already cancelled " << endl;
			return false;
		}

		//g_MatchingEngine.showMatchingEngineState(s3);

	}
	return true;
}


bool Test_1d_HandleSimpleNewMatchingCase()
{
	Symbol g_Symbol("6758.T", 100, 12000, 10000);
	MatchingEngine g_MatchingEngine(g_Symbol, "TOSTNET");

	std::string ric = "6758.T";

	// Scenario Both Mkt Order Matches 
	// 1. New Order , Ack , 
	// 2. Oppposite order , Ack
	// 3. Both orders match 
	// 4. Check fills Quality 

	{
		//1. New Order and clear results
		NewOrderRequest req1("c1", "BANK1", "TOSTNET", 'D', 'B', ric, 0, 1000, 'M');
		g_MatchingEngine.acceptNew(req1);
		
		//2. Send Opposite Order to match  
		NewOrderRequest req2("c2", "BANK1", "TOSTNET", 'D', 'S', ric, 0, 1000, 'M');
		g_MatchingEngine.acceptNew(req2);

		auto results = g_MatchingEngine.getLastMatchingResults();
		ostringstream s1;

		for (const auto& responses : results) {
			s1 << responses;
		}

		cout << results.size() << endl;
		if (results.size() != 4) {
			// Count is c1 ack , c2 ack , c1 fill , c2 fill ;
			return false;
		}

		
		if (!(s1.str() == 
			"c1:NEW_ACK:0:1000:M:0:0::0:" 
			"c2:NEW_ACK:0:1000:M:0:0::0:"
			"c1:FILL:0:0:L:12000:1000:TOSTNET:1:"
			"c2:FILL:0:0:L:12000:1000:TOSTNET:0:"))
			return false;

	}

	return true;
}

bool Test_1e_MatchBigAndSmall() {

	Symbol g_Symbol("6758.T", 100, 12000, 10000);
	MatchingEngine g_MatchingEngine(g_Symbol, "TOSTNET");

	std::string ric = "6758.T";
	
	NewOrderRequest req1("c1", "BANK1", "TOSTNET", 'D', 'B', ric, 0, 1000, 'M');
	//1 big Sell Order
	NewOrderRequest req2("c2", "BANK1", "TOSTNET", 'D', 'S', ric, 10390, 10000, 'L');

	g_MatchingEngine.acceptNew(req1);
	
	g_MatchingEngine.acceptNew(req2);

	// This should match all remaining qty of C2 
	NewOrderRequest req3("c3", "BANK1", "TOSTNET", 'D', 'B', ric, 0, 9000, 'M');
	g_MatchingEngine.acceptNew(req3);

	g_MatchingEngine.showMatchingEngineState(std::cout);

	auto results = g_MatchingEngine.getLastMatchingResults();

	if (results.size() != 7) {
		/*
		c1:NEW_ACK:0 : 1000 : M : 0 : 0::0 :
		c2 : NEW_ACK : 10390 : 10000 : L : 0 : 0::0 :
		c1 : FILL : 0 : 0 : L : 12000 : 1000 : TOSTNET : 1 :
		c2 : PARTIAL_FILL : 0 : 0 : L : 12000 : 1000 : TOSTNET : 0 :
		c3 : NEW_ACK : 0 : 9000 : M : 0 : 0::0 :
		c2 : FILL : 0 : 0 : L : 10390 : 9000 : TOSTNET : 1 :
		c3 : FILL : 0 : 0 : L : 10390 : 9000 : TOSTNET : 0 :
		*/
		cout << "Expected 7 messages as above but received " << results.size() << endl;
		return false;
	}
	

	ostringstream s3;
	g_MatchingEngine.showMatchingEngineState(s3);

	//3a. Confirm matchingengine is empty after cancellation.
	if (!(s3.str() == "")) {
		cout << "Expected no open orders on matching engine" << endl;
		return false;
	}

	return true;
}


bool Test_1f_MatchManyOrderCheckRemaining() {

	Symbol g_Symbol("6758.T", 100, 12000, 10000);
	MatchingEngine g_MatchingEngine(g_Symbol, "TOSTNET");

	std::string ric = "6758.T";
	//8 buy Orders

	NewOrderRequest req1("c1", "BANK1", "TOSTNET", 'D', 'B', ric, 0,     1000, 'M');
	NewOrderRequest req2("c2", "BANK1", "TOSTNET", 'D', 'B', ric, 0,     1000, 'M');
	NewOrderRequest req3("c3", "BANK1", "TOSTNET", 'D', 'B', ric, 10400, 1000, 'L');
	NewOrderRequest req4("c4", "BANK1", "TOSTNET", 'D', 'B', ric, 10410, 1000, 'L');
	NewOrderRequest req5("c5", "BANK1", "TOSTNET", 'D', 'B', ric, 10400, 1000, 'L');
	NewOrderRequest req6("c6", "BANK1", "TOSTNET", 'D', 'B', ric, 10420, 1000, 'L');
	NewOrderRequest req7("c7", "BANK1", "TOSTNET", 'D', 'B', ric, 0,     1000, 'M');
	NewOrderRequest req8("c8", "BANK1", "TOSTNET", 'D', 'B', ric, 10390, 1000, 'L');


	//1 big Sell Order
	NewOrderRequest req9("c9", "BANK1", "TOSTNET", 'D', 'S', ric, 10390, 10000, 'L');

	CancelRequest cxlReq9("c9", 'S');  // Specify ClOrdID , Side for Cancel Request..


	g_MatchingEngine.acceptNew(req1);
	g_MatchingEngine.acceptNew(req2);
	g_MatchingEngine.acceptNew(req3);
	g_MatchingEngine.acceptNew(req4);
	g_MatchingEngine.acceptNew(req5);
	g_MatchingEngine.acceptNew(req6);
	g_MatchingEngine.acceptNew(req7);
	g_MatchingEngine.acceptNew(req8);

	if (g_MatchingEngine.getLastMatchingResults().size() != 8) {
		cout << "Expected 8 order responses in the queue ";
		return false;
	}

	//g_MatchingEngine.showMatchingEngineState(std::cout);

	g_MatchingEngine.acceptNew(req9);

	g_MatchingEngine.showMatchingEngineState(std::cout);

	auto results = g_MatchingEngine.getLastMatchingResults();

	for (const auto& a : results) {
		cout << a << endl;
		
	}

	
	return true;
}


int main()
{
	
	if (!Test_1a_SanityInitTests()) {
		cout << "Test_1a_SanityInitTests failed" << endl;
	}
	else {
		cout << "Test_1a_SanityInitTests passed" << endl;
	}
	
	
	if (!Test_1b_HandleNewOrder()) {
		cout << "Test_1b_HandleNewOrder failed" << endl;
	}
	else {
		cout << "Test_1b_HandleNewOrder passed" << endl;
	}
	

	if (!Test_1c_HandleNewOrderCancel()) {
		cout << "Test_1c_HandleNewCancel failed" << endl;
	}
	else {
		cout << "Test_1c_HandleNewCancel passed" << endl;
	}
	
	
	if (!Test_1d_HandleSimpleNewMatchingCase()) {
		cout << "Test_1d_HandleSimpleNewMatchingCase failed" << endl;
	}
	else {
		cout << "Test_1d_HandleSimpleNewMatchingCase passed" << endl;
	}

	
	if (!Test_1e_MatchBigAndSmall()) {
		cout << "Test_1e_MatchBigAndSmall failed" << endl;
	}
	else {
		cout << "Test_1e_MatchBigAndSmall passed" << endl;
	}
	
	
	if (!Test_1f_MatchManyOrderCheckRemaining()) {
		cout << "Test_1f_MatchManyOrderCheckRemaining failed" << endl;
	}
	else {
		cout << "Test_1f_MatchManyOrderCheckRemaining passed" << endl;
	}
	
}


