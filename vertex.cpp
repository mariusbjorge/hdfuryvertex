#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <usb.h>
#include <assert.h>

#include "vertex.h"

#define VERTEX_USB_VID                      0x10c4
#define VERTEX_USB_PID                      0xEA61

#define TX_TIMEOUT		4000
#define RX_TIMEOUT  	 	1000
#define BUFFER_SIZE		4096

#ifdef DEBUG
#define LOGD(args...) printf (args)
#else
#define LOGD(format, ...)
#endif
#define LOGE(args...) printf (args)

struct VertexCtx
{
	usb_dev_handle* udev;
	int interface;
	int ep_out;
	int ep_in;
	size_t buffer_offset;
	char buffer[BUFFER_SIZE];
};

static int vertex_init = 0;

static void init()
{
	if (vertex_init == 0)
	{
		usb_init();
		vertex_init = 1;
	}
}

static int _VertexRead(VertexCtx* ctx, int timeout)
{
	assert(ctx != NULL);

	size_t toread = BUFFER_SIZE - (int)ctx->buffer_offset;
	int r = usb_bulk_read(ctx->udev, ctx->ep_in, ctx->buffer + ctx->buffer_offset, toread, timeout);
	if (r > 0)
	{
		ctx->buffer_offset += (size_t)r;
	}
	return r;
}

static size_t _VertexCopyBuffer(VertexCtx* ctx, void* mem, size_t NumBytes)
{
	assert(ctx != NULL);
	assert(mem != NULL);

	if (ctx->buffer_offset < NumBytes)
		NumBytes = ctx->buffer_offset;
	else if (NumBytes > BUFFER_SIZE)
		NumBytes = BUFFER_SIZE;

	if (NumBytes > 0)
	{
		memcpy(mem, ctx->buffer, NumBytes);
		ctx->buffer_offset -= NumBytes;
		if (ctx->buffer_offset > 0)
			memmove(ctx->buffer, ctx->buffer + NumBytes, ctx->buffer_offset);
	}

	return NumBytes;
}

static VertexHandle VertexOpen()
{
	init();
	usb_find_busses();
	usb_find_devices();

	struct usb_bus* busses = usb_get_busses();
	struct usb_device* pdev = NULL;

	for (struct usb_bus* bus = busses; bus; bus = bus->next)
	{
		for (struct usb_device* dev = bus->devices; dev; dev = dev->next) 
		{
			if (dev->descriptor.idVendor == VERTEX_USB_VID && dev->descriptor.idProduct == VERTEX_USB_PID)
			{
				pdev = dev;
				break;
			}
		}
	}

	if (pdev == NULL)
	{
		return NULL;
	}

	VertexCtx* ph = (VertexCtx*)malloc(sizeof(VertexCtx));
	if (ph == NULL)
		return NULL;

	memset(ph, 0, sizeof(VertexCtx));

	ph->ep_out = -1;
	ph->ep_in = -1;

	// get bulk in/out endpoints
	for (int i = 0; i < pdev->config[0].interface[0].altsetting[0].bNumEndpoints; i++)
	{
		if (pdev->config[0].interface[0].altsetting[0].endpoint[i].bmAttributes == USB_ENDPOINT_TYPE_BULK)
		{
			if ((pdev->config[0].interface[0].altsetting[0].endpoint[i].bEndpointAddress&USB_ENDPOINT_DIR_MASK) != 0)
				ph->ep_in = pdev->config[0].interface[0].altsetting[0].endpoint[i].bEndpointAddress;
			else
				ph->ep_out = pdev->config[0].interface[0].altsetting[0].endpoint[i].bEndpointAddress;
		}
	}

	if (ph->ep_out==-1 || ph->ep_in==-1)
	{
		LOGE("Unable to identify BULK IN/OUT endpoints\n");
		free(ph);
		return NULL;
	}

	ph->udev = usb_open(pdev);
	if (ph->udev == NULL)
	{
		free(ph);
		return NULL;
	}

	// claim the interface
	ph->interface = pdev->config[0].interface[0].altsetting[0].bInterfaceNumber;
	if (usb_claim_interface(ph->udev, ph->interface))
	{
		LOGE("Unable to claim interface. Ensure device is not claimed by any kernel modules. Check permissions of /dev/bus/usb/...\n");
		usb_close(ph->udev);
		free(ph);
		return NULL;
	}

	usb_control_msg(ph->udev, 0x40, 0x00, 0xFFFF, 0, NULL, 0, TX_TIMEOUT);
	usb_resetep(ph->udev, ph->ep_in);
	usb_resetep(ph->udev, ph->ep_out);
	usb_clear_halt(ph->udev, ph->ep_in);
	usb_clear_halt(ph->udev, ph->ep_out);
	usb_control_msg(ph->udev, 0x40, 0x02, 0x0002, 0, NULL, 0, TX_TIMEOUT);
	//_VertexRead(ph, 100);

	return ph;
}

