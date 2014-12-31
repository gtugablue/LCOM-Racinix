#ifndef __VBE_H
#define __VBE_H

#include <stdint.h>
#include <minix/syslib.h>
#include <sys/types.h>
#include "lmlib.h"

#define VBE_INTERRUPT_VECTOR									0x10
#define VBE_FUNCTION											0x4F

// VBE Return Status
#define VBE_FUNCTION_SUPPORTED									0x4F
#define VBE_FUNCTION_CALL_SUCCESSFUL							0x0
#define VBE_FUNCTION_CALL_FAILED								0x1
#define VBE_FUNCTION_NOT_SUPPORTED_IN_CURR_HW_CONFIG			0x2
#define VBE_FUNCTION_CALL_INVALID_IN_CURR_VIDEO_MODE			0x3

// VBE Mode Numbers
#define VBE_MODE_NUMBER_LINEAR_FLAT_FRAME_BUFFER_BIT			14
#define VBE_MODE_NUMBER_PRESERVE_DISPLAY_MEMORY_BIT				15
#define VBE_MODE_GRAPHICS_640_400_256							0x100
#define VBE_MODE_GRAPHICS_640_480_256							0x101
#define VBE_MODE_GRAPHICS_800_600_16							0x102
#define VBE_MODE_GRAPHICS_800_600_256							0x103
#define VBE_MODE_GRAPHICS_1024_768_16							0x104
#define VBE_MODE_GRAPHICS_1024_786_256							0x105
#define VBE_MODE_GRAPHICS_1280_1024_16							0x106
#define VBE_MODE_GRAPHICS_1280_1024_256							0x107
#define VBE_MODE_TEXT_80_60										0x108
#define VBE_MODE_TEXT_132_25									0x109
#define VBE_MODE_TEXT_132_43									0x10A
#define VBE_MODE_TEXT_132_50									0x10B
#define VBE_MODE_TEXT_132_60									0x10C
#define VBE_MODE_GRAPHICS_320_200_32K							0x10D
#define VBE_MODE_GRAPHICS_320_200_64K							0x10E
#define VBE_MODE_GRAPHICS_320_200_16_8M							0x10F
#define VBE_MODE_GRAPHICS_640_480_32K							0x110
#define VBE_MODE_GRAPHICS_640_480_64K							0x111
#define VBE_MODE_GRAPHICS_640_480_16_8M							0x112
#define VBE_MODE_GRAPHICS_800_600_32K							0x113
#define VBE_MODE_GRAPHICS_800_600_64K							0x114
#define VBE_MODE_GRAPHICS_800_600_16_8M							0x115
#define VBE_MODE_GRAPHICS_1024_768_32K							0x116
#define VBE_MODE_GRAPHICS_1024_768_64K							0x117
#define VBE_MODE_GRAPHICS_1024_768_16_8M						0x118
#define VBE_MODE_GRAPHICS_1280_1024_32K							0x119
#define VBE_MODE_GRAPHICS_1280_1024_64K							0x11A
#define VBE_MODE_GRAPHICS_1280_1024_16_8M						0x11B
#define VBE_MODE_SPECIAL_PRESERVE_CURRENT_MEMORY				0x81FF

// VBE Functions
#define VBE_RETURN_VBE_CONTROLLER_INFO							0x0
#define VBE_RETURN_VBE_MODE_INFO								0x1
#define VBE_SET_VBE_MODE										0x2
#define VBE_RETURN_CURRENT_VBE_MODE								0x3
#define VBE_SAVE_RESTORE_STATE									0x4
#define VBE_DISPLAY_WINDOW_CONTROL								0x5
#define VBE_SET_GET_LOGICAL_SCAN_LINE_LENGTH					0x6
#define VBE_SET_GET_DISPLAY_START								0x7
#define VBE_SET_GET_DAC_PALETTE_FORMAT							0x8
#define VBE_SET_GET_PALETTE_DATA								0x9
#define VBE_RETURN_VBE_PROTECTED_MODE_INTERFACE					0xA

#define VBE_VIDEO_MODE_PTR_TERMINATE							-1

#define VBE_CONTROLLER_CAPABILITIES_DAC_SWITCHABLE_WIDTH_BIT	0
#define VBE_CONTROLLER_CAPABILITIES_NOT_VGA_BIT					1
#define VBE_CONTROLLER_CAPABILITIES_RAMDAC_USE_BLANK_BIT		2

/** @defgroup vbe vbe
 * @{
 *
 * Functions related to the VBE standard
 */

/** @name VBE Mode Info Block */
/**@{
 *
 * Packed VBE Mode Info Block 
 */ 
 
