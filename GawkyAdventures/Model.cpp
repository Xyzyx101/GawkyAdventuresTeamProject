#include "Model.h"

Model::Model() :
vb( 0 ),
ib( 0 ),
indexFormat( DXGI_FORMAT_R16_UINT ),
vertexStride( 0 ) {}

Model::~Model() {
	ReleaseCOM( vb );
	ReleaseCOM( ib );
}