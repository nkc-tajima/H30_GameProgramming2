#include "Polygon.h"
#include <fstream>
#include <string>
#include <stdlib.h>

D3D11_INPUT_ELEMENT_DESC vertexDesc[]
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


void CPolygon::Init(ID3D11Device* device, std::string vsStr, std::string psStr)
{
	CreateVS(device, vsStr);
	CreatePS(device, psStr);

	for (int i = 0; i < 4; i++)
	{
		// ���_�̈ʒu
		vertexList[i].pos[0] = i < 2 ? -0.5f : 0.5f;
		vertexList[i].pos[1] = i % 2 == 0 ? -0.5f : 0.5f;
		vertexList[i].pos[2] = 0.0f;
		// ���_�̐F
		vertexList[i].col[0] = 1.0f;
		vertexList[i].col[1] = 1.0f;
		vertexList[i].col[2] = 1.0f;
		vertexList[i].col[3] = 1.0f;
	}

	// �o�b�t�@���\�[�X�ɂ��ċL�q
	D3D11_BUFFER_DESC descBuffer;
	// ������
	ZeroMemory(&descBuffer, sizeof(descBuffer));
	// �o�b�t�@�őz�肳��Ă���ǂݍ��݋y�я������݂̕��@������
	descBuffer.Usage = D3D11_USAGE_DEFAULT;
	// �o�b�t�@�̃T�C�Y ����͎O�p�`��\������̂ŃT�C�Y��3��
	descBuffer.ByteWidth = sizeof(vertex) * 4;
	// �o�b�t�@���ǂ̂悤�ɃO���t�B�b�N�X�p�C�v���C���Ƀo�C���h���邩������
	descBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPU�̃A�N�Z�X�t���O
	descBuffer.CPUAccessFlags = 0;

	// �T�u���\�[�X�̏������Ɏg�p�����f�[�^���w��
	D3D11_SUBRESOURCE_DATA InitData;
	// ������
	ZeroMemory(&InitData, sizeof(InitData));
	// �������f�[�^�ւ̃|�C���^
	InitData.pSysMem = vertexList;
	// �e�N�X�`���ɂ���1�{�̐��̐�[����ׂ̐��܂ł̋���
	InitData.SysMemPitch = 0;
	// 3D�e�N�X�`���Ɋ֘A����l
	InitData.SysMemSlicePitch = 0;
	// �o�b�t�@�̍쐬
	device->CreateBuffer(
		// �o�b�t�@�̋L�q�ւ̃|�C���^
		&descBuffer,
		// �������f�[�^�ւ̃|�C���^
		&InitData,
		// �쐬�����o�b�t�@�ւ̃|�C���^�̃A�h���X
		&pVertexBuffer
	);
}

void CPolygon::Render(ID3D11DeviceContext* context, ID3D11Buffer* buffer)
{
	// �T�u���\�[�X�̍X�V
	context->UpdateSubresource(
		pVertexBuffer,
		0,
		nullptr,
		vertexList,
		0,
		0
	);
	// ���̓A�Z���u���X�e�[�W�ɓ��̓��C�A�E�g�I�u�W�F�N�g���o�C���h
	context->IASetInputLayout(pVertexLayout);
	// ���̓A�Z���u���X�e�[�W�ɒ��_�o�b�t�@�̔z����o�C���h
	UINT strides = sizeof(vertex);
	UINT offsets = 0;
	context->IASetVertexBuffers(
		0,					// �o�C���h�Ɏg�p����ŏ��̓��̓X���b�g
		1,					// �z����̒��_�o�b�t�@�̐�
		&pVertexBuffer,	// ���_�o�b�t�@�̔z��ւ̃|�C���^
		&strides,			// �X�g���C�h�l
		&offsets			// �I�t�Z�b�g�l
	);
	// ���_�V�F�[�_���f�o�C�X�ɐݒ�
	context->VSSetShader(
		pVertexShader,// ���_�V�F�[�_�ւ̃|�C���^
		nullptr,			// �N���X�C���X�^���X�C���^�[�t�F�C�X�̔z��ւ̃|�C���^
		0					// �z��̃N���X�C���X�^���X�C���^�[�t�F�C�X�̐�
	);
	// ���_�V�F�[�_�̃p�C�v���C���X�e�[�W�Ŏg�p�����萔�o�b�t�@��ݒ�
	context->VSSetConstantBuffers(
		0,					// �f�o�C�X�̔z��̒��Œ萔�o�b�t�@�̐ݒ���J�n����ʒu
		1,					// �ݒ肷��o�b�t�@�̐�
		&buffer	// �f�o�C�X�ɐݒ肷��萔�o�b�t�@�̔z��
	);

	// �s�N�Z���V�F�[�_���f�o�C�X�ɐݒ�
	context->PSSetShader(
		pPixelShader,		// �s�N�Z���V�F�[�_�ւ̃|�C���^
		nullptr, 			// �N���X�C���X�^���X�C���^�[�t�F�C�X�̔z��ւ̃|�C���^
		0					// �z��̃N���X�C���X�^���X�C���^�[�t�F�C�X�̐�
	);
	// ���̓A�Z���u���X�e�[�W�Ƀv���~�e�B�u�̌`����o�C���h
	context->IASetPrimitiveTopology(
		// ���_�f�[�^���O�p�`�̃��X�g�Ƃ��ĉ���
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		// �����̎O�p�`���o�������Ƃ���TRIANGLESTRIP�ɂ���ƒ��_��ߖ�ł���
	);
	// �v���~�e�B�u��`��
	context->Draw(
		4,		// �`�悷�钸�_�̐�
		0		// �ŏ��̒��_�̃C���f�b�N�X
	);

}

