#include "KcVlCoordSystem.h"
#include "KgAxis.h"
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include "KglPaint.h"


KcVlCoordSystem::KcVlCoordSystem()
	: KvCoordSystem(vec3(0, 0, 0), vec3(10, 10, 10))
{

}


void KcVlCoordSystem::draw(KglPaint* paint) const
{
	if (visible()) {
		for (int i = 0; i < 12; i++) {
			auto ax = axis(i);
			if (ax && ax->visible()) {
				auto clr = ax->baselineColor();
				paint->setColor(vl::fvec4(clr.x, clr.y, clr.z, clr.w));
				//paint->setLineWidth(ax->baselineSize());
				paint->setLineWidth(0.5); // TODO: dock后，线的宽度会改变
				paint->drawLine(ax->start().x, ax->start().y, ax->start().z,
					ax->end().x, ax->end().y, ax->end().z);
			}
		}
	}
}
