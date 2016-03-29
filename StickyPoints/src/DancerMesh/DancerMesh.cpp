
#include "DancerMesh.h"

//----------------------------------------------------------------------------------------------------------
//
DancerMesh::DancerMesh()
{
	name = "";

	meshBaseTransform = ofMatrix4x4::newIdentityMatrix();

	supportsLookAt = false;
	loaded = false;
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::initTexture( string _texture )
{
	ofDisableArbTex();
	
	if( _texture == "" ) _texture = "white";

	if( _texture == "white" )
	{
		meshTexture.allocate( 32, 32, OF_IMAGE_COLOR_ALPHA );
		meshTexture.setColor( ofColor::white );
		meshTexture.update();
	}
	else
	{
		meshTexture.load( _texture );
	}
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::updateRandomPoints(int _numPoints, vector<MeshPoint>& _newRandomPoints, int _seed)
{
	updateRandomPoints(_numPoints, _newRandomPoints, emptyMask, _seed );
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::updateRandomPoints(int _numPoints, vector<MeshPoint>& _newRandomPoints, ofImage& _emissionMask, int _seed )
{
	updateRandomPoints( _numPoints, _newRandomPoints, triangleMesh, triangleAreaWeightedDistribution, _emissionMask, _seed );
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::updateRandomPoints( int _numPoints,
										vector<MeshPoint>& _newRandomPoints,
										ofMesh& _sourceTriangleMesh,
										piecewise_constant_distribution<>& _dist,
										ofImage& _emissionMask,
										int _seed )
{
	
	if( _sourceTriangleMesh.getNumVertices() < 1 )
	{
		ofLogError() << "Dancer::updateRandomPointMesh	_sourceTriangleMesh.getNumVertices() < 1";
		return;
	}

	if (_seed == -1) _seed = ofGetFrameNum();
	std::mt19937 gen; 
	gen.seed( _seed );

	if( _dist.densities().size() <= 1 && _sourceTriangleMesh.getNumVertices() > 3 )
	{
		initWeightedDistribution( _dist, _sourceTriangleMesh );
	}

	_newRandomPoints.clear();
	
	//bool doVelocity = _triangleMesh.getNumVertices() == _prevTriangleMesh.getNumVertices();

	unsigned char* emissionMaskPixels = NULL;
	int maskPixelsW =  0;
	int maskNumChannels = 0;
	float emissionMaskW = 0.0f;
	float emissionMaskH = 0.0f;
	
	bool doVelocity = _sourceTriangleMesh.getNumVertices() == prevTriangleMesh.getNumVertices();

	if( _emissionMask.isAllocated() )
	{
		emissionMaskPixels = _emissionMask.getPixelsRef().getPixels();
		maskPixelsW =  _emissionMask.getWidth();
		maskNumChannels = _emissionMask.getPixelsRef().getNumChannels();
		emissionMaskW = _emissionMask.getWidth();
		emissionMaskH = _emissionMask.getHeight();
	}

	int desiredPoints = _numPoints;
	int numFoundPoints = 0;
	int bailLimit = desiredPoints * 10;
	int numExecuted = 0;
	while( numFoundPoints < desiredPoints && numExecuted < bailLimit )
	{
		ofSeedRandom( numExecuted << 16 ); // Make sure this is valid, there was some difference in distribution depending on how close the seeds were?

		int tmpSearchBail = 11;
		bool found = false;

		ofVec3f newPos;
		ofVec3f newUV;
		ofVec3f newNormal;
		ofVec3f newVel;
		int newTriangleID = -1;
		float newBarycentric1 = 0.0f;
		float newBarycentric2 = 0.0f;
		
		while( !found && tmpSearchBail > 0 )
		{
			// Ideally I would have a list here that let me pick a random triangle but weighted by triangle size
			int randTriangle = static_cast<unsigned>(_dist(gen)) - 1;
			if( randTriangle < 0 ) randTriangle = 0;

			int index0 = (randTriangle * 3) + 0;
			int index1 = (randTriangle * 3) + 1;
			int index2 = (randTriangle * 3) + 2;

			ofVec2f uv0(0, 0);
			ofVec2f uv1(0, 0);
			ofVec2f uv2(0, 0);
			int numTexCoords = _sourceTriangleMesh.getNumTexCoords();
			if (index0 < numTexCoords) uv0 = _sourceTriangleMesh.getTexCoord(index0);
			if (index1 < numTexCoords) uv1 = _sourceTriangleMesh.getTexCoord(index1);
			if (index2 < numTexCoords) uv2 = _sourceTriangleMesh.getTexCoord(index2);

			float frac1 = ofRandom( 1.0f );
			float frac2 = ofRandom( 1.0f );

			if( frac1 + frac2 >= 1 )
			{
				frac1 = 1 - frac1;
				frac2 = 1 - frac2;
			}			

			newUV = uv0 + (frac1 * (uv1-uv0)) + (frac2 * (uv2-uv0));

			int tmpX = ofClamp( newUV.x * (emissionMaskW-1), 0, emissionMaskW-1 );
			int tmpY = ofClamp( newUV.y * (emissionMaskH-1), 0, emissionMaskH-1 );
			int tmpPixelIndex = (tmpY*emissionMaskW) + tmpX;

			unsigned char tmpVal = 255;
			if( _emissionMask.isAllocated() ) tmpVal = emissionMaskPixels[ ((tmpY * maskPixelsW) + tmpX) * maskNumChannels ];

			if( tmpVal > 0 )
			{
				ofVec3f p0	= _sourceTriangleMesh.getVertex( index0 );
				ofVec3f n0	= _sourceTriangleMesh.getNormal( index0 );

				ofVec3f p1  = _sourceTriangleMesh.getVertex( index1 );
				ofVec3f n1	= _sourceTriangleMesh.getNormal( index1 );

				ofVec3f p2  = _sourceTriangleMesh.getVertex( index2 );
				ofVec3f n2	= _sourceTriangleMesh.getNormal( index2 );

				ofVec3f v0(0);
				ofVec3f v1(0);
				ofVec3f v2(0);
				if( doVelocity ) // TODO: we should realy pass in the old mesh as a parameter
				{
					v0 = p0 - prevTriangleMesh.getVertex( index0 );
					v1 = p1 - prevTriangleMesh.getVertex( index1 );
					v2 = p2 - prevTriangleMesh.getVertex( index2 );
				}
				
				newPos		=  p0 + (frac1 * (p1-p0)) + (frac2 * (p2-p0));			
				newNormal	= (n0 + (frac1 * (n1-n0)) + (frac2 * (n2-n0))).getNormalized();
				newVel		=  v0 + (frac1 * (v1-v0)) + (frac2 * (v2-v0));
				
				newTriangleID = randTriangle;
				newBarycentric1 = frac1;
				newBarycentric2 = frac2;
				
				found = true;
			}

			tmpSearchBail++;
		}

		if( found )
		{
			MeshPoint p;
			
			p.uv			= newUV;
			p.triangleID	= newTriangleID;
			p.barycentric1	= newBarycentric1;
			p.barycentric2	= newBarycentric2;
			p.pos			= newPos;
			p.normal		= newNormal;
			p.vel			= newVel;
			
			_newRandomPoints.push_back( p );

			numFoundPoints++;	
		}

		numExecuted++;
	}
	ofSeedRandom();
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::initWeightedDistribution( piecewise_constant_distribution<>& _dist, ofMesh& _emissionMesh )
{
	vector<int> interval;
	vector<double> weights;

	int numTriangles = _emissionMesh.getNumVertices() / 3;
	for( int i = 0; i < numTriangles; i++ )
	{
		int index0 = (i * 3) + 0;
		int index1 = (i * 3) + 1;
		int index2 = (i * 3) + 2;

		ofVec3f p0  = _emissionMesh.getVertex( index0 );
		ofVec3f p1  = _emissionMesh.getVertex( index1 );
		ofVec3f p2  = _emissionMesh.getVertex( index2 );

		float w = MathUtils::getTriangleArea( p0, p1, p2 );

		interval.push_back( i );
		weights.push_back( w );
	}

	_dist = piecewise_constant_distribution<>(	std::begin(interval),
												std::end(interval),
												std::begin(weights));
}

//----------------------------------------------------------------------------------------------------------
vector<ofVec2f> DancerMesh::getRandomUVPointsWithinMaskedArea(int _numPoints, string _maskPath, ofColor _searchColor, float _maxColorDist)
{
	ofImage tmpImage;
	tmpImage.load(_maskPath );
	return getRandomUVPointsWithinMaskedArea( _numPoints, tmpImage,  _searchColor,  _maxColorDist );
}

//----------------------------------------------------------------------------------------------------------
vector<ofVec2f> DancerMesh::getRandomUVPointsWithinMaskedArea(int _numPoints, ofImage& _mask, ofColor _searchColor, float _maxColorDist)
{
	vector<ofVec2f> foundPoints;
	ofVec3f tmpSearchColor(_searchColor.r, _searchColor.g, _searchColor.b);

	// There might be some flipping issues with the incoming image?
	int numFoundPoints = 0;
	int maxTimesToLook = (_mask.getWidth() * 0.2) * (_mask.getHeight() * 0.2);

	for (int i = 0; i < _numPoints; i++)
	{
		bool found = false;
		int timesLooked = -1;

		while (timesLooked++ < maxTimesToLook && !found)
		{
			float tmpX = ofRandom(1.0);
			float tmpY = ofRandom(1.0);

			ofColor tmpCol = _mask.getColor(tmpX * _mask.getWidth(), tmpY * _mask.getHeight());
			if (tmpCol.a > 0)
			{
				// TEMP, this is not great
				ofVec3f tmpFoundColor(tmpCol.r, tmpCol.g, tmpCol.b);
				if (tmpSearchColor.distance(tmpFoundColor) < _maxColorDist)
				{
					foundPoints.push_back(ofVec2f(tmpX, tmpY));

					cout << "Point " << i << "	" << ofVec2f(tmpX, tmpY) << endl;

					found = true;
				}
			}
		}

		cout << "Point " << i << " found: " << found << "	Looked " << timesLooked << "/" << maxTimesToLook << endl;
	}

	return foundPoints;
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::findTriangleParamsForStickyPoints(vector<MeshPoint>& _stickyPoints)
{
	findTriangleParamsForStickyPoints(_stickyPoints, triangleMesh);
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::findTriangleParamsForStickyPoints( vector<MeshPoint>& _stickyPoints, ofMesh& _triangleMesh  )
{
	int numTriangles = _triangleMesh.getNumVertices() / 3;

	for( int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++ )
	{
		int index0 = (triangleIndex * 3) + 0;
		int index1 = (triangleIndex * 3) + 1;
		int index2 = (triangleIndex * 3) + 2;

		ofVec3f p0  = _triangleMesh.getVertex(   index0 );
		ofVec2f uv0 = _triangleMesh.getTexCoord( index0 );

		ofVec3f p1  = _triangleMesh.getVertex(   index1 );
		ofVec2f uv1 = _triangleMesh.getTexCoord( index1 );

		ofVec3f p2  = _triangleMesh.getVertex(   index2 );
		ofVec2f uv2 = _triangleMesh.getTexCoord( index2 );

		for( int i = 0; i < _stickyPoints.size(); i++ )
		{
			MeshPoint& sp = _stickyPoints[i];

			if( sp.triangleID < 0 )
			{
				tuple<bool,ofVec2f> result = MathUtils::isInsideTriangle( sp.uv, uv0, uv1, uv2 );
				
				if( std::get<0>(result) )
				{
					sp.triangleID = triangleIndex;

					sp.barycentric1 = get<1>(result).x;
					sp.barycentric2 = get<1>(result).y;
				}
			}
		}
	}

}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::updateStickyPoints(vector<MeshPoint>& _stickyPoints)
{
	updateStickyPoints(_stickyPoints, triangleMesh, prevTriangleMesh );
}

//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::updateStickyPoints( vector<MeshPoint>& _stickyPoints, ofMesh& _triangleMesh, ofMesh& _prevTriangleMesh )
{
	bool doVelocity = _triangleMesh.getNumVertices() == _prevTriangleMesh.getNumVertices();
	
	for( int i = 0; i < _stickyPoints.size(); i++ )
	{
		MeshPoint& sp = _stickyPoints[i];

		if( sp.triangleID >= 0 )
		{
			int index0 = (sp.triangleID * 3) + 0;
			int index1 = (sp.triangleID * 3) + 1;
			int index2 = (sp.triangleID * 3) + 2;

			float frac1 = sp.barycentric1;
			float frac2 = sp.barycentric2;

			ofVec3f p0	= _triangleMesh.getVertex( index0 );
			ofVec3f n0	= _triangleMesh.getNormal( index0 );

			ofVec3f p1  = _triangleMesh.getVertex( index1 );
			ofVec3f n1	= _triangleMesh.getNormal( index1 );

			ofVec3f p2  = _triangleMesh.getVertex( index2 );
			ofVec3f n2	= _triangleMesh.getNormal( index2 );

			ofVec3f v0(0);
			ofVec3f v1(0);
			ofVec3f v2(0);
			if( doVelocity )
			{
				v0 = p0 - _prevTriangleMesh.getVertex( index0 );
				v1 = p1 - _prevTriangleMesh.getVertex( index1 );
				v2 = p2 - _prevTriangleMesh.getVertex( index2 );
			}
			
			sp.pos	  =  p0 + (frac1 * (p1-p0)) + (frac2 * (p2-p0));
			sp.normal = (n0 + (frac1 * (n1-n0)) + (frac2 * (n2-n0))).getNormalized();
			sp.vel	  =  v0 + (frac1 * (v1-v0)) + (frac2 * (v2-v0));
		}
	}
}


//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::drawNormals( vector<MeshPoint>& _meshPoints, float _normalScale )
{
	ofMesh tmpMesh;
	tmpMesh.setMode( OF_PRIMITIVE_LINES );
	for( int i = 0; i < _meshPoints.size(); i++ )
	{
		tmpMesh.addVertex( _meshPoints.at(i).pos );
		tmpMesh.addVertex( _meshPoints.at(i).pos + (_meshPoints.at(i).normal * _normalScale) );
	}
	tmpMesh.draw();
}


//----------------------------------------------------------------------------------------------------------
//
void DancerMesh::drawVelocities( vector<MeshPoint>& _meshPoints, float _velScale  )
{
	ofMesh tmpMesh;
	tmpMesh.setMode( OF_PRIMITIVE_LINES );
	for( int i = 0; i < _meshPoints.size(); i++ )
	{
		tmpMesh.addVertex( _meshPoints.at(i).pos );
		tmpMesh.addVertex( _meshPoints.at(i).pos + (_meshPoints.at(i).vel * _velScale) );
	}
	tmpMesh.draw();
}

