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

#include <graph/ModelRenderer.h>
#include <graph/OptionsDisplay.h>
#include <graph/TextureStore.h>
#include <3dsparse/ModelMaths.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLInfo.h>

ModelRenderer::ModelRenderer(Model *model) : 
	model_(model)
{
	setup();
}

ModelRenderer::~ModelRenderer()
{
	while (!boneTypes_.empty())
	{
		BoneType *type = boneTypes_.back();
		boneTypes_.pop_back();
		delete type;
	}
	while (!meshInfos_.empty())
	{
		MeshInfo info = meshInfos_.back();
		meshInfos_.pop_back();

		while (!info.frameInfos_.empty())
		{
			MeshFrameInfo frameInfo = info.frameInfos_.back();
			info.frameInfos_.pop_back();
			if (frameInfo.displayList != 0)
			{
				glDeleteLists(frameInfo.displayList, 1);
			}
		}
	}
}

void ModelRenderer::setup()
{
	std::vector<BoneType *> &baseTypes = model_->getBaseBoneTypes();
	std::vector<BoneType *>::iterator itor;
	for (itor = baseTypes.begin();
		itor != baseTypes.end();
		itor++)
	{
		boneTypes_.push_back(new BoneType(*(*itor)));
	}

	MeshInfo info;
	MeshFrameInfo frameInfo;
	for (int f=0; f<=model_->getTotalFrames(); f++)
	{
		info.frameInfos_.push_back(frameInfo);
	}
	
	for (unsigned int m=0; m<model_->getMeshes().size(); m++)
	{
		meshInfos_.push_back(info);
	}
}

void ModelRenderer::drawBottomAligned(float currentFrame, float fade)
{
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -model_->getMin()[2]);
		draw(currentFrame, fade);
	glPopMatrix();
}

void ModelRenderer::draw(float currentFrame, float fade)
{
	// Set transparency on
	GLState glstate(GLState::BLEND_ON | GLState::ALPHATEST_ON);

	// Fade the model (make it transparent)
	bool useBlendColor = (GLStateExtension::hasBlendColor() && fade < 1.0f);
	if (useBlendColor)
	{
		fade = MIN(1.0f, MAX(fade, 0.2f));
		GLStateExtension::glBlendColorEXT()(0.0f, 0.0f, 0.0f, fade);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA_EXT);
	}

	// Draw the model
	for (unsigned int m=0; m<model_->getMeshes().size(); m++)
	{
		Mesh *mesh = model_->getMeshes()[m];
		drawMesh(m, mesh, currentFrame);
	}

	// Turn off fading
	if (useBlendColor)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void ModelRenderer::drawMesh(unsigned int m, Mesh *mesh, float currentFrame)
{
	MeshInfo &meshInfo = meshInfos_[m];

	bool useTextures =
		(!OptionsDisplay::instance()->getNoSkins() &&
		mesh->getTextureName()[0]);
	unsigned state = GLState::TEXTURE_OFF;
	if (useTextures)
	{
		state = GLState::TEXTURE_ON;
		if (!meshInfo.texture)
		{
			meshInfo.texture = 
				TextureStore::instance()->loadTexture(
 					mesh->getTextureName(), mesh->getATextureName());
		}
		if (meshInfo.texture) meshInfo.texture->draw();
		
		if (mesh->getSphereMap())
		{
			state |= GLState::NORMALIZE_ON;

			glEnable(GL_TEXTURE_GEN_S);						
			glEnable(GL_TEXTURE_GEN_T);	
			glEnable(GL_TEXTURE_GEN_R);
			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
	}
	bool vertexLighting = OptionsDisplay::instance()->getNoModelLighting();
	if (!vertexLighting)
	{
		state |= 
			GLState::NORMALIZE_ON | 
			GLState::LIGHTING_ON | 
			GLState::LIGHT1_ON;

		if (useTextures)
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh->getAmbientColor());
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->getDiffuseColor());
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh->getSpecularColor());
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh->getEmissiveColor());
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh->getShininessColor());
		}
		else
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh->getAmbientNoTexColor());
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->getDiffuseNoTexColor());
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh->getSpecularNoTexColor());
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh->getEmissiveNoTexColor());
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh->getShininessColor());
		}
	}

	// Get the current frame for the animation
	// If we have no bones, when we only have one frame
	int frame = model_->getStartFrame();
	if (mesh->getReferencesBones())
	{
		// If we have bones, make sure the frame falls within the accepted bounds
		if (model_->getTotalFrames() > 1)
		{
			frame = int(currentFrame) % model_->getTotalFrames();
			if (frame < 0) frame = 0;
		}
	}

	GLState glState(state);
	{
		int frameNo = frame;
		DIALOG_ASSERT(frameNo >= 0 && frameNo < (int) meshInfo.frameInfos_.size());
		
		unsigned int lastState = meshInfo.frameInfos_[frameNo].lastCachedState;
		unsigned int displayList = meshInfo.frameInfos_[frameNo].displayList;
		if (lastState != state)
		{
			if (displayList != 0)
			{
				glDeleteLists(displayList, 1);
				displayList = 0;
			}
			meshInfo.frameInfos_[frameNo].lastCachedState = state;
		}

		if (!displayList)
		{
			glNewList(displayList = glGenLists(1), GL_COMPILE);
				drawVerts(m, mesh, vertexLighting, frame);
			glEndList();

			meshInfo.frameInfos_[frameNo].displayList = displayList;
		}

		glCallList(displayList);
		GLInfo::addNoTriangles((int) mesh->getFaces().size());
	}

	if (useTextures)
	{
		if (mesh->getSphereMap())
		{
			glDisable(GL_TEXTURE_GEN_S);						
			glDisable(GL_TEXTURE_GEN_T);	
			glDisable(GL_TEXTURE_GEN_R);
		}
	}
}

