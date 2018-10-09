#include <tchar.h>			// �e�L�X�g�╶������������߂ɕK�v�ȃw�b�_�[�t�@�C��
#include <Windows.h>		// Windows�v���O�����ŕK�v�ɂȂ���̂������Ă���
#include <D3D11.h>			// DirectX11SDK���g�����߂ɕK�v�Ȃ���
#include <d3dcompiler.h>	// �V�F�[�_�[�t�@�C�����R���p�C�����邽�߂ɕK�v
#include <directxcolors.h>	// DirectX::Colors�𗘗p���邽�߂ɕK�v

#include "Polygon.h"

#pragma comment(lib, "d3d11.lib")		// DirectX11���C�u�����𗘗p���邽�߂ɕK�v
#pragma comment(lib, "d3dCompiler.lib")	// �V�F�[�_�[�R�[�h���R���p�C�����邽�߂ɕK�v


using namespace std;
using namespace DirectX;


// ���W�ϊ��\����
struct ConstantBuffer
{
	XMMATRIX world;			// ���[���h�ϊ��s��
	XMMATRIX view;			// �r���[�ϊ��s��
	XMMATRIX projection;	// �v���W�F�N�V�����ϊ��s��
};

// ��ʃT�C�Y�ݒ�
#define WINDOW_W 1280
#define WINDOW_H 720

HWND g_hWnd;			// �E�B���h�E�n���h�� �E�B���h�E�𑀍삷�邽�߂ɕK�v�Ȃ���

						// Direct3D�f�o�C�X�̃^�[�Q�b�g�ƂȂ�@�\�Z�b�g���L�q
D3D_FEATURE_LEVEL		g_pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
UINT					g_FeatureLevels = 3;	  // �z��̗v�f��
D3D_FEATURE_LEVEL		g_FeatureLevelsSupported; // �f�o�C�X�쐬���ɕԂ����@�\���x��

												  // �C���^�[�t�F�C�X
ID3D11Device*			g_pd3dDevice = nullptr;			// �����_�����O����у��\�[�X�̍쐬�Ɏg�p
ID3D11DeviceContext*	g_pImmediateContext = nullptr;	// �����_�����O�R�}���h�𐶐�����f�o�C�X�R���e�L�X�g
IDXGISwapChain*			g_pSwapChain = nullptr;			// �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@���g���ĉ�ʂ��X�V����
ID3D11RenderTargetView*	g_pRenderTargetView = nullptr;	// �`�悷�邽�߂̃��\�[�X���쐬���A���ۂ̕`�摀����s��
D3D11_VIEWPORT			g_ViewPort;						// �`��͈�
ID3D11Texture2D*		g_pDepthStencil;				// �[�x/�X�e���V�����\�[�X�Ƃ��Ďg�p����
ID3D11DepthStencilView*	g_pDepthStencilView = nullptr;	// �[�x/�X�e���V�����\�[�X�ɃA�N�Z�X����

ID3D11VertexShader*     g_pVertexShader = nullptr;		// ���_�V�F�[�_
ID3D11PixelShader*      g_pPixelShader = nullptr;		// �s�N�Z���V�F�[�_

ID3D11InputLayout*      g_pVertexLayout = nullptr;		// ���_�f�[�^�̍\�����`
ID3D11Buffer*           g_pVertexBuffer = nullptr;		// ���_�f�[�^��ۑ�

ID3D11BlendState*		g_pBlendState = nullptr;		// �A���t�@�u�����f�B���O
ID3D11RasterizerState*  g_pRasterizerState = nullptr;	// ���X�^���C�U

														// ���W�ϊ��p�ϐ���`
ID3D11Buffer*           g_pConstantBuffer = nullptr;	// ���W�n�f�[�^��ۑ�
XMMATRIX				g_World;						// ���[���h�ϊ��s��
XMMATRIX				g_View;							// �r���[�ϊ��s��
XMMATRIX				g_Proj;							// �v���W�F�N�V�����ϊ��s��


void InitWindow(HINSTANCE hInstance);	 // �E�B���h�E����
HRESULT InitDevice();		// �f�o�C�X�ƃX���b�v�`�F�[��������
void CleanupDevice();		// �f�o�C�X�ƃX���b�v�`�F�[�����
void Render();				// �|���S���`��

