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


// GLCameraFrustum.h: interface for the GLCameraFrustum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCAMERAFRUSTUM_H__4C341296_0589_46D5_BFD7_3114534F7E2D__INCLUDED_)
#define AFX_GLCAMERAFRUSTUM_H__4C341296_0589_46D5_BFD7_3114534F7E2D__INCLUDED_


#include <engine/GameStateI.h>
#include <common/Vector.h>

class GLCameraFrustum : public GameStateI
{
public:
	static GLCameraFrustum *instance();

	virtual void draw(const unsigned state);

	bool pointInFrustum(Vector &point);
	bool sphereInFrustum(Vector &point, float fRadius);

	Vector &getBilboardVectorX();
	Vector &getBilboardVectorY();

	float *getViewMatrix() { return fView; }

protected:
	static GLCameraFrustum *instance_;
	float frustum_[6][4];
	float fProj[16];
	float fView[16];
	float fClip[16];

	void normalize(float vector[4]);

private:
	GLCameraFrustum();
	virtual ~GLCameraFrustum();

};

#endif // !defined(AFX_GLCAMERAFRUSTUM_H__4C341296_0589_46D5_BFD7_3114534F7E2D__INCLUDED_)
