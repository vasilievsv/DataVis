#include "KvCoord.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"


void KvCoord::draw(KvPaint* paint) const
{
	if (visible()) {

		forPlane([paint](KcCoordPlane& plane) {
			if (plane.visible())
				plane.draw(paint);
			return true;
			});

		forAxis([paint](KcAxis& axis) {
			if (axis.visible() && axis.length() > 0)
				axis.draw(paint);
			return true;
			});
	}
}


KvCoord::aabb_t KvCoord::boundingBox() const
{
	aabb_t box;

	forAxis([&box](KcAxis& axis) {
		box.merge(axis.boundingBox());
		return true;
		});

	return box;
}


void KvCoord::zoom(float_t factor)
{
	auto c = center();
	auto delta = (upper() - lower()) * factor * 0.5;
	setExtents(c - delta, c + delta);

	assert(c.isApproxEqual(center()));
}
