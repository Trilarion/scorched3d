////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <GLEXT/GLDynamicVertexArray.h>
#include <GLEXT/GLInfo.h>

GLDynamicVertexArray *GLDynamicVertexArray::instance_ = 0;

GLDynamicVertexArray *GLDynamicVertexArray::instance()
{
	if (!instance_) 
	{
		instance_ = new GLDynamicVertexArray();
	}
	return instance_;
}

GLDynamicVertexArray::GLDynamicVertexArray() : 
	capacity_(3000), used_(0), vbo_(0), array_(0)
{
	if (GLStateExtension::glGenBuffersARB())
	{
		GLStateExtension::glGenBuffersARB()(1, &vbo_);
		GLStateExtension::glBindBufferARB()(GL_ARRAY_BUFFER_ARB, vbo_);
		GLStateExtension::glBufferDataARB()
			(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat) * capacity_, 
			0, GL_DYNAMIC_DRAW_ARB);
		if (glGetError() == GL_OUT_OF_MEMORY) vbo_ = 0;
	}

	if (!vbo_)
	{
		array_ = new GLfloat[capacity_];
	}
}

GLDynamicVertexArray::~GLDynamicVertexArray()
{
	if (vbo_)
	{
		GLStateExtension::glDeleteBuffersARB()(1, &vbo_);
	}
	else
	{
		delete [] array_;
	}
}

void GLDynamicVertexArray::drawROAM()
{
	if (vbo_)
	{
		array_ = 0;
		GLStateExtension::glBindBufferARB()(GL_ARRAY_BUFFER_ARB, vbo_);
		GLStateExtension::glUnmapBufferARB()(GL_ARRAY_BUFFER_ARB);
	}

	int stride = 5;
	if (GLStateExtension::glClientActiveTextureARB())
	{
		if (GLStateExtension::getTextureUnits() > 2)
		{
			stride = 7;
		}
	}
	if (used_ < stride * 3)
	{
		used_ = 0;
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	if (GLStateExtension::glClientActiveTextureARB())
	{
		GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE2_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	GLStateExtension::glBindBufferARB()(GL_ARRAY_BUFFER_ARB, vbo_);
	glVertexPointer(3, GL_FLOAT, stride * sizeof(GL_FLOAT), array_);
	glTexCoordPointer(2, GL_FLOAT, stride * sizeof(GL_FLOAT), array_ + 3);
	if (GLStateExtension::glClientActiveTextureARB())
	{
		GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE1_ARB);
		glTexCoordPointer(2, GL_FLOAT, stride * sizeof(GL_FLOAT), array_ + 3);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE2_ARB);
			glTexCoordPointer(2, GL_FLOAT, stride * sizeof(GL_FLOAT), array_ + 5);
		}
	}

	glDrawArrays(GL_TRIANGLES, 0, used_ / stride);
	GLInfo::addNoTriangles((used_ / stride) / 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (GLStateExtension::glClientActiveTextureARB())
	{
		GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE2_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	GLStateExtension::glClientActiveTextureARB()(GL_TEXTURE0_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	used_ = 0;
}

void GLDynamicVertexArray::drawQuadStrip(bool useColor)
{
	if (vbo_)
	{
		array_ = 0;
		GLStateExtension::glBindBufferARB()(GL_ARRAY_BUFFER_ARB, vbo_);
		GLStateExtension::glUnmapBufferARB()(GL_ARRAY_BUFFER_ARB);
	}

	int stride = 5;
	if (useColor) stride = 8;
	if (used_ < 4 * stride)
	{
		used_ = 0;
		return;	
	}

	// TODO *** Half of this information is actually static and could be held
	// in non-dynamic memory.

	// Define	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (useColor) glEnableClientState(GL_COLOR_ARRAY);
	GLStateExtension::glBindBufferARB()(GL_ARRAY_BUFFER_ARB, vbo_);
	glVertexPointer(3, GL_FLOAT, stride * sizeof(GL_FLOAT), array_);
	glTexCoordPointer(2, GL_FLOAT, stride * sizeof(GL_FLOAT), array_ + 3);
	if (useColor) glColorPointer(3, GL_FLOAT, stride * sizeof(GL_FLOAT), array_ + 5);

	// Draw
	glDrawArrays(GL_QUAD_STRIP, 0, used_ / stride);
	GLInfo::addNoTriangles((used_ / stride) - 2);

	// Undefine
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (useColor) glDisableClientState(GL_COLOR_ARRAY);

	used_ = 0;
}