// �E�B���h�E�v���V�[�W���i�R�[���o�b�N�֐��j���������C�x���g���Ƃɏ������s��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:		// ����{�^���������ꂽ�Ƃ�
		PostMessage(hWnd, WM_DESTROY, 0, 0);	// WM_DESTROY��
		break;
	case WM_DESTROY:	// �E�B���h�E���I������Ƃ�
		PostQuitMessage(0);	// WM_QUIT�𑗐M����
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)// esc�L�[�ŃE�B���h�E����
			DestroyWindow(hWnd);
		break;
	}
	// ����̃E�B���h�E�v���V�[�W�����Ăяo���ď�����Ԃ�
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// �E�B���h�E����
void InitWindow(HINSTANCE hInstance)
{
	// �E�B���h�E�N���X�o�^
	TCHAR szWindowClass[] = "DIRECTX11 LESSON 01";	// �E�B���h�E�N���X�����ʂ��镶����
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;	// �E�B���h�E�X�^�C��
	wcex.lpfnWndProc = WndProc;				// �E�B���h�E�̃��b�Z�[�W����������R�[���o�b�N�֐��ւ̃|�C���^
	wcex.cbClsExtra = 0;					// �E�B���h�E�N���X�\���̂̌��Ɋ��蓖�Ă�⑫�̃o�C�g��
	wcex.cbWndExtra = 0;					// �E�B���h�E�C���X�^���X�̌��Ɋ��蓖�Ă�⑫�̃o�C�g��
	wcex.hInstance = hInstance;				// ���̃N���X�̂��߂̃E�B���h�E�v���V�[�W��������n���h��
	wcex.hIcon = nullptr;					// �A�C�R��
	wcex.hCursor = nullptr;					// �}�E�X�J�[�\��
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);// �E�B���h�E�w�i�F
	wcex.lpszMenuName = nullptr;			// �f�t�H���g���j���[��
	wcex.lpszClassName = szWindowClass;		// �E�B���h�E�N���X��
	wcex.hIconSm = nullptr;					// �������A�C�R��
	RegisterClassEx(&wcex);					// �E�B���h�E�N���X�o�^

											// �E�B���h�E���� �������̓E�B���h�E�n���h���A���s����null���Ԃ�
	g_hWnd = CreateWindow(
		szWindowClass,				// RegisterClass()�œo�^�����N���X�̖��O
		"DirectX11 Lesson01",	// �^�C�g���o�[
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,// �E�B���h�E�X�^�C��
		CW_USEDEFAULT,				// �E�B���h�E����x���W
		CW_USEDEFAULT,				// �E�B���h�E����y���W
		WINDOW_W,					// �E�B���h�E��
		WINDOW_H,					// �E�B���h�E����
		nullptr,					// �e�E�B���h�E�̃n���h��
		nullptr,					// ���j���[�̃n���h��
		wcex.hInstance,				// �E�B���h�E���쐬���郂�W���[���̃n���h��
		nullptr						// WM_CREATE��LPARAM�ɓn�������l
	);
}
// D3D������
HRESULT InitDevice()
{
	HRESULT hr = S_OK;					// ����������ɍs��ꂽ�����f����ׂɗp��

	DXGI_SWAP_CHAIN_DESC desc;			// �X���b�v �`�F�[�����L�q
	ZeroMemory(&desc, sizeof(desc));	// ������
	desc.BufferCount = 1;				// �o�b�N�o�b�t�@�̐�
	desc.BufferDesc.Width = WINDOW_W;	// �o�b�N�o�b�t�@�̕�
	desc.BufferDesc.Height = WINDOW_H;	// �o�b�N�o�b�t�@�̍���
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// �t�H�[�}�b�g
	desc.BufferDesc.RefreshRate.Numerator = 60;			// ���t���b�V�����[�g�i���q�j
	desc.BufferDesc.RefreshRate.Denominator = 1;		// ���t���b�V�����[�g�i����j
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;// �������`��
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// �X�P�[�����O
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// �o�b�N�o�b�t�@�̎g�p���@
	desc.OutputWindow = g_hWnd;		// �o�̓E�B���h�E�ւ̊֘A�t��
	desc.SampleDesc.Count = 1;		// �s�N�Z���P�ʂ̃}���`�T���v�����O�̐�
	desc.SampleDesc.Quality = 0;	// �}���`�T���v�����O�̃N�I���e�B
	desc.Windowed = TRUE;			// TRUE:�E�B���h�E���[�h FALES:�t���X�N���[�����[�h
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // �X���b�v �`�F�[���̓���̃I�v�V����

														 // �f�o�C�X�ƃX���b�v�`�F�C���̍쐬
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,					// �f�o�C�X�̍쐬���Ɏg�p����r�f�I�A�_�v�^�ւ̃|�C���^
		D3D_DRIVER_TYPE_HARDWARE,	// �쐬����f�o�C�X�̎�ނ�\�� D3D_DRIVER_TYPE 
		nullptr,					// �\�t�g�E�F�A���X�^���C�U�[����������DLL�̃n���h��
		0,							// �L���ɂ��郉���^�C�����C���[
		g_pFeatureLevels,			// �쐬�����݂�@�\���x���̏������w��
		1,							// pFeatureLevels�̗v�f��
		D3D11_SDK_VERSION,			// SDK�̃o�[�W����
		&desc,						// �X���b�v �`�F�[���̏������p�����[�^�[���i�[����X���b�v�`�F�C���̋L�q�ւ̃|�C���^
		&g_pSwapChain,				// �����_�����O�Ɏg�p����X���b�v�`�F�C����\��IDXGISwapChain�I�u�W�F�N�g�ւ̃|�C���^�̃A�h���X��Ԃ�
		&g_pd3dDevice,				// �쐬���ꂽ�f�o�C�X��\��ID3D11Device�I�u�W�F�N�g�ւ̃|�C���^�̃A�h���X��Ԃ�
		&g_FeatureLevelsSupported,	// ���̃f�o�C�X�ŃT�|�[�g����Ă���@�\���x���̔z��ɂ���ŏ��̗v�f��\��D3D_FEATURE_LEVEL�ւ̃|�C���^��Ԃ�
		&g_pImmediateContext);		// �f�o�C�X �R���e�L�X�g��\��ID3D11DeviceContext�I�u�W�F�N�g�ւ̃|�C���^�̃A�h���X��Ԃ�

	if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N


									// �o�b�N�o�b�t�@��`��^�[�Q�b�g�ɐݒ�
									// �X���b�v�`�F�C������ŏ��̃o�b�N�o�b�t�@���擾����
	ID3D11Texture2D* pBackBuffer;	// �o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�C�X(�o�b�t�@��2D�e�N�X�`���Ƃ��Ĉ���)
	hr = g_pSwapChain->GetBuffer(
		0,							// �o�b�N�o�b�t�@�̔ԍ�
		__uuidof(ID3D11Texture2D),	// �o�b�t�@�ɃA�N�Z�X����C���^�[�t�F�C�X
		(LPVOID*)&pBackBuffer		// �o�b�t�@���󂯎��ϐ�
	);

	if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

									// �o�b�N�o�b�t�@�̕`��^�[�Q�b�g�r���[�����
	hr = g_pd3dDevice->CreateRenderTargetView(
		pBackBuffer,			// �r���[�ŃA�N�Z�X���郊�\�[�X
		nullptr,				// �`��^�[�Q�b�g�r���[�̒�`
		&g_pRenderTargetView	// �`��^�[�Q�b�g�r���[���󂯎��ϐ�
	);

	if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

									// �[�x/�X�e���V���e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = WINDOW_W;					// ��
	descDepth.Height = WINDOW_H;				// ����
	descDepth.MipLevels = 1;					// �~�b�v�}�b�v ���x����
	descDepth.ArraySize = 1;					// �z��T�C�Y
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;	// �t�H�[�}�b�g(�[�x�̂�)
	descDepth.SampleDesc.Count = 1;				// �}���`�T���v�����O�̐ݒ�
	descDepth.SampleDesc.Quality = 0;			// �}���`�T���v�����O�̕i��
	descDepth.Usage = D3D11_USAGE_DEFAULT;		// �g�p���@ �f�t�H���g
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// �[�x/�X�e���V���Ƃ��Ďg�p
	descDepth.CPUAccessFlags = 0;				// CPU����A�N�Z�X���Ȃ�
	descDepth.MiscFlags = 0;					// ���̑��̐ݒ�Ȃ�
												// 2D�e�N�X�`���[�̔z����쐬
	hr = g_pd3dDevice->CreateTexture2D(
		&descDepth,			// �쐬����2D�e�N�X�`��
		nullptr,			// �T�u���\�[�X�̋L�q�̔z��ւ̃|�C���^
		&g_pDepthStencil	// �쐬�����e�N�X�`�����󂯎��
	);

	if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

									// �[�x/�X�e���V���r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;	// ���\�[�X �f�[�^�̃t�H�[�}�b�g
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;// ���\�[�X�̃^�C�v
	descDSV.Flags = 0;					// �e�N�X�`���[���ǂݎ���p���ǂ���
	descDSV.Texture2D.MipSlice = 0;		// 2D�e�N�X�`���[�̃T�u���\�[�X���w��
	hr = g_pd3dDevice->CreateDepthStencilView(
		g_pDepthStencil,	// �[�x/�X�e���V���r���[�����e�N�X�`��
		&descDSV,			// �[�x/�X�e���V���r���[�̐ݒ�
		&g_pDepthStencilView// �쐬�����r���[���󂯎��
	);

	if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

									// �`��^�[�Q�b�g�r���[���o�̓}�[�W���[�̕`��^�[�Q�b�g�Ƃ��Đݒ�
	g_pImmediateContext->OMSetRenderTargets(
		1,						// �`��^�[�Q�b�g�̐�
		&g_pRenderTargetView,	// �`��^�[�Q�b�g�r���[�̔z��
		g_pDepthStencilView     // �[�x/�X�e���V���r���[��ݒ肵�Ȃ�
	);

	// �r���[�|�[�g�̐ݒ�(�`��̈�)-1.0�`1.0�͈̔͂ō��ꂽ���[���h���W���X�N���[�����W�ɕϊ����邽�߂̏��
	g_ViewPort.TopLeftX = 0.0f;		// �r���[�|�[�g�̈�̍���X���W
	g_ViewPort.TopLeftY = 0.0f;		// �r���[�|�[�g�̈�̍���Y���W
	g_ViewPort.Width = WINDOW_W;	// �r���[�|�[�g�̈�̕�
	g_ViewPort.Height = WINDOW_H;	// �r���[�|�[�g�̈�̍���
	g_ViewPort.MinDepth = 0.0f;		// �r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
	g_ViewPort.MaxDepth = 1.0f;		// �r���[�|�[�g�̈�̐[�x�l�̍ő�l
									// ���X�^���C�U�Ƀr���[�|�[�g��ݒ�
	g_pImmediateContext->RSSetViewports(
		1,				// �ݒ肷��r���[�|�[�g�̐�
		&g_ViewPort		// �ݒ肷��D3D11_VIEWPORT�\���̂̔z��
	);

	// ���X�^���C�Y�X�e�[�g�I�u�W�F�N�g�̐ݒ�
	D3D11_RASTERIZER_DESC descRast = {
		D3D11_FILL_SOLID,	// �����_�����O���Ɏg�p����`�惂�[�h��ݒ�
		D3D11_CULL_NONE,	// �J�����O�̐ݒ�
		FALSE,				// �O�p�`���O����������������ݒ肷��
		0,					// �w��̃s�N�Z���ɉ��Z����[�x�l
		0.0f,				// �s�N�Z���̍ő�[�x�o�C�A�X
		0.0f,				// �s�N�Z���̌X�΂Ɋ�Â��ăo�C�A�X��������
		FALSE,				// �����Ɋ�Â��ăN���b�s���O��L���ɂ���
		FALSE,				// �V�U�[��`�J�����O��L���ɂ���
		FALSE,				// �}���`�T���v�����O�̃A���`�G�C���A�V���O��L���ɂ���
		FALSE				// ���̃A���`�G�C���A�V���O��L���ɂ���
	};
	// ���X�^���C�Y�X�e�[�g�I�u�W�F�N�g�̍쐬
	g_pd3dDevice->CreateRasterizerState(
		&descRast,					// ���X�^���C�U�X�e�[�g�̋L�q�ւ̃|�C���^
		&g_pRasterizerState			// �쐬����郉�X�^���C�U�X�e�[�g�I�u�W�F�N�g�ւ̃|�C���^�̃A�h���X
	);
	//���X�^���C�U�[���R���e�L�X�g�ɐݒ�
	g_pImmediateContext->RSSetState(g_pRasterizerState);

	// �u�����f�B���O�X�e�[�g�̍쐬
	D3D11_BLEND_DESC descBlend;
	ZeroMemory(&descBlend, sizeof(descBlend));
	descBlend.AlphaToCoverageEnable = FALSE;		// �s�N�Z���������_�[�^�[�Q�b�g�ɐݒ肷��Ƃ��ɁA�A���t�@�g�D�J�o���b�W���}���`�T���v�����O�e�N�j�b�N�Ƃ��Ďg�p���邩�ǂ���������
	descBlend.IndependentBlendEnable = FALSE;		// ���������̃����_�[�^�[�Q�b�g�œƗ������u�����f�B���O��L���ɂ��邩�ǂ���
	descBlend.RenderTarget[0].BlendEnable = TRUE;	// �u�����f�B���O��L���ɂ���
	descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		// �ŏ���RGB�f�[�^�\�[�X���w��
	descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// 2�Ԗڂ�RGB�f�[�^�\�[�X���w��
	descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;			// RGB�f�[�^�\�[�X�̑g�������@���`
	descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;		// �ŏ��̃A���t�@�f�[�^�\�[�X���w��
	descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;	// 2�Ԗڂ̃A���t�@�f�[�^�\�[�X���w��
	descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;	// �A���t�@�f�[�^�\�[�X�̑g�������@���`
	descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // �������݃}�X�N
	g_pd3dDevice->CreateBlendState(
		&descBlend,		// �u�����f�B���O�X�e�[�g�̋L�q�ւ̃|�C���^
		&g_pBlendState	// �쐬�����I�u�W�F�N�g�ւ̃|�C���^�̃A�h���X
	);
	// �u�����f�B���O�X�e�[�g�̐ݒ�
	float blendFactor[4] = {
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ZERO
	};
	// �u�����f�B���O�X�e�[�g�̐ݒ�
	g_pImmediateContext->OMSetBlendState(
		g_pBlendState,	// �u�����f�B���O�X�e�[�g�ւ̃|�C���^
		blendFactor,	// �u�����f�B���O�W���̔z��
		0xffffffff		// �T���v���}�X�N
	);

	// ���W�ϊ��s��ɂ��Ă̋L�q
	D3D11_BUFFER_DESC descCBuffer;
	// ������
	ZeroMemory(&descCBuffer, sizeof(descCBuffer));
	// �o�b�t�@�őz�肳��Ă���ǂݍ��݋y�я������݂̕��@������
	descCBuffer.Usage = D3D11_USAGE_DEFAULT;
	// �o�b�t�@�̃T�C�Y
	descCBuffer.ByteWidth = sizeof(ConstantBuffer);
	// �o�b�t�@���ǂ̂悤�ɃO���t�B�b�N�X�p�C�v���C���Ƀo�C���h���邩������
	descCBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// CPU�̃A�N�Z�X�t���O
	descCBuffer.CPUAccessFlags = 0;

	// �o�b�t�@�̍쐬
	hr = g_pd3dDevice->CreateBuffer(
		// �o�b�t�@�̋L�q�ւ̃|�C���^
		&descCBuffer,
		// �������f�[�^�ւ̃|�C���^
		nullptr,
		// �쐬�����o�b�t�@�ւ̃|�C���^�̃A�h���X
		&g_pConstantBuffer
	);
	if (FAILED(hr))	return E_FAIL;	// �G���[�`�F�b�N

									// �P�ʍs��̍쐬
	g_World = XMMatrixIdentity();

	// �J�����̈ʒu
	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	// �J�����̏œ_
	XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	// ���݂̃��[���h���W�ɂ����������̒�`
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	// �J�����̈ʒu�A������A����яœ_���g�p���āA������W�n�̃r���[�s����쐬
	g_View = XMMatrixLookAtLH(eye, focus, up);
	// ����p�̐ݒ�
	float    fov = XMConvertToRadians(45.0f);
	// �A�X�y�N�g��̐ݒ�
	float    aspect = g_ViewPort.Width / g_ViewPort.Height;
	// �j�A�i������͈͂̑O���j�̐ݒ�
	float    nearZ = 0.1f;
	// �t�@�[�i������͈͂̌���j�̐ݒ�
	float    farZ = 1000.0f;
	// ����Ɋ�Â��āA������W�n�̃p�[�X�y�N�e�B�u�ˉe�s����쐬
	g_Proj = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);


	return S_OK;
}
// �G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	InitWindow(hInstance);			// �E�B���h�E����
	ShowWindow(g_hWnd, nCmdShow);	// �E�B���h�E�\��
	UpdateWindow(g_hWnd);			// �E�B���h�E�X�V

	if (FAILED(InitDevice()))		// �f�o�C�X�ƃX���b�v�`�F�C���̐���
	{
		return 0;
	}


	MSG hMsg = {};
	ZeroMemory(&hMsg, sizeof(hMsg));// ������
	while (hMsg.message != WM_QUIT)	// �I����������Ă��Ȃ��ԃ��[�v����
	{
		if (PeekMessage(&hMsg, nullptr, 0, 0, PM_REMOVE))// ���b�Z�[�W���擾����B�o���Ȃ��Ă��J��Ԃ��������s��
		{
			TranslateMessage(&hMsg);// �L�[�{�[�h���b�Z�[�W���當�����b�Z�[�W�𐶐�����
			DispatchMessage(&hMsg);	// �E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W��n��
		}
		else
		{
			// �`�揈��
			Render();
		}
	}

	// ���
	CleanupDevice();

	return 0;
}