void CPolygon::Release()
{
	if (pVertexLayout)pVertexLayout->Release();
	if (pVertexBuffer)pVertexBuffer->Release();
	if (pVertexShader)pVertexShader->Release();
	if (pPixelShader) pPixelShader->Release();
}
void CPolygon::SetColor(float r, float g, float b, float a, int v)
{
	vertexList[v].col[0] = r;
	vertexList[v].col[1] = g;
	vertexList[v].col[2] = b;
	vertexList[v].col[3] = a;
}

// �V�F�[�_�[�t�@�C���ǂݍ���
HRESULT CPolygon::CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	// �R���p�C����HLSL�R�[�h���R���p�C��������@���w��
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;// �����ȃR���p�C��������
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;				// �f�o�b�O�t�@�C��/�s/�^/�V���{�������o�̓R�[�h�ɑ}������悤�ɃR���p�C���Ɏw��

	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;	// �R�[�h�������ɍœK���X�e�b�v���X�L�b�v����悤�ɃR���p�C���Ɏw��
#endif

	ID3DBlob* pErrorBlob = nullptr;					// �R���p�C����̃o�C�i���f�[�^���擾����
													// �t�@�C������V�F�[�_���R���p�C��
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			// ��������f�o�b�K�ɑ���
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT CPolygon::CreateVS(ID3D11Device* device, std::string vsStr)
{
	// ����������ɍs��ꂽ�����f����ׂɗp��
	HRESULT hr = S_OK;
	// �t�@�C���̊g���q���擾����
	int num = vsStr.find_last_of('.');
	std::string extension = vsStr.substr(num + 1);
	// string����const wchar_t�ɕϊ�����
	wchar_t *vs = new wchar_t[vsStr.length() + 1];
	size_t value = 0;
	mbstowcs_s(&value, vs, 20, vsStr.c_str(), vsStr.length() + 1);


	// �ǂݍ��ފg���q��hlsl?
	if (extension == "hlsl")
	{
		ID3DBlob* pVSBlob = nullptr;			// �I�u�W�F�N�g�R�[�h�ƃG���[���b�Z�[�W��Ԃ�
												// �V�F�[�_�[�t�@�C���ǂݍ���
		hr = CompileShaderFromFile(
			vs,					// �t�@�C���p�X
			"main",				// �G���g���[�|�C���g�̖��O
			"vs_5_0",			// �V�F�[�_�[�̃o�[�W����
			&pVSBlob			// �ۑ�����ϐ� 
		);
		if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

		// ���_�V�F�[�_�̍쐬
		hr = device->CreateVertexShader(
			pVSBlob->GetBufferPointer(),	// �R���p�C���ς݃V�F�[�_�ւ̃|�C���^
			pVSBlob->GetBufferSize(),		// �R���p�C���ς݃V�F�[�_�̃T�C�Y
			nullptr,						// �N���X�����N�C���^�[�t�F�C�X�ւ̃|�C���^
			&pVertexShader					// ID3D11VertexShader�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		);
		if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

		// �C���v�b�g���C�A�E�g�̍쐬
		hr = device->CreateInputLayout(
			vertexDesc,					// ���̓f�[�^�^�̔z��
			ARRAYSIZE(vertexDesc),		// �z����̓��̓f�[�^�^�̐�
			pVSBlob->GetBufferPointer(),// �R���p�C���ς݃V�F�[�_�ւ̃|�C���^
			pVSBlob->GetBufferSize(),	// �R���p�C���ς݃V�F�[�_�̃T�C�Y
			&pVertexLayout				// �쐬�������̓��C�A�E�g�I�u�W�F�N�g�ւ̃|�C���^
		);
		pVSBlob->Release();				// ���		
		if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N
	}
	// �ǂݍ��ފg���q��cso?
	else if (extension == "cso")
	{
		ReadCSOFile(vs);

		// ���_�V�F�[�_�̍쐬
		hr = device->CreateVertexShader(
			pCsoBin,						// �R���p�C���ς݃V�F�[�_�ւ̃|�C���^
			CsoSize,						// �R���p�C���ς݃V�F�[�_�̃T�C�Y
			nullptr,						// �N���X�����N�C���^�[�t�F�C�X�ւ̃|�C���^
			&pVertexShader					// ID3D11VertexShader�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		);
		if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

										// �C���v�b�g���C�A�E�g�̍쐬
		hr = device->CreateInputLayout(
			vertexDesc,					// ���̓f�[�^�^�̔z��
			ARRAYSIZE(vertexDesc),		// �z����̓��̓f�[�^�^�̐�
			pCsoBin,					// �R���p�C���ς݃V�F�[�_�ւ̃|�C���^
			CsoSize,					// �R���p�C���ς݃V�F�[�_�̃T�C�Y
			&pVertexLayout				// �쐬�������̓��C�A�E�g�I�u�W�F�N�g�ւ̃|�C���^
		);
	}

	return hr;
}


HRESULT CPolygon::CreatePS(ID3D11Device* device, std::string psStr)
{
	// ����������ɍs��ꂽ�����f����ׂɗp��
	HRESULT hr = S_OK;
	// �t�@�C���̊g���q���擾����
	int num = psStr.find_last_of('.');
	std::string extension = psStr.substr(num + 1);
	// string����const wchar_t�ɕϊ�����
	wchar_t *ps = new wchar_t[psStr.length() + 1];
	size_t value = 0;
	mbstowcs_s(&value, ps, 20, psStr.c_str(), psStr.length() + 1);


	// �ǂݍ��ފg���q��hlsl?
	if (extension == "hlsl")
	{
		ID3DBlob* pPSBlob = nullptr;			// �I�u�W�F�N�g�R�[�h�ƃG���[���b�Z�[�W��Ԃ�
	// �V�F�[�_�[�t�@�C���ǂݍ���
		hr = CompileShaderFromFile(
			ps,					// �t�@�C���p�X
			"main",				// �G���g���[�|�C���g�̖��O
			"ps_5_0",			// �V�F�[�_�[�̃o�[�W����
			&pPSBlob			// �ۑ�����ϐ� 
		);
		if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

		// �s�N�Z���V�F�[�_�̍쐬
		hr = device->CreatePixelShader(
			pPSBlob->GetBufferPointer(),// �R���p�C���ς݃V�F�[�_�ւ̃|�C���^
			pPSBlob->GetBufferSize(),	// �R���p�C���ς݃V�F�[�_�̃T�C�Y
			nullptr,					// �N���X�����N�C���^�[�t�F�C�X�ւ̃|�C���^
			&pPixelShader				// ID3D11PixelShader�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		);
		pPSBlob->Release();				// ���		

		if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N
	}
	// �ǂݍ��ފg���q��cso?
	else if (extension == "cso")
	{
		ReadCSOFile(ps);

		// �s�N�Z���V�F�[�_�̍쐬
		hr = device->CreatePixelShader(
			pCsoBin,					// �R���p�C���ς݃V�F�[�_�ւ̃|�C���^
			CsoSize,					// �R���p�C���ς݃V�F�[�_�̃T�C�Y
			nullptr,					// �N���X�����N�C���^�[�t�F�C�X�ւ̃|�C���^
			&pPixelShader				// ID3D11PixelShader�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		);

	}
	return hr;
}

// CSO�t�@�C����ǂݍ���
void  CPolygon::ReadCSOFile(LPCWSTR szFileName)
{
	std::ifstream binfile(szFileName, std::ios::in | std::ios::binary);

	if (binfile.is_open()) {
		int fsize = static_cast<int>(binfile.seekg(0, std::ios::end).tellg());
		binfile.seekg(0, std::ios::beg);
		char* code(new char[fsize]);
		binfile.read(code, fsize);
		CsoSize = fsize;
		pCsoBin = std::move(code);
	}
}
