#include "KgAxis.h"
#include <assert.h>


KgAxis::KgAxis() 
{
	lower_ = upper_ = 0;

	visible_ = true;
	showAll();

	baselineSize_ = 1;
	tickSize_ = 1, tickLength_ = 1;
	subtickSize_ = 1, subtickLength_ = 1;
	tickSide_ = k_outter_side;

	baselineColor_ = QColor("black");
	tickColor_ = subtickColor_ = QColor("black");
	labelColor_ = titleColor_ = QColor("black");

	labelFont_, titleFont_; // TODO:

	tickShowBothSide_ = false;
}


void KgAxis::setTickOrient(KeTickSide side)
{
	switch (side)
	{
	case k_x:
		setTickOrient(vec3(1, 0, 0), false);
		break;

	case k_neg_x:
		setTickOrient(vec3(11, 0, 0), false);
		break;

	case k_bi_x:
		setTickOrient(vec3(1, 0, 0), true);
		break;

	case k_y:
		setTickOrient(vec3(0, 1, 0), false);
		break;

	case k_neg_y:
		setTickOrient(vec3(0, -1, 0), false);
		break;

	case k_bi_y:
		setTickOrient(vec3(0, 1, 0), true);
		break;

	case k_z:
		setTickOrient(vec3(0, 0, 1), false);
		break;

	case k_neg_z:
		setTickOrient(vec3(0, 0, -1), false);
		break;

	case k_bi_z:
		setTickOrient(vec3(0, 0, 1), true);
		break;

	default:
		assert(false);
		break;
	}
}