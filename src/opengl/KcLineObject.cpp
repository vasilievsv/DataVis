#include "KcLineObject.h"
#include "glad.h"
#include "KsShaderManager.h"
#include "opengl/KcGlslProgram.h"


KcLineObject::KcLineObject(KeType type)
    : super_(type, KsShaderManager::singleton().programMono())
{

}


void KcLineObject::draw() const
{
    glLineWidth(lineWidth_);
    prog_->useProgram();
    glUniform4f(1, lineColor_[0], lineColor_[1], lineColor_[2], lineColor_[3]);
    super_::draw();
}

