// ���_�V�F�[�_����󂯎��l
struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};

float4 main(PS_IN input) : SV_TARGET
{
	return input.col;
}