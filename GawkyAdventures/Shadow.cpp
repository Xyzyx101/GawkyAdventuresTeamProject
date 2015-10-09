#include "Shadow.h"

Shadow::Shadow()
	:playerPosition(0.0f, 0.0f, 0.0f), mShapesVB(0)
{
}
Shadow::~Shadow()
{

}
void Shadow::GetPlayerPos(XMFLOAT3 playerPos)
{
	playerPosition = playerPos;
}

void Shadow::CreateShadow(XMFLOAT3 playerPos, ID3D11DeviceContext* dc,ID3D11Device* dv)
{
	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();
	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoMake;
	
	world = XMLoadFloat4x4(&mSphereWorld);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world*view*proj;
	geoMake.CreateSphere(0.5f, 20, 20, sphere);

	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
	mSphereIndexCount = sphere.Indices.size();
	mSphereIndexOffset = 0;
//	mSphereVertexCount = sphere.Vertices.size();
	UINT totalVertexCount = sphere.Vertices.size();
	UINT totalIndexCount = sphere.Indices.size();
	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);
	UINT k = 0;
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].Tex = sphere.Vertices[i].TexC;
		//vertices[k].TangentU = sphere.Vertices[i].TangentU;
	}
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(dv->CreateBuffer(&vbd, &vinitData, &mShapesVB));
	dc->DrawIndexed(mSphereIndexCount, 0, 0);
}