typedef struct {
  /*  Mandatory information for all VBE revisions */
  uint16_t ModeAttributes; 	/**< @brief mode attributes */
  uint8_t WinAAttributes; 		/**< @brief window A attributes */
  uint8_t WinBAttributes; 		/**< @brief window B attributes */
  uint16_t WinGranularity; 	/**< @brief window granularity */
  uint16_t WinSize;		/**< @brief window size */
  uint16_t WinASegment;		/**< @brief window A start segment */
  uint16_t WinBSegment;		/**< @brief window B start segment */
  phys_bytes WinFuncPtr;	/**< @brief real mode/far pointer to window function */
  uint16_t BytesPerScanLine; 	/**< @brief bytes per scan line */

  /* Mandatory information for VBE 1.2 and above */

  uint16_t XResolution;      	/**< @brief horizontal resolution in pixels/characters */
  uint16_t YResolution;      	/**< @brief vertical resolution in pixels/characters */
  uint8_t XCharSize; 		/**< @brief character cell width in pixels */
  uint8_t YCharSize; 		/**< @brief character cell height in pixels */
  uint8_t NumberOfPlanes; 		/**< @brief number of memory planes */
  uint8_t BitsPerPixel; 		/**< @brief bits per pixel */
  uint8_t NumberOfBanks;		/**< @brief number of banks */
  uint8_t MemoryModel;		/**< @brief memory model type */
  uint8_t BankSize;		/**< @brief bank size in KB */
  uint8_t NumberOfImagePages;	/**< @brief number of images */
  uint8_t Reserved1;		/**< @brief reserved for page function */

  /* Direct Color fields (required for direct/6 and YUV/7 memory models) */
  
  uint8_t RedMaskSize;		/* size of direct color red mask in bits */
  uint8_t RedFieldPosition;	/* bit position of lsb of red mask */
  uint8_t GreenMaskSize;		/* size of direct color green mask in bits */
  uint8_t GreenFieldPosition;	/* bit position of lsb of green mask */
  uint8_t BlueMaskSize; 		/* size of direct color blue mask in bits */
  uint8_t BlueFieldPosition;	/* bit position of lsb of blue mask */
  uint8_t RsvdMaskSize;		/* size of direct color reserved mask in bits */
  uint8_t RsvdFieldPosition;	/* bit position of lsb of reserved mask */
  uint8_t DirectColorModeInfo;	/* direct color mode attributes */

  /* Mandatory information for VBE 2.0 and above */
  phys_bytes PhysBasePtr;       /**< @brief physical address for flat memory frame buffer */
  uint8_t Reserved2[4]; 		/**< @brief Reserved - always set to 0 */
  uint8_t Reserved3[2]; 		/**< @brief Reserved - always set to 0 */

  /* Mandatory information for VBE 3.0 and above */
  uint16_t LinBytesPerScanLine;    /* bytes per scan line for linear modes */
  uint8_t BnkNumberOfImagePages; 	/* number of images for banked modes */
  uint8_t LinNumberOfImagePages; 	/* number of images for linear modes */
  uint8_t LinRedMaskSize; 	        /* size of direct color red mask (linear modes) */
  uint8_t LinRedFieldPosition; 	/* bit position of lsb of red mask (linear modes) */
  uint8_t LinGreenMaskSize; 	/* size of direct color green mask (linear modes) */
  uint8_t LinGreenFieldPosition; /* bit position of lsb of green mask (linear  modes) */
  uint8_t LinBlueMaskSize; 	/* size of direct color blue mask (linear modes) */
  uint8_t LinBlueFieldPosition; 	/* bit position of lsb of blue mask (linear modes ) */
  uint8_t LinRsvdMaskSize; 	/* size of direct color reserved mask (linear modes) */
  uint8_t LinRsvdFieldPosition;	 /* bit position of lsb of reserved mask (linear modes) */
  uint32_t MaxPixelClock; 	         /* maximum pixel clock (in Hz) for graphics mode */
  uint8_t Reserved4[190]; 		 /* remainder of ModeInfoBlock */
} __attribute__((packed)) vbe_mode_info_t;

/** @} end of vbe_mode_info_t*/

/** @name VBE Mode Info Block */
/**@{
 *
 * Packed VBE Mode Info Block
 */

typedef struct {
	char VbeSignature[4];	/* VBE signature */
	uint16_t VbeVersion;	/* VBE Version */
	phys_bytes OemStringPtr;	/* pointer to OEM String */
	uint32_t Capabilities;	/* capabilities of graphics controller */
	phys_bytes VideoModePtr;	/* pointer to VideoModeList */
	uint16_t TotalMemory;	/* number of 64kb memory blocks */

	/* Added for VBE 2.0 */
	uint16_t OemSoftwareRev;	/* VBE implementation Software revision */
	phys_bytes OemVendorNamePtr;	/* pointer to Vendor Name String */
	phys_bytes OemProductNamePtr;	/* pointer to Product Name String */
	phys_bytes OemProductRevPtr;	/* pointer to Product Revision String */
	uint8_t Reserved[222];	/* reserved for VBE implementation scratch area */
	uint8_t OemData[256];	/* Data Area for OEM Strings */
} __attribute__((packed)) vbe_info_block_t;

/** @} end of vbe_info_block_t*/

/**
 * @brief Returns information on the input VBE mode, including screen dimensions, color depth and VRAM physical address
 * 
 * Initializes unpacked vbe_mode__info_t structure passed as an address with
 *  the information of the input mode, by calling VBE function 0x01
 *  Return VBE Mode Information and unpacking the ModeInfoBlock struct
 *  returned by that function.
 * 
 * @param mode mode whose information should be returned
 * @param vmi_p address of vbe_mode_info_t structure to be initialized
 * @return 0 on success, non-zero otherwise
 */
int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p);

/**
 * @brief
 *
 * @param vib_p
 * @param video_modes
 * @param num_video_modes
 *
 * @return
 */
int vbe_get_info_block(vbe_info_block_t *vib_p, uint16_t **video_modes, unsigned *num_video_modes);

 /** @} end of vbe */

#endif /* __VBE_H */
