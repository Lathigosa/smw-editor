#include "ui_rendering/utilities/hex_spinbox.h"

	HexSpinBox::HexSpinBox(bool only16Bits, QWidget *parent) : QSpinBox(parent), m_only16Bits(only16Bits)
	{
		setPrefix("0x");
		//setDisplayIntegerBase(16);
		if (only16Bits)
			setRange(0, 0xFFFF);
		else
			setRange(INT_MIN, INT_MAX);
	}
	unsigned int HexSpinBox::hexValue() const
	{
		return u(value());
	}
	void HexSpinBox::setHexValue(unsigned int value)
	{
		setValue(i(value));
	}

	void HexSpinBox::setNumberOfDigits(unsigned int number)
	{
		number_of_digits = number;
	}

	QString HexSpinBox::textFromValue(int value) const
	{
		//return QString::number(u(value), 16).toUpper();
		return QString("%1").arg(u(value), number_of_digits , 16, QChar('0')).toUpper();
	}
	int HexSpinBox::valueFromText(const QString &text) const
	{
		return i(text.toUInt(0, 16));
	}
	QValidator::State HexSpinBox::validate(QString &input, int &pos) const
	{
		QString copy(input);
		if (copy.startsWith("0x"))
			copy.remove(0, 2);
		pos -= copy.size() - copy.trimmed().size();
		copy = copy.trimmed();
		if (copy.isEmpty())
			return QValidator::Intermediate;
		input = QString("0x") + copy.toUpper();
		bool okay;
		unsigned int val = copy.toUInt(&okay, 16);
		if (!okay || (m_only16Bits && val > 0xFFFF))
			return QValidator::Invalid;
		return QValidator::Acceptable;
	}

	inline unsigned int HexSpinBox::u(int i) const
	{
		return *reinterpret_cast<unsigned int *>(&i);
	}

	inline int HexSpinBox::i(unsigned int u) const
	{
		return *reinterpret_cast<int *>(&u);
	}
