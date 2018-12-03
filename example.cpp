#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "vertex.h"

struct vic_mode
{
	const char* name;
	uint32_t width;
	uint32_t height;
	uint32_t vrefresh;
	uint32_t flags;
};

static const vic_mode vic_modes[] =
{
	{ "(na)", 0, 0, 0, 0 },
	{ "640x480", 640, 480, 60, 0 },
	{ "720x480", 720, 480, 60, 0 },
	{ "720x480", 720, 480, 60, 0 },
	{ "1280x720", 1280, 720, 60, 0 },
	{ "1920x1080i", 1920, 1080, 60, 0 },
	{ "720x480i", 720, 480, 60, 0 },
	{ "720x480i", 720, 480, 60, 0 },
	{ "720x240", 720, 240, 60, 0 },
	{ "720x240", 720, 240, 60, 0 },
	{ "2880x480i", 2880, 480, 60, 0 },
	{ "2880x480i", 2880, 480, 60, 0 },
	{ "2880x240", 2880, 240, 60, 0 },
	{ "2880x240", 2880, 240, 60, 0 },
	{ "1440x480", 1440, 480, 60, 0 },
	{ "1440x480", 1440, 480, 60, 0 },
	{ "1920x1080", 1920, 1080, 60, 0 },
	{ "720x576", 720, 576, 50, 0 },
	{ "720x576", 720, 576, 50, 0 },
	{ "1280x720", 1280, 720, 50, 0 },
	{ "1920x1080i", 1920, 1080, 50, 0 },
	{ "720x576i", 720, 576, 50, 0 },
	{ "720x576i", 720, 576, 50, 0 },
	{ "720x288", 720, 288, 50, 0 },
	{ "720x288", 720, 288, 50, 0 },
	{ "2880x576i", 2880, 576, 50, 0 },
	{ "2880x576i", 2880, 576, 50, 0 },
	{ "2880x288", 2880, 288, 50, 0 },
	{ "2880x288", 2880, 288, 50, 0 },
	{ "1440x576", 1440, 576, 50, 0 },
	{ "1440x576", 1440, 576, 50, 0 },
	{ "1920x1080", 1920, 1080, 50, 0 },
	{ "1920x1080", 1920, 1080, 24, 0 },
	{ "1920x1080", 1920, 1080, 25, 0 },
	{ "1920x1080", 1920, 1080, 30, 0 },
	{ "2880x480", 2880, 480, 60, 0 },
	{ "2880x480", 2880, 480, 60, 0 },
	{ "2880x576", 2880, 576, 50, 0 },
	{ "2880x576", 2880, 576, 50, 0 },
	{ "1920x1080i", 1920, 1080, 50, 0 },
	{ "1920x1080i", 1920, 1080, 100, 0 },
	{ "1280x720", 1280, 720, 100, 0 },
	{ "720x576", 720, 576, 100, 0 },
	{ "720x576", 720, 576, 100, 0 },
	{ "720x576i", 720, 576, 100, 0 },
	{ "720x576i", 720, 576, 100, 0 },
	{ "1920x1080i", 1920, 1080, 120, 0 },
	{ "1280x720", 1280, 720, 120, 0 },
	{ "720x480", 720, 480, 120, 0 },
	{ "720x480", 720, 480, 120, 0 },
	{ "720x480i", 720, 480, 120, 0 },
	{ "720x480i", 720, 480, 120, 0 },
	{ "720x576", 720, 576, 200, 0 },
	{ "720x576", 720, 576, 200, 0 },
	{ "720x576i", 720, 576, 200, 0 },
	{ "720x576i", 720, 576, 200, 0 },
	{ "720x480", 720, 480, 240, 0 },
	{ "720x480", 720, 480, 240, 0 },
	{ "720x480i", 720, 480, 240, 0 },
	{ "720x480i", 720, 480, 240, 0 },
	{ "1280x720", 1280, 720, 24, 0 },
	{ "1280x720", 1280, 720, 25, 0 },
	{ "1280x720", 1280, 720, 30, 0 },
	{ "1920x1080", 1920, 1080, 120, 0 },
	{ "1920x1080", 1920, 1080, 100, 0 },
	{ "1280x720", 1280, 720, 24, 0 },
	{ "1280x720", 1280, 720, 25, 0 },
	{ "1280x720", 1280, 720, 30, 0 },
	{ "1280x720", 1280, 720, 50, 0 },
	{ "1280x720", 1280, 720, 60, 0 },
	{ "1280x720", 1280, 720, 100, 0 },
	{ "1280x720", 1280, 720, 120, 0 },
	{ "1920x1080", 1920, 1080, 24, 0 },
	{ "1920x1080", 1920, 1080, 25, 0 },
	{ "1920x1080", 1920, 1080, 30, 0 },
	{ "1920x1080", 1920, 1080, 50, 0 },
	{ "1920x1080", 1920, 1080, 60, 0 },
	{ "1920x1080", 1920, 1080, 100, 0 },
	{ "1920x1080", 1920, 1080, 120, 0 },
	{ "1680x720", 1680, 720, 24, 0 },
	{ "1680x720", 1680, 720, 25, 0 },
	{ "1680x720", 1680, 720, 30, 0 },
	{ "1680x720", 1680, 720, 50, 0 },
	{ "1680x720", 1680, 720, 60, 0 },
	{ "1680x720", 1680, 720, 100, 0 },
	{ "1680x720", 1680, 720, 120, 0 },
	{ "2560x1080", 2560, 1080, 24, 0 },
	{ "2560x1080", 2560, 1080, 25, 0 },
	{ "2560x1080", 2560, 1080, 30, 0 },
	{ "2560x1080", 2560, 1080, 50, 0 },
	{ "2560x1080", 2560, 1080, 60, 0 },
	{ "2560x1080", 2560, 1080, 100, 0 },
	{ "2560x1080", 2560, 1080, 120, 0 },
	{ "3840x2160", 3840, 2160, 24, 0 },
	{ "3840x2160", 3840, 2160, 25, 0 },
	{ "3840x2160", 3840, 2160, 30, 0 },
	{ "3840x2160", 3840, 2160, 50, 0 },
	{ "3840x2160", 3840, 2160, 60, 0 },
	{ "4096x2160", 4096, 2160, 24, 0 },
	{ "4096x2160", 4096, 2160, 25, 0 },
	{ "4096x2160", 4096, 2160, 30, 0 },
	{ "4096x2160", 4096, 2160, 50, 0 },
	{ "4096x2160", 4096, 2160, 60, 0 },
	{ "3840x2160", 3840, 2160, 24, 0 },
	{ "3840x2160", 3840, 2160, 25, 0 },
	{ "3840x2160", 3840, 2160, 30, 0 },
	{ "3840x2160", 3840, 2160, 50, 0 },
	{ "3840x2160", 3840, 2160, 60, 0 },
};


