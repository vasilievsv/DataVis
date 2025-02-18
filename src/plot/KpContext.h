#pragma once
#include <string>
#include <ostream>
#include <iomanip>
#include "KtColor.h"


// 一些绘制元素的上下文属性


class KpMarker
{
public:

	enum KeType
	{
		k_dot,       // a point marker
		// TODO: k_ball,      // a 3d-ball marker (no outline)
		k_circle,    // a circle marker
		k_square,    // a square maker
		k_diamond,   // a diamond marker
		k_up,        // an upward-pointing triangle marker
		k_down,      // an downward-pointing triangle marker
		k_left,      // an leftward-pointing triangle marker
		k_right,     // an rightward-pointing triangle marker
		k_cross,     // a cross marker (not fillable)
		k_plus,      // a plus marker (not fillable)
		k_asterisk,  // a asterisk marker (not fillable)
		k_count
	};

	int type{ k_dot };
	float size{ 5 }; // the size of marker
	color4f fill{ 0, 0, 0, 1 };
	color4f outline{ 0, 0, 0, 1 }; // 仅当hasOutline为真时，该成员才有意义
	float weight{ 1 }; // the width of outline
	bool showFill{ true };
	bool showOutline{ false };
	
	bool hasOutline() const {
		return type >= k_circle && type <= k_right;
	}

	bool operator==(const KpMarker& rhs) const {
		return type == rhs.type && size == rhs.size &&
			fill == rhs.fill && outline == rhs.outline && weight == rhs.weight &&
			showFill == rhs.showFill && showOutline == rhs.showOutline;
	}

	bool operator!=(const KpMarker& rhs) const {
		return !(*this == rhs);
	}

	bool visible() const {
		return showFill || (hasOutline() && showOutline);
	}
};


class KpPen
{
public:

	enum KeStyle
	{
		k_none,
		k_solid,
		k_dot,
		k_dash,
		k_dash4,
		k_dash8,
		k_dash_dot,
		k_dash_dot_dot
	};

	int style{ k_solid };
	float width{ 1 };
	color4f color{ 0, 0, 0, 1 };

	static const KpPen& null() {
		static KpPen nullPen{ k_none, 0, color4f(0) };
		return nullPen;
	}

	bool visible() const {
		return style != k_none && color.a() > 0 && width > 0;
	}
};


class KpBrush
{
public:

	enum KeStyle
	{
		k_none,
		k_solid
	};

	int style{ k_solid };
	color4f color{ 1, 1, 1, 1 };

	static const KpBrush& null() {
		static KpBrush nullBrush{ k_none, color4f(0) };
		return nullBrush;
	}

	bool visible() const {
		return style != k_none && color.a() > 0;
	}
};


class KpFont
{
public:
	std::string family;
	double size{ 13 };
	bool bold{ false };
	bool italic{ false };
	bool underline{ false };

	bool operator==(const KpFont& rhs) const {
		return family == rhs.family && size == rhs.size &&
			bold == rhs.bold && italic == rhs.italic && underline == rhs.underline;
	}

	bool operator!=(const KpFont& rhs) const {
		return !operator==(rhs);
	}
};


class KpLabel
{
public:
	KpFont font;
	color4f color{ 0, 0, 0, 1 };
	int align{ 0 };
	point2f spacing{ 0, 0 };
};


class KpNumericFormatter
{
public:

	enum
	{
		k_fixed,        // 语义同std::fixed
		k_scientific,   // 语义同std::scientific
		k_hexfloat,     // 语义同std::hexfloat
		k_defaultfloat  // 语义同std::defaultfloat
	};

	int format{ k_defaultfloat };
	int precision{ 6 }; // 语义同std::setprecision

	template<class CharT>
	void apply(std::basic_ostream<CharT>& strm) const {
		strm << std::setprecision(precision);
		switch (format) {
		case 0:  strm << std::fixed; break;
		case 1:  strm << std::scientific; break;
		case 2:  strm << std::hexfloat; break;
		default: strm << std::defaultfloat; break;
		}
	}
};