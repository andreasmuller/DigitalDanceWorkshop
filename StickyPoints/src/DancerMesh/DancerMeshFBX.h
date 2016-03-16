#pragma once

#include "DancerMesh.h"
#include "ofxFBX.h"

class DancerMeshFBX : public DancerMesh
{
	public:

		// ---------------------------------------------
		DancerMeshFBX()
		{
			eyeMeshName = "Eyes";
			meshSkipList.push_back( eyeMeshName );
			animationTime = 0;

			lookAt.set(false);

			supportsLookAt = true;
		}

		// ---------------------------------------------
		~DancerMeshFBX()
		{
		
		}

		// ---------------------------------------------
		bool load(string _inputFilePath, string _texture = "white" )
		{
			ofxFBXSceneSettings settings;

			loaded = false;
			loaded = scene.load(_inputFilePath, settings);
	
			if( loaded ) 	{ /*cout << "DancerMeshFBX::load Loaded the scene OK" << endl;*/ }
			else 			{ cout << "DancerMeshFBX::load Error loading the scene" << endl; }
	
			if( !loaded )
			{
				return false;
			}

			fbx.setup( &scene );
			fbx.setAnimation(0);
			fbx.update();
			fbx.lateUpdate();

			//cout << fbx.getSkeletonInfo() << endl;

			updateTriangleMesh(triangleMesh);

			initTexture(_texture);

			ofDisableArbTex();
			eyeTexture.loadImage("Textures/Dancer/blue_eye_small.png");

			ofLogNotice() << "DancerMeshFBX speeds up the animation on update as a hack";

			return loaded;
		}

		// ---------------------------------------------
		void addToPanel(ofxPanel* _panel)
		{
			_panel->add( lookAt.set("Look At", true ) );
			_panel->add(minAngleBeforeTurningTowards.set("Min Angle", 20, 0, 180));
			_panel->add(maxAngleBeforeTurningTowards.set("Max Angle", 40, 0, 360));
			_panel->add(maxLookAtFrac.set("Max Look At Fraction", 1, 0, 1));

			_panel->add(modelLookAtTargetPos.set("Model Look At", ofVec3f(0,4, -10), ofVec3f(-10,0,-10), ofVec3f(10, 6, 10)));
		}

		// ---------------------------------------------
		void update( float _time )
		{
			if (!loaded) return;

	// TEMP		
			ofxFBXAnimation& currentAnimation = fbx.getCurrentAnimation();
			currentAnimation.setFramerate(120.0f);
			
		/*	
			currentAnimation.stop();
			float animationLength = currentAnimation.getDurationSeconds();

			animationTime += ofGetLastFrameTime() * 1.0;
			animationTime = fmodf(animationTime, animationLength);

			float animationTimeFrac = animationTime / animationLength;
			int desiredAnimationFrame = animationTimeFrac * currentAnimation.getTotalNumFrames();

			currentAnimation.setFrame(desiredAnimationFrame);
			*/

			ofxFBXBone* bone = NULL;

			vector<string> headBoneNameList;// = {"Head", "DEF-head"}; // Change back when I can use C++11
			headBoneNameList.push_back("Head");
			headBoneNameList.push_back("DEF-head");

			for (auto tmpName : headBoneNameList) { if( bone == NULL ) bone = fbx.getBone(tmpName); }

			if (bone != NULL)
			{
				bone->enableAnimation();
			}
					
			fbx.update();

				// Perform any bone manipulation here
				if( bone != NULL && lookAt )
				{
					updateLookAt(bone);
					bone->disableAnimation();
					bone->setOrientation( interpolatedLookAtNode.getOrientationQuat() );
				}

			fbx.lateUpdate();

			//cout << currentAnimation.isFrameNew() << " ";
			if (currentAnimation.isFrameNew() )
			{
				updateTriangleMesh(triangleMesh);
			}
		}

