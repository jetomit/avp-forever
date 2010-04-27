#ifndef _included_d3_func_h_
#define _included_d3_func_h_

#ifdef __cplusplus
	extern "C" {
#endif

#include <xtl.h>

typedef LPDIRECT3DTEXTURE8 RENDERTEXTURE;
#define D3DLOCK_DISCARD	0

#include "aw.h"
#include "stdint.h"

/*
  Direct3D globals
*/

/* 
 *	Pre-DX8 vertex format
 *	taken from http://www.mvps.org/directx/articles/definitions_for_dx7_vertex_types.htm
 */

typedef struct _D3DTVERTEX 
{
	float sx;
	float sy;
	float sz;

	D3DCOLOR color;
	D3DCOLOR specular;

	float tu;
	float tv;

} D3DLVERTEX;

#define D3DFVF_LVERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

typedef struct POINTSPRITEVERTEX
{
	float x;
	float y;
	float z;

	float size;

	DWORD colour;

	float u;
	float v;

} POINTSPRITEVERTEX;

#define D3DFVF_POINTSPRITEVERTEX (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct ORTHOVERTEX 
{
	float x;
	float y;
	float z;		// Position in 3d space 

	DWORD colour;	// Colour  

	float u;
	float v;		// Texture coordinates 

} ORTHOVERTEX;

// orthographic quad vertex format
#define D3DFVF_ORTHOVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef LPDIRECT3DTEXTURE8 D3DTEXTURE;

/* 
  Maximum number of Direct3D drivers ever
  expected to be resident on the system.
*/
#define MAX_D3D_DRIVERS 2
/*
  Maximum number of texture formats ever
  expected to be reported by a Direct3D
  driver.
*/
#define MAX_TEXTURE_FORMATS 10

/*
  Description of a D3D driver.
*/

typedef struct D3DDriverInfo {
	D3DFORMAT				Formats[20];
	D3DADAPTER_IDENTIFIER8	AdapterInfo;
	D3DDISPLAYMODE			DisplayMode[100];
	int						NumModes;
} D3DDRIVERINFO;


typedef struct D3DInfo {
    LPDIRECT3D8				lpD3D;
    LPDIRECT3DDEVICE8		lpD3DDevice; 
    D3DVIEWPORT8			D3DViewport; 
	D3DPRESENT_PARAMETERS	d3dpp;

	LPDIRECT3DVERTEXBUFFER8 lpD3DVertexBuffer;
	LPDIRECT3DINDEXBUFFER8	lpD3DIndexBuffer;

	LPDIRECT3DVERTEXBUFFER8 lpD3DOrthoVertexBuffer;

    int						NumDrivers;
    int						CurrentDriver;
	D3DDRIVERINFO			Driver[MAX_D3D_DRIVERS];
    int						CurrentTextureFormat;
    int						NumTextureFormats;

} D3DINFO;

extern D3DINFO d3d;

/* KJL 14:24:45 12/4/97 - render state information */
enum TRANSLUCENCY_TYPE
{
	TRANSLUCENCY_OFF,
	TRANSLUCENCY_NORMAL,
	TRANSLUCENCY_INVCOLOUR,
	TRANSLUCENCY_COLOUR,
	TRANSLUCENCY_GLOWING,
	TRANSLUCENCY_DARKENINGCOLOUR,
	TRANSLUCENCY_JUSTSETZ,
	TRANSLUCENCY_NOT_SET
};

enum FILTERING_MODE_ID
{
	FILTERING_BILINEAR_OFF,
	FILTERING_BILINEAR_ON,
	FILTERING_NOT_SET
};

enum TEXTURE_ADDRESS_MODE
{
	TEXTURE_WRAP,
	TEXTURE_CLAMP
};

typedef struct
{
	enum TRANSLUCENCY_TYPE TranslucencyMode;
	enum FILTERING_MODE_ID FilteringMode;
	enum TEXTURE_ADDRESS_MODE TextureAddressMode;
	int FogDistance;
	unsigned int FogIsOn :1;
	unsigned int WireFrameModeIsOn :1;

} RENDERSTATES;

D3DTEXTURE CreateD3DTexture(AVPTEXTURE *tex, uint8_t *buf, uint32_t usage, D3DPOOL poolType);
D3DTEXTURE CreateD3DTexturePadded(AVPTEXTURE *tex, uint32_t *realWidth, uint32_t *realHeight);
D3DTEXTURE CreateD3DTallFontTexture(AVPTEXTURE *tex);

BOOL ReleaseVolatileResources();
BOOL CreateVolatileResources();
BOOL ChangeGameResolution(int width, int height, int colour_depth);

void DrawAlphaMenuQuad(int topX, int topY, int image_num, int alpha);
void DrawTallFontCharacter(int topX, int topY, int texU, int texV, int char_width, int alpha);
void DrawBigChar(char c, int x, int y, int colour);
void DrawCloudTable(int topX, int topY, int word_length, int alpha);
void DrawFadeQuad(int topX, int topY, int alpha);
void DrawSmallMenuCharacter(int topX, int topY, int texU, int texV, int red, int green, int blue, int alpha);
void DrawProgressBar(RECT srcRect, RECT destRect, uint32_t textureID, AVPTEXTURE *tex, uint32_t newWidth, uint32_t newHeight);
void DrawQuad(uint32_t x, uint32_t y, uint32_t width, uint32_t height, int32_t textureID, uint32_t colour, enum TRANSLUCENCY_TYPE translucencyType);
void ReleaseD3DTexture(D3DTEXTURE *d3dTexture);
void DrawFmvFrame(uint32_t frameWidth, uint32_t frameHeight, uint32_t textureWidth, uint32_t textureHeight, D3DTEXTURE fmvTexture);
void DrawFmvFrame2(uint32_t frameWidth, uint32_t frameHeight, uint32_t textureWidth, uint32_t textureHeight, D3DTEXTURE tex[3]);
void CreateScreenShotImage();
void DeRedTexture(D3DTEXTURE texture);
D3DTEXTURE CreateFmvTexture(uint32_t *width, uint32_t *height, uint32_t usage, uint32_t pool);
D3DTEXTURE CreateFmvTexture2(uint32_t *width, uint32_t *height, uint32_t usage, uint32_t pool);
void SetTransforms();

D3DINFO GetD3DInfo();
char* GetDeviceName();
#define RGB_MAKE	D3DCOLOR_XRGB

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

#ifdef __cplusplus
}
#endif

#endif /* ! _included_d3_func_h_ */
