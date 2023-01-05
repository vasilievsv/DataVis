#include "KcLineObject.h"
#include "glad.h"
#include "KsShaderManager.h"


KcLineObject::KcLineObject(KePrimitiveType type)
    : super_(type)
{
    prog_ = KsShaderManager::singleton().programFlat();
}


void KcLineObject::draw() const
{
    glLineWidth(lineWidth_);
    super_::draw();
}

