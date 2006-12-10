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

#include <math.h>
#include <tankgraph/TankMesh.h>
#include <GLEXT/GLState.h>
#include <graph/ModelRenderer.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>

TankMesh::TankMesh(Model &tank) : 
	ModelRenderer(&tank),
	scale_(1.0f)
{
	setupTankMesh();
}

TankMesh::~TankMesh()
{
}

int TankMesh::getNoTris()
{
	return (model_?model_->getNumberTriangles():0);
}

void TankMesh::setupTankMesh()
{
	// Make sure the tank is not too large
	const float maxSize = 3.0f;
	Vector &min = model_->getMin();
	Vector &max = model_->getMax();
	float size = (max - min).Magnitude();
	if (size > maxSize) scale_ = 2.2f / size;

	// Find the turrets and guns
	Mesh *turretPivot = 0, *gunPivot = 0;
	int turretCount = 0;

	std::vector<Mesh *>::iterator itor;
	for (itor = model_->getMeshes().begin();
		itor != model_->getMeshes().end();
		itor++)
	{
		Mesh *mesh = (*itor);
		const char *name = mesh->getName();

		if (strstr(name, "\"Turret") == name ||
			strstr(name, "\"turret") == name)
		{
			if (strstr(name, "pivot") ||
				strstr(name, "Pivot"))
			{
				turretPivot = mesh;
			}
			else
			{
				// Find the center that the tank should rotate around
				turretCount++;
				turretCenter_ += (mesh->getMax() + mesh->getMin()) / 2.0f;
			}

			meshTypes_.push_back(eTurret);
		}
		else if (strstr(name, "\"Gun") == name ||
			strstr(name, "\"gun") == name)
		{
			if (strstr(name, "pivot") ||
				strstr(name, "Pivot"))
			{
				gunPivot = mesh;
			}

			meshTypes_.push_back(eGun);
		}
		else
		{
			meshTypes_.push_back(eNone);
		}
	}

	// Find the center of rotation for the turret
	if (turretPivot)
	{
		turretCenter_ = (turretPivot->getMax() + turretPivot->getMin()) / 2.0f;
	}
	else
	{
		turretCenter_ /= float(turretCount);
	}
	Vector gunCenter = turretCenter_;
	turretCenter_[2] = 0.0f;

	// Find the center of rotation for the gun
	if (gunPivot)
	{
		gunCenter = (gunPivot->getMax() + gunPivot->getMin()) / 2.0f;
	}
	gunOffset_ = gunCenter - turretCenter_;
}

void TankMesh::draw(float frame, bool drawS, float angle, Vector &position, 
					float fireOffset, float rotXY, float rotXZ,
					bool absCenter, float scale)
{
	rotXY_ = rotXY;
	rotXZ_ = rotXZ;
	drawS_ = drawS;
	fireOffSet_ = fireOffset;

	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		glScalef(scale * scale_, scale * scale_, scale * scale_);

		if (absCenter) ModelRenderer::draw(frame);
		else ModelRenderer::drawBottomAligned(frame);
	glPopMatrix();
}

void TankMesh::drawMesh(unsigned int m, Mesh *mesh, float currentFrame)
{
	glPushMatrix();
		MeshType type = meshTypes_[m];
		vertexTranslation_ = -turretCenter_;

		if (type == eTurret || type == eGun)
		{
			glRotatef(rotXY_, 0.0f, 0.0f, 1.0f);
			if (type == eGun)
			{
				glTranslatef(gunOffset_[0], gunOffset_[1], gunOffset_[2]);
				vertexTranslation_ -= gunOffset_;
				glRotatef(rotXZ_, 1.0f, 0.0f, 0.0f);

				// Draw the sight
				if (drawS_ &&
					OptionsDisplay::instance()->getDrawPlayerSight() &&
					OptionsDisplay::instance()->getOldSightPosition())
				{
					GLState sightState(GLState::BLEND_OFF | GLState::TEXTURE_OFF);
					glPushMatrix();
						glScalef(1.0f / scale_, 1.0f / scale_, 1.0f / scale_);
						drawSight();
					glPopMatrix();
				}

				if (fireOffSet_ != 0.0f) glTranslatef(0.0f, fireOffSet_, 0.0f);
			}
		}

		ModelRenderer::drawMesh(m, mesh, currentFrame);
	glPopMatrix();

	vertexTranslation_.zero();
}

void TankMesh::drawSight()
{
	static GLuint sightList_ = 0;
	if (!sightList_)
	{
		glNewList(sightList_ = glGenLists(1), GL_COMPILE);
			glBegin(GL_QUAD_STRIP);
				float x;
				for (x=135.0f; x>=90.0f; x-=9.0f)
				{
					const float deg = 3.14f / 180.0f;
					float dx = x * deg;
					float color = 1.0f - fabsf(90.0f - x) / 45.0f;

					glColor3f(1.0f * color, 0.5f * color, 0.5f * color);
					glVertex3f(+0.03f * color, 2.0f * sinf(dx), 2.0f * cosf(dx));
					glVertex3f(+0.03f * color, 10.0f * sinf(dx), 10.0f * cosf(dx));
				}
				for (x=90.0f; x<135.0f; x+=9.0f)
				{
					const float deg = 3.14f / 180.0f;
					float dx = x * deg;
					float color = 1.0f - fabsf(90.0f - x) / 45.0f;

					glColor3f(1.0f * color, 0.5f * color, 0.5f * color);
					glVertex3f(-0.03f * color, 2.0f * sinf(dx), 2.0f * cosf(dx));
					glVertex3f(-0.03f * color, 10.0f * sinf(dx), 10.0f * cosf(dx));
				}
			glEnd();
		glEndList();
	}
	glCallList(sightList_);
}
