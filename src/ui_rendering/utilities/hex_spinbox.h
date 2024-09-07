#ifndef HEX_SPINBOX_H
#define HEX_SPINBOX_H

#include "main.h"

class HexSpinBox : public QSpinBox
{
public:
	HexSpinBox(bool only16Bits, QWidget *parent = 0);
	unsigned int hexValue() const;
	void setHexValue(unsigned int value);

	void setNumberOfDigits(unsigned int number);
protected:
	QString textFromValue(int value) const;
	int valueFromText(const QString &text) const;
	QValidator::State validate(QString &input, int &pos) const;

private:
	int number_of_digits = 2;
	bool m_only16Bits;
	inline unsigned int u(int i) const;
	inline int i(unsigned int u) const;

};

#endif // HEX_SPINBOX_H