		//--------------------------------------------------------------
		void updateLookAt( ofxFBXBone* _bone )
		{
			ofVec3f headWorlPos = _bone->getGlobalTransformMatrix().getTranslation();
			ofVec3f modelLookAtTargetBonePos = modelLookAtTargetPos.get() * meshBaseTransform.getInverse();
			ofVec3f towardsTarget = (modelLookAtTargetBonePos - headWorlPos).getNormalized();

			headNode = *_bone;

			// Find difference between current head tranform and the head looking at the target
			lookingAtTargetNode = headNode;
			lookingAtTargetNode.lookAt(modelLookAtTargetBonePos);
			lookingAtTargetNode.rotate(180, lookingAtTargetNode.getUpDir());

			// The nodes coming from the FBX are looking in the Z+ direction, for now let's just to a test with rotated nodes 

			// TEMP, this will be wrong as we are working in bone space
			distToTarget = headWorlPos.distance(modelLookAtTargetBonePos);
			distWithNodeScale = distToTarget * (1.0 / lookingAtTargetNode.getGlobalTransformMatrix().getScale().x);

			angleBetweenCurrentHeadOrientationAndLookAt = headNode.getLookAtDir().angle(lookingAtTargetNode.getLookAtDir());

			lookAtLerpAmount = 0.0;
			if (ofGetKeyPressed(OF_KEY_LEFT_SHIFT))
			{
				lookAtLerpAmount = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1);
			}
			else
			{
				lookAtLerpAmount = 1 - MathUtils::smoothStep(minAngleBeforeTurningTowards, maxAngleBeforeTurningTowards, angleBetweenCurrentHeadOrientationAndLookAt);
			}

			lookAtLerpAmount = ofMap(lookAtLerpAmount, 0, 1,	0, maxLookAtFrac);

