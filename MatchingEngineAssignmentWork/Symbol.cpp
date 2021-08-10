#include "Symbol.h"

std::ostream& operator<<(std::ostream& os, const Symbol& s) {
	return os << s.getRicCode() << ":" << s.getLotSize() << ":" << s.getHigh() << ":" << s.getLow();

}