#ifndef TSHP_HU_KEY_VALUE_H
#define TSHP_HU_KEY_VALUE_H

// �����URL�����Ĳ���(key=value)����
const unsigned int kMaxReqParam         = 50;
// ����Ĳ�����key����󳤶�
const unsigned int kMaxReqKeyLen        = 20;
// ����Ĳ�����value����󳤶�
const unsigned int kMaxReqValueLen      = 1024;
// http���������󳤶� (5 KB)
const unsigned int kMaxHttpReqLen       = 10 << 10;
// http�������url���� (2 KB) (<kMaxHttpReqLen)
const unsigned int kMaxHttpUrlLen       = 10 << 10;

#pragma pack(1)

struct Key2Value
{
    char key[kMaxReqKeyLen];
    char value[kMaxReqValueLen];
};

struct RequestParam
{
    Key2Value param[kMaxReqParam];
    int param_num;

	void Reset()
	{
		param_num = 0;
	}
};

#pragma pack()
#endif

