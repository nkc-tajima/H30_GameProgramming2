#pragma once
#include <D3D11.h>			// DirectX11SDK���g�����߂ɕK�v�Ȃ���
#include <d3dcompiler.h>	// �V�F�[�_�[�t�@�C�����R���p�C�����邽�߂ɕK�v
#include <string>

#pragma comment(lib, "d3d11.lib")		// DirectX11���C�u�����𗘗p���邽�߂ɕK�v
#pragma comment(lib, "d3dCompiler.lib")	// �V�F�[�_�[�R�[�h���R���p�C�����邽�߂ɕK�v

// ���_�f�[�^�\����
struct vertex
{
	float pos[3];		// ���_���W	
	float col[4];		// ���_���Ƃ̐F
};

class CPolygon
{
private:
	ID3D11InputLayout*	pVertexLayout = nullptr;		// ���_�f�[�^�̍\�����`
	ID3D11Buffer*		pVertexBuffer = nullptr;		// ���_�f�[�^��ۑ�
	ID3D11VertexShader* pVertexShader = nullptr;		// ���_�V�F�[�_
	ID3D11PixelShader*	pPixelShader = nullptr;		// �s�N�Z���V�F�[�_
	char*				pCsoBin = nullptr;
	int					CsoSize;

	HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	// const wchar_t����string�ɂ�����
	HRESULT CreateVS(ID3D11Device* device, std::string vsStr);	// ���_�V�F�[�_�쐬
	HRESULT CreatePS(ID3D11Device* device, std::string psStr);	// �s�N�Z���V�F�[�_�쐬
	void    ReadCSOFile(LPCWSTR szFileName);

	vertex vertexList[4];
public:
	void Init(ID3D11Device* device, std::string vsStr, std::string psStr);// const wchar_t����string�ɂ�����
	void Render(ID3D11DeviceContext* context, ID3D11Buffer* buffer);
	void Release();
	void SetColor(float r, float g, float b, float a, int v);
};