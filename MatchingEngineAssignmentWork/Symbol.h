#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>

class Symbol {

public:
	Symbol() = delete;

	Symbol(std::string ricCode, int lotSize = 100, int dayHigh = 10000,int dayLow = 9000) :
		m_RicCode(ricCode), m_LotSize(lotSize), m_DayHigh(dayHigh), m_DayLow(dayLow) {
	}

	int getHigh() const{
		return m_DayHigh;
	}

	int getLow() const{
		return m_DayLow;
	}

	std::string getRicCode() const{
		return m_RicCode;
	}

	int getLotSize() const {
		return m_LotSize;
	}

	friend std::ostream& operator<<(std::ostream& os, const Symbol& sym);

private:
	const std::string m_RicCode;
	const int m_LotSize;
	const int m_DayHigh;
	const int m_DayLow;
};
#endif