Vertex* Vertex::Create()
{
	return new Vertex(NULL);
}

Vertex::Vertex(VertexHandle h)
{
	m_h = h;
}

Vertex::~Vertex()
{
	Close();
}

bool Vertex::IsConnected()
{
	return !!(m_h != NULL);
}

bool Vertex::Open()
{
	if (m_h == NULL)
	{
		m_h = VertexOpen();
	}
	return !!(m_h != NULL);
}

void Vertex::Close()
{
	if (m_h)
	{
		VertexCtx* ctx = (VertexCtx*)m_h;
		usb_release_interface(ctx->udev, ctx->interface);
	  usb_close(ctx->udev);
	}
	free(m_h);
	m_h = NULL;
}

bool Vertex::CheckUSBError(int error)
{
	if (error >= 0)
		return true;

	switch (error)
	{
		case -ETIMEDOUT:
			return true;
		case -ENODEV:
		{
			LOGE("No device - closing USB...\n");
			Close();
			return false;
		}
		default:
		{
			LOGE("Error %d encountered - closing USB...\n", error);
			Close();
			return false;
		}
	}
}

size_t Vertex::Read(void* pMem, size_t NumBytes)
{
	if (!Open())
		return 0;
	
	VertexCtx* ctx = (VertexCtx*)m_h;
	if (ctx->buffer_offset < NumBytes)
	{
		int r = _VertexRead(ctx, RX_TIMEOUT);
		if (!CheckUSBError(r))
			return 0;
	}

	return _VertexCopyBuffer(ctx, pMem, NumBytes);
}

size_t Vertex::Write(const void* pMem, size_t NumBytes)
{
	if (!Open())
		return 0;

	VertexCtx* ctx = (VertexCtx*)m_h;

	int r = _VertexRead(ctx, 100);
	if (!CheckUSBError(r)) return 0;

	int written = usb_bulk_write(ctx->udev, ctx->ep_out, (const char*)pMem, NumBytes, TX_TIMEOUT);
	if (!CheckUSBError(written)) return 0;

	r = _VertexRead(ctx, 100);
	if (!CheckUSBError(r)) return 0;

	return (size_t)written;
}

struct CmdRead : Vertex::Request
{
	CmdRead(Vertex::TYPE_e command_type)
	{
		method = Vertex::METHOD_READ;
		response_size = 0;
		type = (uint8_t)command_type;
	}
};

struct CmdWrite : Vertex::Request
{
	CmdWrite(Vertex::TYPE_e command_type)
	{
		method = Vertex::METHOD_WRITE;
		response_size = 0;
		type = (uint8_t)command_type;
	}
};

struct CmdReadParam : CmdRead
{
	uint8_t param;
	CmdReadParam(Vertex::PARAM_e param) : CmdRead(Vertex::TYPE_PARAM)
	{
		this->param = (uint8_t)param;
	}
};

struct CmdWriteParam : CmdWrite
{
	uint8_t param;
	CmdWriteParam(Vertex::PARAM_e param) : CmdWrite(Vertex::TYPE_PARAM)
	{
		this->param = (uint8_t)param;
	}

	CmdWriteParam(uint8_t type, uint8_t param) : CmdWrite((Vertex::TYPE_e)type)
	{
		this->param = param;
	}
};

static bool _ValidateResponse(const Vertex::Request* request, const Vertex::Response* response, size_t response_size)
{
	if (response_size < sizeof(Vertex::Response))
		return false;

	if (response->type != request->type)
		return false;

	return true;
}

size_t Vertex::Query(const Request* pRequest, size_t RequestSize, uint8_t* pBuffer, size_t BufferSize)
{
	Write(pRequest, RequestSize);
	size_t read = Read(pBuffer, BufferSize);

	if (!_ValidateResponse(pRequest, (Response*)pBuffer, read))
		return 0;

	read -= sizeof(Response);
	memmove(pBuffer, pBuffer + 2, read);
	return read;
}

bool Vertex::GetFwVersion(char* Str, size_t MaxLen)
{
	CmdReadParam get_fw(PARAM_FW_VERSION);

	size_t r = Query(&get_fw, sizeof(get_fw), (uint8_t*)Str, MaxLen);

	if (r == 0)
		return false;

	Str[r] = '\0';
	return true;
}

size_t Vertex::ReadParamData(PARAM_e Param, uint8_t* pData, size_t DataMaxSize)
{
	CmdReadParam get(Param);
	return Query(&get, sizeof(get), pData, DataMaxSize);
}