static const char* str_vertex_status[] =
{
	"eInPort",
	"eInputMode",
	"eScaleCustom",
	"eEdidModeSel",
	"eEncSel",
	"eSwState_edid",
	"eSwState_hdcp",
	"eSwState_scale",
	"eAutoSw",
	"eAutoSwPrio",			   
	"eEdidCustom",
	"eEdidAlgo",
	"eEdidCustomTableTop",
	"eEdidCustomTableBot",
	"eAudioFlags",
	"eBt2020",
	"eYuv",
	"e3DForce",
	"eHdrForce",
	"eDolbyVisionDb",	 		 
	"eHdrCustom",
	"eHdrDisable",
	"eAviCustom",
	"eAviDisable",
	"eVsiCustom",
	"eVsiDisable",
	"ePreset4k30Mode",
	"ePreset4k60Mode",
	"ePreset4k60HdrMode",
	"eAutoScalePresetNotUsed",			
	"eLedEnable",
	"eOledEnable",
	"eOledReverse",
	"eOledRotate",
	"eOledFade",
	"eOledMenuPage",
	"eCec",
	"eCecLa",
	"eHtpcMode",
	"eTempA",						
	"eTempB",
	"eTempC",
	"eBaudRate",
	"eArcHbr",
	"eScaleModeSel",
	"eQuickP",
	"eAutoNoUpscale",
};

int main(int argc, char** args)
{
	Vertex* pv = Vertex::Create();

	if (pv)
	{
		uint8_t data[256];

		char fw[32];
		if (pv->GetFwVersion(fw, sizeof(fw)))
			printf("Vertex firmware version: %s\n", fw);

		memset(data, 0, sizeof(data));
		size_t count = pv->GetStateVector(data, sizeof(data));
		if (count > 0)
		{
			printf("Vertex state vector:\n");
			for (size_t i = 0; i < sizeof(str_vertex_status) / sizeof(char*); i++)
			{
				printf("%s = %d\n", str_vertex_status[i], data[i]);
			}
		}

		count = pv->ReadParamData(Vertex::PARAM_INFOFRAME_AVI, data, sizeof(data));
		if (count > 0)
		{
			printf("AVI IF: ");
			for (size_t i = 0; i < count; i++)
			{
				printf("%02x ", data[i]);
			}
			printf("\n");
			// parse AVI IF based on HDMI spec
			// useful enums found here: https://github.com/torvalds/linux/blob/master/include/linux/hdmi.h
			printf("   Colorspace     : %d\n", (data[4] >> 5) & 3);
			printf("   Colorimetry    : %d\n", (data[5] >> 6) & 3);
			printf("   ExtColorimetry : %d\n", (data[6] >> 4) & 7);
			printf("   VIC            : %d [%s]\n", data[7], vic_modes[data[7]].name);
		}

		count = pv->ReadParamData(Vertex::PARAM_INFOFRAME_HDR, data, sizeof(data));
		if (count > 0)
		{
			printf("HDR IF: ");
			for (size_t i = 0; i < count; i++)
			{
				printf("%02x ", data[i]);
			}
			printf("\n");
		}

		count = pv->ReadParamData(Vertex::PARAM_STATUS_RX0, data, sizeof(data));
		if (count > 0) printf("RX0: %s\n", data);
		count = pv->ReadParamData(Vertex::PARAM_STATUS_RX1, data, sizeof(data));
		if (count > 0) printf("RX1: %s\n", data);

		count = pv->ReadParamData(Vertex::PARAM_STATUS_TX0, data, sizeof(data));
		if (count > 0) printf("TX0: %s\n", data);
		count = pv->ReadParamData(Vertex::PARAM_STATUS_TX1, data, sizeof(data));
		if (count > 0) printf("TX1: %s\n", data);

		count = pv->ReadParamData(Vertex::PARAM_STATUS_TX0_SINK, data, sizeof(data));
		if (count > 0) printf("TX0_SINK: %s\n", data);
		count = pv->ReadParamData(Vertex::PARAM_STATUS_TX1_SINK, data, sizeof(data));
		if (count > 0) printf("TX1_SINK: %s\n", data);
	}
	else
		printf("failed to open vertex device\n");

	delete pv;
}