void ModelRenderer::drawVerts(unsigned int m, Mesh *mesh, bool vertexLighting, int frame)
{
	// Move the bones into position
	for (unsigned int b=0; b<boneTypes_.size(); b++)
	{
		Bone *bone = model_->getBones()[b];
		BoneType *type = boneTypes_[b];

		unsigned int posKeys = bone->getPositionKeys().size();
		unsigned int rotKeys = bone->getRotationKeys().size();
		if (posKeys == 0 && rotKeys == 0)
		{
			memcpy(type->final_, type->absolute_, sizeof(BoneMatrixType));
			continue;
		}

		BoneMatrixType m;
		bone->getRotationAtTime(float(frame), m);
				
		Vector &pos = bone->getPositionAtTime(float(frame));
		m[0][3] = pos[0];
		m[1][3] = pos[1];
		m[2][3] = pos[2];

		ModelMaths::concatTransforms(type->relative_, m, type->relativeFinal_);
		if (type->parent_ == -1)
		{
			memcpy(type->final_, type->relativeFinal_, sizeof(BoneMatrixType));
		}
		else
		{
			BoneType *parent = boneTypes_[type->parent_];
			ModelMaths::concatTransforms(parent->final_, type->relativeFinal_, type->final_);
		}
	}

	// Draw the vertices
	Vector vec;
	glBegin(GL_TRIANGLES);

	int faceVerts[3];
	
	std::vector<Face *>::iterator itor;
	for (itor = mesh->getFaces().begin();
		itor != mesh->getFaces().end();
		itor++)
	{
		Face *face = *itor;

		for (int i=0; i<3; i++)
		{
			int index = face->v[i];
			faceVerts[i] = index;
		}
		
		if (faceVerts[0] != faceVerts[1] &&
			faceVerts[1] != faceVerts[2] &&
			faceVerts[0] != faceVerts[2])
		{
			GLInfo::addNoTriangles(1);
			for (int i=0; i<3; i++)
			{
				Vertex *vertex = mesh->getVertex(faceVerts[i]);

				if (vertexLighting)
				{
					if (GLState::getState() & GLState::TEXTURE_OFF) 
					{
						glColor3f(
							mesh->getDiffuseNoTexColor()[0] * vertex->lightintense[0],
							mesh->getDiffuseNoTexColor()[1] * vertex->lightintense[1],
							mesh->getDiffuseNoTexColor()[2] * vertex->lightintense[2]);
					}
					else
					{
						glColor3fv(vertex->lightintense);
					}
				}

				glTexCoord2f(face->tcoord[i][0], face->tcoord[i][1]);
				glNormal3fv(face->normal[i]);

				if (vertex->boneIndex != -1)
				{
					BoneType *type = boneTypes_[vertex->boneIndex];

					// Note: Translation of MS to S3D coords
					Vector newPos, newVec;
					newPos[0] = vertex->position[0];
					newPos[1] = vertex->position[2];
					newPos[2] = vertex->position[1];

					ModelMaths::vectorRotate(newPos, type->final_, newVec);
					vec[0] = newVec[0];
					vec[1] = newVec[2];
					vec[2] = newVec[1];

					vec[0] += type->final_[0][3] + vertexTranslation_[0];
					vec[1] += type->final_[2][3] + vertexTranslation_[1];
					vec[2] += type->final_[1][3] + vertexTranslation_[2];
					
				}
				else
				{
					vec[0] = vertex->position[0] + vertexTranslation_[0];
					vec[1] = vertex->position[1] + vertexTranslation_[1];
					vec[2] = vertex->position[2] + vertexTranslation_[2];
				}

				glVertex3fv(vec);
			}
		}
	}
	glEnd();
}
