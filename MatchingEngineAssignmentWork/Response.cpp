#include "Response.h"

std::ostream& operator<<(std::ostream& os, const Response& res) {

	std::string descText;

	switch (res.m_ResponseType) {
	case NEW_ACK:
		descText = "NEW_ACK";
		break;
	case NEW_REJECT:
		descText = "NEW_REJECT";
		break;
	case CANCEL_ACK:
		descText = "CANCEL_ACK";
		break;
	case CANCEL_REJECT:
		descText = "CANCEL_REJECT";
		break;
	case PARTIAL_FILL:
		descText = "PARTIAL_FILL";
		break;
	case FILL:
		descText = "FILL";
		break;
	default:
		descText = "UNKNOWN RESPONSE";
	}

	os << 
		res.m_ClOrdID				<< ":" << 
		descText					<< ":" << 
		//res.m_ExchangeExecutionID	<< ":" << // Specifically removed from output for testing
		res.m_OrdPrice				<< ":" <<
		res.m_OrdQty				<< ":" <<
		res.m_OrdType				<< ":" <<

		res.m_LastPrice				<< ":" <<
		res.m_LastQty				<< ":" <<
		res.m_LastMkt				<< ":" <<
		res.m_LiquidityProvider		<< ":" <<

		res.m_TextReason;

	
	
	return os;
}
