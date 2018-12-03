/*
* This file is part of the HdFury Vertex API/DLL control library.

* Vertex and HDFURY are trademark of LegendSky Tech Co Ltd
*
* This program is free to redistribute and/or modify for the purpose of
* remote controlling HDFURY Vertex device.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*
* For more information contact:
* LegendSky Tech Co Ltd, Peng Yung Ge Jingli Mansion,
* No.48 Beili Road South, Shenzhen, PRC
* contact@hdfury.com
* www.hdfury.com
*
*/

#if !defined(_DEFINE_H_INCLUDE_)
#define _DEFINE_H_INCLUDE_

typedef enum {
	STATE_OFF,
	STATE_ON
}VERTEX_STATE;

typedef enum {
	INPORT_BOT = 1,
	INPORT_TOP,
	INPORT_THRU,
	INPORT_SWAP
} VERTEX_INPORT;

typedef enum {
	SLIDESW_MIDDLE = 1,
	SLIDESW_LEFT,
	SLIDESW_RIGHT
} VERTEX_SLIDESW;

typedef enum {
	HDCP_AUTO = 1,
	HDCP_14,
	HDCP_22
} VERTEX_HDCP;

typedef enum  {
	AUDIO_STEREO = 1,
	AUDIO_51,
	AUDIO_71,
	AUDIO_NATIVE
}VERTEX_AUDIOFLAGS;

typedef enum {
	EDID_AUTOMIX = 1,
	EDID_FIXED,
	EDID_CUSTOM,
	EDID_COPYTOP,
	EDID_COPYBOT
} VERTEX_EDIDMODECUSTOM;

typedef enum {
	CECLA_VIDEO,
	CECLA_AUDIO
} VERTEX_CECLA;

typedef enum {
	HDR10_OFF_HLG_OFF,
	HDR10_ON_HLG_OFF,
	HDR10_OFF_HLG_ON,
	HDR10_ON_HLG_ON
}VERTEX_HDR10;

typedef enum {
	RESET_EDID = 1,
	RESET_SETTINGS,
	RESET_EDID_SETTINGS
}VERTEX_RESETLEVEL;

typedef enum {
	BAUDRATE_9600,
	BAUDRATE_19200,
	BAUDRATE_57600,
	BAUDRATE_115200
}VERTEX_BAUDRATE;

typedef enum {
	SCALE_NONE = 0,
	SCALE_AUTO = 1,
	SCALE_CUSTOM = 2,
} VERTEX_SCALE;

typedef enum {
	EDIDALGO_NORMAL = 0,
	EDIDALGO_MAXAUDIO,
	EDIDALGO_PRIOBOT,
	EDIDALGO_PRIOTOP,
	EDIDALGO_MAXALL
} VERTEX_EDIDALGO;

typedef enum {
	PRESET4K30_AUTOSENSE,
	PRESET4K30_4K30_444_8,
	PRESET4K30_4K30_422_12,
	PRESET4K30_1080P30_444_8_10_12,
} VERTEX_AUTOPRESET4K30;

typedef enum {
	PRESET4K60_AUTOSENSE,
	PRESET4K60_4K60_420_8,
	PRESET4K60_1080P60_444_8_10_12,
	PRESET4K60_1080P60_444_8,
} VERTEX_AUTOPRESET4K60;

typedef enum {
	PRESET4K60HDR_AUTOSENSE,
	PRESET4K60HDR_4K60_420_8,
	PRESET4K60HDR_4K60_420_10_12,
	PRESET4K60HDR_1080P60_444_8_10_12,
	PRESET4K60HDR_1080P60_444_8,
} VERTEX_AUTOPRESET4K60HDR;

typedef enum {
	CS_FOLLOW,
	CS_RGB_FULL,
	CS_RGB_2020,
	CS_YUV444_709,
	CS_YUV444_2020,
	CS_YUV422_709,
	CS_YUV422_2020,
	CS_YUV420_709,
	CS_YUV420_2020
} VERTEX_SCALEPORT_CS;

typedef enum {
	DC_FOLLOW,
	DC_8,
	DC_10,
	DC_12,
	DC_16,
} VERTEX_SCALEPORT_DC;

typedef enum {
	RESO_1080,
	RESO_4K,
} VERTEX_SCALEPORT_RESO;

typedef enum {
	SCALE_OFF,
	SCALE_ON,
} VERTEX_SCALEPORT;

typedef enum {
	eInPort,
	eInputMode,
	eScaleCustom,
	eEdidModeSel,
	eEncSel,
	eSwState_edid,
	eSwState_hdcp,
	eSwState_scale,
	eAutoSw,
	eAutoSwPrio,			   
	eEdidCustom,
	eEdidAlgo,
	eEdidCustomTableTop,
	eEdidCustomTableBot,
	eAudioFlags,
	eBt2020,
	eYuv,
	e3DForce,
	eHdrForce,
	eDolbyVisionDb,	 		 
	eHdrCustom,
	eHdrDisable,
	eAviCustom,
	eAviDisable,
	eVsiCustom,
	eVsiDisable,
	ePreset4k30Mode,
	ePreset4k60Mode,
	ePreset4k60HdrMode,
	eAutoScalePresetNotUsed,			
	eLedEnable,
	eOledEnable,
	eOledReverse,
	eOledRotate,
	eOledFade,
	eOledMenuPage,
	eCec,
	eCecLa,
	eHtpcMode,
	eTempA,						
	eTempB,
	eTempC,
	eBaudRate,
	eArcHbr,
	eScaleModeSel,
	eQuickP,
	eAutoNoUpscale
} VERTEX_STATUS;

#endif // _DEFINE_H_INCLUDE_