int Vertex::ReadParam(PARAM_e Param)
{
	CmdReadParam get(Param);
	uint8_t buffer[32];
	size_t read = Query(&get, sizeof(get), buffer, sizeof(buffer));

	if (read > 0)
		return (int)buffer[0];
	else
		return -1;
}

bool Vertex::WriteParamData(PARAM_e Param, const uint8_t* pData, size_t DataSize)
{
	if (DataSize > 256)
		return false;

	uint8_t tmp[256 + sizeof(CmdWriteParam)];

	CmdWriteParam* pCmd = (CmdWriteParam*)tmp;
	*pCmd = CmdWriteParam(Param);

	memcpy(&pCmd[1], pData, DataSize);
	Write(tmp, sizeof(*pCmd) + DataSize);

	size_t read = Read(tmp, sizeof(tmp));
	// todo: parse response to determine success
	return !!(read != 0);
}

bool Vertex::WriteParam(PARAM_e Param, uint8_t Value)
{
	return WriteParamData(Param, &Value, 1);
}

size_t Vertex::GetStateVector(uint8_t* pState, size_t StateMaxSize)
{
	CmdRead read_state(TYPE_STATE_VECTOR);
	return Query(&read_state, sizeof(read_state), pState, StateMaxSize);
}

bool Vertex::CEC_Send(const uint8_t* pCec, size_t CecSize)
{
	if (CecSize >= 256) return false;

	size_t command_length = sizeof(CmdWrite);
	uint8_t tmp[256 + sizeof(CmdWrite)];
	CmdWrite* pCmd = (CmdWrite*)tmp;
	*pCmd = CmdWrite(TYPE_CEC);

	const char* cec_prefix = "cecif";
	const int cec_prefix_len = strlen(cec_prefix);
	memcpy(tmp + command_length, cec_prefix, cec_prefix_len);
	command_length += cec_prefix_len;

	tmp[command_length++] = (uint8_t)CecSize;
	memcpy(tmp + command_length, pCec, CecSize);
	command_length += CecSize;

	Write(tmp, command_length);

	size_t read = Read(tmp, sizeof(tmp));
	// todo: parse response to determine success
	return !!(read != 0);
}

bool Vertex::OSD_SetText(bool Enable, uint16_t x, uint16_t y, const char* Text)
{
	size_t command_length = sizeof(CmdWrite);
	uint8_t tmp[256];
	CmdWrite* pCmd = (CmdWrite*)tmp;
	*pCmd = CmdWrite(TYPE_OSD_TEXT);

	const char* osd_prefix = "osdtext ";
	const size_t osd_prefix_len = strlen(osd_prefix);
	const size_t osd_text_len = strlen(Text);
	memcpy(tmp + command_length, osd_prefix, osd_prefix_len);
	command_length += osd_prefix_len;

	tmp[command_length++] = (uint8_t)osd_text_len;
	tmp[command_length++] = Enable ? 1 : 0;
	memcpy(tmp + command_length, &x, sizeof(uint16_t));
	memcpy(tmp + command_length + 2, &y, sizeof(uint16_t));
	command_length += 4;
	memcpy(tmp + command_length, Text, osd_text_len);
	command_length += osd_text_len;

	Write(tmp, command_length);
	size_t read = Read(tmp, sizeof(tmp));
	// todo: parse response to determine success
	return !!(read != 0);
}

bool Vertex::WriteCustomEdid(uint32_t Index, uint8_t* pEdid)
{
	if (Index == 0 || Index > 10)
		return false;

	uint8_t tmp[32];
	CmdWrite* pCmd = (CmdWrite*)tmp;
	*pCmd = CmdWrite(TYPE_EDID);
	tmp[3] = (uint8_t)Index;

	uint8_t buffer[32];

	for (size_t offset = 0; offset < 256; offset += 8)
	{
		tmp[4] = (uint8_t)offset;
		memcpy(tmp + 5, pEdid + offset, 8);

		if (Query(pCmd, 13, buffer, sizeof(buffer)) == 0)
			return false;
	}

	*pCmd = CmdWrite(TYPE_EDID_SAVE);
	tmp[3] = (uint8_t)Index;

	if (Query(pCmd, 4, buffer, sizeof(buffer)) == 0)
		return false;

	return true;
}