// �`��
void Render()
{
	// �w�i�̐F��ݒ� �����_�[�^�[�Q�b�g�N���A
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, DirectX::Colors::Black);
	// �[�x/�X�e���V���o�b�t�@�N���A
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ConstantBuffer cb;
	// ���[���h���W�s���]�u
	cb.world = XMMatrixTranspose(g_World);
	// �r���[���W�s���]�u
	cb.view = XMMatrixTranspose(g_View);
	// �v���W�F�N�V�������W�s���]�u
	cb.projection = XMMatrixTranspose(g_Proj);
	// �f�[�^���R�s�[����g_pConstantBuffer�̓��e������������i�X�V����j
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer,0,nullptr,&cb,0,0);


	// �����_�����O���ꂽ�C���[�W����ʂɕ`�悷��
	g_pSwapChain->Present(0, 0);

}

// ���
void CleanupDevice()
{
	// �X�e�[�g�̃N���A
	if (g_pImmediateContext)	g_pImmediateContext->ClearState();

	// �C���^�[�t�F�C�X�̉��
	if (g_pDepthStencilView)	g_pDepthStencilView->Release();
	if (g_pDepthStencil)		g_pDepthStencil->Release();
	if (g_pRenderTargetView)	g_pRenderTargetView->Release();
	if (g_pSwapChain)			g_pSwapChain->Release();
	if (g_pImmediateContext)	g_pImmediateContext->Release();
	if (g_pd3dDevice)			g_pd3dDevice->Release();
	if (g_pVertexShader)		g_pVertexShader->Release();
	if (g_pPixelShader)			g_pPixelShader->Release();
	if (g_pVertexLayout)		g_pVertexLayout->Release();
	if (g_pVertexBuffer)		g_pVertexBuffer->Release();
	if (g_pBlendState)			g_pBlendState->Release();
	if (g_pRasterizerState)		g_pRasterizerState->Release();
	if (g_pConstantBuffer)		g_pConstantBuffer->Release();
}

