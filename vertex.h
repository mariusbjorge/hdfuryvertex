#ifndef __VERTEX_H
#define __VERTEX_H

#include <stdlib.h>
#include "vertexapi_enum.h"

typedef struct VertexCtx* VertexHandle;

class Vertex
{
public:

	enum PORT_e
	{
		PORT_RX0 = 0,
		PORT_RX1,
		PORT_TX0,
		PORT_TX1
	};

	enum METHOD_e
	{
		METHOD_READ = 0x11,
		METHOD_WRITE = 0x10,
	};

	enum TYPE_e
	{
		TYPE_UNKNOWN = 0,

		TYPE_EDID = 0xB0,
		TYPE_EDID_SAVE = 0xB1,

		TYPE_EDID_TX_BOTTOM = 0xB5,
		TYPE_EDID_TX_TOP = 0xB6,
		TYPE_EDID_RX_BOTTOM = 0xB7,
		TYPE_EDID_RX_TOP = 0xB8,

		TYPE_PARAM = 0xE0,
		TYPE_STATE_VECTOR = 0xE1,

		TYPE_IF_HDR = 0xE5,
		TYPE_IF_AVI = 0xE6,
		TYPE_IF_VSI = 0xE8,

		TYPE_SCALE_PARAM = 0xE7,

		TYPE_CEC = 0xEA,
		TYPE_OSD_PARAM = 0xEB,
		TYPE_OSD_TEXT = 0xEC,
	};

	enum INFOFRAME_e
	{
		INFOFRAME_SPD = 0,
		INFOFRAME_AVI,
		INFOFRAME_AUD,
		INFOFRAME_HDR,
		INFOFRAME_HVS,
		INFOFRAME_VSI,
	};

	enum PARAM_e
	{
		PARAM_FW_VERSION = 0x00,

		PARAM_STATUS_RX0, // 1
		PARAM_STATUS_RX1, // 2
		PARAM_STATUS_RX0_AUDIO, // 3
		PARAM_STATUS_RX1_AUDIO, // 4

		PARAM_STATUS_TX0, // 5
		PARAM_STATUS_TX1, // 6
		PARAM_STATUS_TX0_SINK, // 7
		PARAM_STATUS_TX1_SINK, // 8

		PARAM_INFOFRAME_SPD = 0x11,
		PARAM_INFOFRAME_AVI,
		PARAM_INFOFRAME_AUD,
		PARAM_INFOFRAME_HDR,
		PARAM_INFOFRAME_HVS,
		PARAM_INFOFRAME_VSI, // 0x16

		PARAM_SCALE = 0x1C,
		PARAM_AUTOSCALE_NO_UPSCALE = 0x44,

		PARAM_HTPCMODE = 0x3A,
		PARAM_HOTPLUG = 0x3E, // with value of 1

		PARAM_CEC = 0x38,
		PARAM_CECLA = 0x39,

		PARAM_HDR_CUSTOM = 0x28,
		PARAM_HDR_DISABLE = 0x29,
		PARAM_AVI_CUSTOM = 0x2A,
		PARAM_AVI_DISABLE = 0x2B,
		PARAM_VSI_CUSTOM = 0x2C,
		PARAM_VSI_DISABLE = 0x2D,

		PARAM_INPUT_PORT = 0x19,
		PARAM_HDCP_LEVEL = 0x1E,
		PARAM_AUTO_SWITCH = 0x1A,
		PARAM_AUTO_SWITCH_PRIORITY = 0x1B,

		PARAM_LOGO_ENABLE = 0x32,
		PARAM_OLED_ENABLE = 0x33,
		PARAM_OLED_REVERSE = 0x34,
		PARAM_OLED_ROTATE = 0x35,
		PARAM_OLED_FADEVALUE = 0x36,
		PARAM_OLED_MENUPAGE = 0x37,

		PARAM_EDID_MODE = 0x1D,
		PARAM_EDID_ALGO = 0x1F,
		PARAM_EDID_AUDIO_FLAGS = 0x22,
		PARAM_EDID_BT2020 = 0x23,
		PARAM_EDID_HDR = 0x26,
		PARAM_EDID_DOLBY_VISION = 0x27,
		PARAM_EDID_CUSTOM_TABLE_BOTTOM = 0x20,
		PARAM_EDID_CUSTOM_TABLE_TOP = 0x21,

		PARAM_PRESET4K30 = 0x2E,
		PARAM_PRESET4K60 = 0x2F,
		PARAM_PRESET4K60HDR = 0x30,

		PARAM_FACTORY_RESET = 0x3F,

		PARAM_BAUDRATE = 0x42,
		PARAM_ARC_HBR = 0x43,
	};

	struct Request
	{
		uint8_t method;
		uint8_t response_size; // ??
		uint8_t type;
	};

	struct Response
	{
		uint8_t type;
		uint8_t errorcode; // error code?
	};

	struct ScalingRule
	{
		bool active;
		VERTEX_SCALEPORT_RESO scale;
		VERTEX_SCALEPORT_CS cs;
		VERTEX_SCALEPORT_DC dc;

		ScalingRule()
		{
			active = false;
			scale = RESO_1080;
			cs = CS_FOLLOW;
			dc = DC_FOLLOW;
		}
	};

	struct ScalingParam
	{
		ScalingRule scale4k60;
		ScalingRule scale4k30;
		ScalingRule scale1080p60;
		ScalingRule scale1080p30;
	};

	static Vertex* Create();
	~Vertex();
	bool Open();
	void Close();

	bool IsConnected();

	bool GetFwVersion(char* Str, size_t MaxLen);
	size_t GetStateVector(uint8_t* pState, size_t StateMaxSize);

	bool CEC_Send(const uint8_t* pCec, size_t CecSize);
	bool OSD_SetText(bool Enable, uint16_t x, uint16_t y, const char* Text);

	bool SetScalingParam(PORT_e Port, const ScalingParam& Param);
	bool ReadEdid(PORT_e Port, uint8_t* pEdid, size_t MaxSize);
	bool WriteCustomEdid(uint32_t Index, uint8_t* pEdid);

	bool WriteInfoFrame(INFOFRAME_e InfoFrame, const uint8_t* pIF);

	size_t ReadParamData(PARAM_e Param, uint8_t* pData, size_t DataMaxSize);
	int ReadParam(PARAM_e Param);

	bool WriteParamData(PARAM_e Param, const uint8_t* pData, size_t DataSize);
	bool WriteParam(PARAM_e Param, uint8_t Value);

private:

	Vertex(VertexHandle h);

	size_t Query(const Request* pRequest, size_t RequestSize, uint8_t* pBuffer, size_t BufferSize);
	size_t Read(void* pMem, size_t NumBytes);
	size_t Write(const void* pMem, size_t NumBytes);
	bool CheckUSBError(int error);

	VertexHandle m_h;
};

#endif