			interpolatedLookAtNode = headNode;
			ofQuaternion newLookAtNodeOrientation;
			newLookAtNodeOrientation.slerp(lookAtLerpAmount, headNode.getOrientationQuat(), lookingAtTargetNode.getOrientationQuat());
			interpolatedLookAtNode.setOrientation(newLookAtNodeOrientation);
		}

		// ---------------------------------------------
		void updateTriangleMesh( ofMesh& _mesh )
		{
			if (!loaded) return;

			vector<ofMesh>& fbxMeshes = fbx.getMeshes();
			bool randomTriangleColor = true;

			ofVec3f normalFlip(1,1,1); 

			// Go through the meshes in the FBX and combine them
			_mesh.clear();
			_mesh.setMode( OF_PRIMITIVE_TRIANGLES );

			int triangleCounter = 0;
			for(int meshIndex = 0; meshIndex < fbxMeshes.size(); meshIndex++ ) 
			{ 
				string meshName = fbx.getMeshName(meshIndex);
				bool skipMesh = ofContains( meshSkipList, meshName );

				if( !skipMesh )
				{
					ofMesh& sourceMesh = fbxMeshes.at(meshIndex);

					ofMatrix4x4 meshTransform = meshBaseTransform;
					if (fbx.meshTransforms.size() > meshIndex)
					{
						meshTransform = fbx.meshTransforms.at(meshIndex).getGlobalTransformMatrix() * meshTransform;
					}

					if( sourceMesh.getMode() == OF_PRIMITIVE_TRIANGLES )
					{
						bool meshIsValid = true;

						// TODO: we can't expect the mesh to be set up this way, so we need need a more general way to read the meshes
						if( (sourceMesh.getNumIndices() % 3) != 0 )							meshIsValid = false;
						if( sourceMesh.getNumIndices() != sourceMesh.getNumVertices() )		meshIsValid = false;
						if( sourceMesh.getNumVertices() != sourceMesh.getNumTexCoords() )	meshIsValid = false;		

						if( meshIsValid )
						{
							for( int i = 0; i < sourceMesh.getNumIndices() / 3; i++ )
							{
								// Notice we're rearranging the vertex order here
								int index0 = (i * 3) + 2;
								int index1 = (i * 3) + 1;
								int index2 = (i * 3) + 0;

								ofVec3f vertex0 = sourceMesh.getVertex(index0) * meshTransform;
								ofVec3f vertex1 = sourceMesh.getVertex(index1) * meshTransform;
								ofVec3f vertex2 = sourceMesh.getVertex(index2) * meshTransform;

								ofVec3f normal0 = ofMatrix4x4::transform3x3((sourceMesh.getNormal(index0) * normalFlip), meshTransform );
								ofVec3f normal1 = ofMatrix4x4::transform3x3((sourceMesh.getNormal(index1) * normalFlip), meshTransform );
								ofVec3f normal2 = ofMatrix4x4::transform3x3((sourceMesh.getNormal(index2) * normalFlip), meshTransform );

								ofSeedRandom( triangleCounter << 24 );
								ofFloatColor tmpCol = ofFloatColor::white;
								if( randomTriangleColor ) tmpCol = ofFloatColor::fromHsb(ofRandom(1.0), 0.8, 0.8);

								_mesh.addVertex( vertex0 );
								_mesh.addTexCoord( sourceMesh.getTexCoord( index0 ) );
								_mesh.addNormal( normal0.getNormalized() );
								_mesh.addColor( tmpCol );

								_mesh.addVertex( vertex1 );
								_mesh.addTexCoord( sourceMesh.getTexCoord( index1 ) );
								_mesh.addNormal( normal1.getNormalized());
								_mesh.addColor( tmpCol );

								_mesh.addVertex( vertex2 );
								_mesh.addTexCoord( sourceMesh.getTexCoord( index2 ) );
								_mesh.addNormal( normal2.getNormalized());
								_mesh.addColor( tmpCol );

								triangleCounter++;
							}
						}
					}
				}
			}

			ofSeedRandom();
		}

		// ---------------------------------------------
		ofMesh* getMesh( string _name, ofMatrix4x4* _meshTransform = NULL )
		{
			vector<ofMesh>& fbxMeshes = fbx.getMeshes();
			for( int i = 0; i < fbxMeshes.size(); i++ )
			{
				if( fbx.getMeshName(i) == eyeMeshName )
				{
					if (_meshTransform != NULL)
					{
						if (i < fbx.meshTransforms.size())
						{
							_meshTransform->set( fbx.meshTransforms.at(i).getGlobalTransformMatrix() * meshBaseTransform );
						}
					}

					return &fbxMeshes.at(i);
				}
			}

			return NULL;
		}

		// ---------------------------------------------
		void debugDrawInfo( ofTrueTypeFontExt& _font, ofVec2f _pos )
		{
			if (!loaded) return;

			ofVec2f pos = _pos;
			ofVec2f nextLine(0,11);

			int numBones = 0;
			int numVertices = 0;

			vector<ofMesh>& fbxMeshes = fbx.getMeshes();

			vector< shared_ptr<ofxFBXSkeleton> >& skeletons = fbx.getSkeletons();
			for( int i = 0; i < skeletons.size(); i++ ) { numBones += skeletons[i]->getNumBones(); }
			for (int i = 0; i < fbxMeshes.size(); i++ ) { numVertices += fbxMeshes.at(i).getNumVertices(); }

	//		fontSmall.drawStringShadowed( fbx.getFbxScene()->getFbxFilePath().getFileName(),		pos ); pos += nextLine;

			_font.drawStringShadowed( "Vertices: "	+ ofToString(numVertices),		pos ); pos += nextLine;
			_font.drawStringShadowed( "Bones: "		+ ofToString(numBones),			pos ); pos += nextLine;
			_font.drawStringShadowed( "Skeletons: "	+ ofToString(skeletons.size()),	pos ); pos += nextLine;
			pos += nextLine;

			for(int i = 0; i < fbx.getNumAnimations(); i++ )
			{
				string tmpName = "";
				ofxFBXAnimation& anim = fbx.getAnimation( i );
				if( i == fbx.getCurrentAnimationIndex() )  { tmpName += "- "; }
				tmpName += "name: " + anim.name + " " + ofToString(anim.getPositionSeconds(), 3) + " | " + ofToString(anim.getDurationSeconds(), 3) + " frame: " + ofToString(anim.getFrameNum()) + " / " + ofToString(anim.getTotalNumFrames());

				_font.drawStringShadowed( tmpName, pos ); 
				pos += nextLine;
			}

			pos += nextLine;
			pos += nextLine;

			/*
			int numBones = 0;
			vector< shared_ptr<ofxFBXSkeleton> >& skeletons = fbx.getSkeletons();
			for (int i = 0; i < skeletons.size(); i++) { numBones += skeletons[i]->getNumBones(); }

			ofSetColor(60, 60, 60);
			stringstream ds;

			ds << "Scale is " << fbx.getScale() << endl;
			if (fbx.getNumPoses() > 0)
			{
				ds << "Pose: " << fbx.getCurrentPose()->getName() << " num poses: " << fbx.getNumPoses() << " enabled (p): " << fbx.arePosesEnabled() << endl;
			}
			ofDrawBitmapString(ds.str(), 10, 20);

			for (int i = 0; i < fbx.getNumAnimations(); i++)
			{
				stringstream ss;
				ofxFBXAnimation& anim = fbx.getAnimation(i);
				if (i == fbx.getCurrentAnimationIndex())
				{
					ss << "- ";
				}
				ss << "name: " << anim.name << " " << ofToString(anim.getPositionSeconds(), 3) << " | " << ofToString(anim.getDurationSeconds(), 3) << " frame: " << anim.getFrameNum() << " / " << anim.getTotalNumFrames() << endl;
				ofDrawBitmapString(ss.str(), 10, i * 30 + 300);
			}
			*/
		}

		// ---------------------------------------------
		void debugDraw()
		{
			if (!loaded) return;

		}

		float animationTime;

		ofImage eyeTexture;

		ofxFBXScene scene;
		ofxFBXManager fbx;	
};
