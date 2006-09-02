////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <GLEXT/GLTexture.h>
#include <GLEXT/GLStateExtension.h>
#include <common/Defines.h>
#include <set>

static std::set<GLuint> usedNumbers_;
unsigned int GLTexture::textureSpace_ = 0;

GLTexture::GLTexture() : 
	texNum_(0), texType_(GL_TEXTURE_2D), 
	usedSpace_(0),
	width_(0), height_(0)
{

}

GLTexture::~GLTexture()
{
	if (texNum_)
	{
		glDeleteTextures(1, &texNum_);
		usedNumbers_.erase(texNum_);
		texNum_ = 0;
	}
}

void GLTexture::draw(bool force)
{
	if ((this != lastBind_) || force)
	{
		glBindTexture(texType_, texNum_);
		lastBind_ = this;
	}
}

bool GLTexture::replace(GLImage &bitmap,
						GLenum format, 
						bool mipMap)
{
	if (textureValid())
	{ 
		if(GLStateExtension::getNoTexSubImage() ||
			(bitmap.getWidth() != width_) ||
			(bitmap.getHeight() != height_))
		{
			glDeleteTextures(1, &texNum_);
			usedNumbers_.erase(texNum_);
			texNum_ = 0;
		}
	}

	if (textureValid())
	{
		glBindTexture(texType_, texNum_);
		if (glGetError() != GL_INVALID_VALUE &&
			glGetError() != GL_INVALID_OPERATION)
		{
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmap.getWidth());
			glTexSubImage2D(texType_, 0, 
				0, 0, 
				bitmap.getWidth(), bitmap.getHeight(), 
				format, GL_UNSIGNED_BYTE, 
				bitmap.getBits());
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		}
	}
	else
	{
		return create(bitmap, format, mipMap);
	}
	return true;
}

bool GLTexture::create(GLImage &bitmap, 
					   GLenum format, 
					   bool mipMap)
{
	bool success = create(bitmap.getBits(),
			bitmap.getWidth(), 
			bitmap.getHeight(), 
			bitmap.getComponents(), 
			bitmap.getAlignment(),
			format,
			mipMap);

	return success;
}

bool GLTexture::createObject()
{
	if (!textureValid())
	{
		GLfloat priority = 1.0f;
		glGenTextures(1, &texNum_);

		if (usedNumbers_.find(texNum_) != usedNumbers_.end())
		{
			DIALOG_ASSERT("Texture Reuse" == 0);
		}
		usedNumbers_.insert(texNum_);

		if (glGetError() == GL_INVALID_VALUE ||
			glGetError() == GL_INVALID_OPERATION)
		{
			//DIALOG_ASSERT("Failed to create texture" == 0);
			return false;
		}
		glPrioritizeTextures(1, &texNum_, &priority);
	}

	return true;
}

bool GLTexture::create(const void * data, 
					   GLint width, 
					   GLint height, 
					   GLint components, 
					   GLint alignment,
					   GLenum format, 
					   bool mipMap)
{
	bool success = false;
	if (data)
	{
		if (height == 1 || width == 1) texType_ = GL_TEXTURE_1D;
		else texType_ = GL_TEXTURE_2D;

		if (!validateSize(width) ||	!validateSize(height))
		{
			dialogExit("Scorched3D",
				formatString("Invalid OpenGL texture size %ix%i specified",
				width, height));
		}

		createObject();
		glBindTexture(texType_, texNum_);
		if (glGetError() == GL_INVALID_VALUE ||
			glGetError() == GL_INVALID_OPERATION)
		{
			//DIALOG_ASSERT("Failed to bind create texture" == 0);
		}

		success = createTexture(data, width, height, components, alignment, format, mipMap);
	}

	return success;
}

bool GLTexture::createTexture(const void * data, 
							  GLint width, 
							  GLint height, 
							  GLint components, 
							  GLint alignment,
							  GLenum format, 
							  bool mipMap)
{
	textureSpace_ -= usedSpace_;
	usedSpace_ = width * height * components;
	textureSpace_ += usedSpace_;
	width_ = width;
	height_ = height;

	texFormat_ = format;
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	if (height == 1)
	{
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		gluBuild1DMipmaps(GL_TEXTURE_1D, components, width, 
				format, GL_UNSIGNED_BYTE, data);
	}
	else if (width == 1)
	{
		if (mipMap)
		{
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			gluBuild1DMipmaps(GL_TEXTURE_1D, components, height, 
					format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage1D(GL_TEXTURE_1D, 0, components, height, 
				0, format, GL_UNSIGNED_BYTE, data);
		}
	}
	else
	{
		if (mipMap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

			if (GLStateExtension::hasHardwareMipmaps()) // Use hardware mipmaps
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

				glTexImage2D(GL_TEXTURE_2D, 0, components, width, 
					height, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				gluBuild2DMipmaps(GL_TEXTURE_2D, components, width, 
					height, format, GL_UNSIGNED_BYTE, data);
			}
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

			glTexImage2D(GL_TEXTURE_2D, 0, components, width, 
				height, 0, format, GL_UNSIGNED_BYTE, data);
		}
	};

	return true;
}

bool GLTexture::createBufferTexture(GLint width, GLint height, bool depthTex)
{
	// Init internals
	texType_ = GL_TEXTURE_2D;
	width_ = width;
	height_ = height;

	// Create texture
	if (!createObject()) return false;

	glBindTexture(texType_, texNum_);
	if (glGetError() == GL_INVALID_VALUE ||
		glGetError() == GL_INVALID_OPERATION)
	{
		return false;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (!depthTex)
	{
		texFormat_ = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE,NULL);
	}
	else
	{
		texFormat_ = GL_DEPTH_COMPONENT;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

		// Find current depth buffer and use the same depth
		GLint depth_bits;
		glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
		GLenum depth_format;
		if(depth_bits == 16)  depth_format = GL_DEPTH_COMPONENT16_ARB;
		else                  depth_format = GL_DEPTH_COMPONENT24_ARB;

		glTexImage2D(GL_TEXTURE_2D, 0, depth_format, width, height, 0, 
			GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);		
	}

	return true;
}

bool GLTexture::textureValid()
{
	return (glIsTexture(texNum_) == GL_TRUE);
}

bool GLTexture::validateSize(int size)
{
	return (!(size != 1 &&
		size != 2 &&
		size != 4 &&
		size != 8 && 
		size != 16 &&
		size != 32 &&
		size != 64 &&
		size != 128 &&
		size != 256 &&
		size != 512 &&
		size != 1024 &&
		size != 2048));
}