bool Vertex::ReadEdid(PORT_e Port, uint8_t* pEdid, size_t MaxSize)
{
	if (MaxSize < 256)
		return false;

	static const TYPE_e PORT_lut[] =
	{
		TYPE_EDID_RX_BOTTOM,
		TYPE_EDID_RX_TOP,
		TYPE_EDID_TX_BOTTOM,
		TYPE_EDID_TX_TOP
	};

	uint8_t buffer[32];

	CmdWrite set_register(PORT_lut[Port]);

	if (Query(&set_register, sizeof(set_register), buffer, sizeof(buffer)) == 0)
		return false;

	for (size_t offset = 0; offset < 256; offset += 16)
	{
		CmdWriteParam read_offset(TYPE_EDID, offset);
		read_offset.response_size = 16;

		if (Query(&read_offset, sizeof(read_offset), buffer, sizeof(buffer)) != read_offset.response_size)
			return false;

		memcpy(pEdid + offset, buffer, read_offset.response_size);
	}

	return true;
}

bool Vertex::WriteInfoFrame(INFOFRAME_e InfoFrame, const uint8_t* pIF)
{
	static const TYPE_e TYPE_lut[] =
	{
		TYPE_UNKNOWN,
		TYPE_IF_AVI,
		TYPE_UNKNOWN,
		TYPE_IF_HDR,
		TYPE_UNKNOWN,
		TYPE_IF_VSI,
	};

	static const uint8_t HDMI_lut[] =
	{
		0x83,
		0x82,
		0x84,
		0x87,
		0, // ??
		0x81,
	};

	if (pIF[0] != HDMI_lut[InfoFrame])
		return false;

	uint8_t tmp[64];
	CmdWrite* pCmd = (CmdWrite*)tmp;
	*pCmd = CmdWrite(TYPE_lut[InfoFrame]);

	if (pCmd->type == TYPE_UNKNOWN)
		return false;

	memcpy(&pCmd[1], pIF, pIF[2] + 4);

	Write(tmp, sizeof(CmdWrite) + 4 + pIF[2]);
	size_t read = Read(tmp, sizeof(tmp));
	// todo: parse response to determine success
	return !!(read != 0);
}

bool Vertex::SetScalingParam(PORT_e Port, const ScalingParam& Param)
{
	if (Port != PORT_TX0 && Port != PORT_TX1) return false;

	static const uint8_t SCALEPORT_CS_lut[] = 
	{
		0x23, //CS_FOLLOW
		0x02, //CS_RGB_FULL
		0x06, //CS_RGB_2020
		0x12, //CS_YUV444_709
		0x18, //CS_YUV444_2020
		0x09, //CS_YUV422_709
		0x0F, //CS_YUV422_2020
		0x1B, //CS_YUV420_709
		0x21, //CS_YUV420_2020
	};

	static const uint8_t SCALEPORT_DC_lut[] = 
	{
		0x06,	//DC_FOLLOW
		0x01, //DC_8
		0x03, //DC_10
		0x04, //DC_12
		0x05, //DC_16
	};

	size_t command_length = sizeof(CmdWrite);
	uint8_t tmp[64];
	CmdWrite* pCmd = (CmdWrite*)tmp;
	*pCmd = CmdWrite(TYPE_SCALE_PARAM);

	const char* prefix = "param ";
	const size_t prefix_len = strlen(prefix);
	memcpy(tmp + command_length, prefix, prefix_len);
	command_length += prefix_len;

	tmp[command_length++] = (Port == PORT_TX0) ? 0x30 : 0x31;

	tmp[command_length++] = (uint8_t)Param.scale4k60.active;
	tmp[command_length++] = (uint8_t)Param.scale4k60.scale;
	tmp[command_length++] = (uint8_t)SCALEPORT_CS_lut[Param.scale4k60.cs];
	tmp[command_length++] = (uint8_t)SCALEPORT_DC_lut[Param.scale4k60.dc];

	tmp[command_length++] = (uint8_t)Param.scale4k30.active;
	tmp[command_length++] = (uint8_t)Param.scale4k30.scale;
	tmp[command_length++] = (uint8_t)SCALEPORT_CS_lut[Param.scale4k30.cs];
	tmp[command_length++] = (uint8_t)SCALEPORT_DC_lut[Param.scale4k30.dc];

	tmp[command_length++] = (uint8_t)Param.scale1080p60.active;
	tmp[command_length++] = (uint8_t)Param.scale1080p60.scale;
	tmp[command_length++] = (uint8_t)SCALEPORT_CS_lut[Param.scale1080p60.cs];
	tmp[command_length++] = (uint8_t)SCALEPORT_DC_lut[Param.scale1080p60.dc];

	tmp[command_length++] = (uint8_t)Param.scale1080p30.active;
	tmp[command_length++] = (uint8_t)Param.scale1080p30.scale;
	tmp[command_length++] = (uint8_t)SCALEPORT_CS_lut[Param.scale1080p30.cs];
	tmp[command_length++] = (uint8_t)SCALEPORT_DC_lut[Param.scale1080p30.dc];

	Write(tmp, command_length);
	size_t read = Read(tmp, sizeof(tmp));
	// todo: parse response to determine success
	return !!(read != 0);
}

