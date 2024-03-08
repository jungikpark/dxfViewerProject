/*-------------------------------------------------------------------*\
|  CadLib Version 2.1                                                 |
|  Written by Omid Shahabi <omid_shahabi@hotmail.com>                 |
|  Copyright ?2002-2004                                              |
|  Pars Technology Development Co.                                    |
|                                                                     |
|  This software is distributed on an "AS IS" basis, WITHOUT WARRANTY |
|  OF ANY KIND, either express or implied.                            |
|                                                                     |
|                                                                     |
|  DrawingDef.h                                                       |
\*-------------------------------------------------------------------*/

#ifndef DRAWINGDEF_H
#define DRAWINGDEF_H

#if _MSC_VER > 1000
#pragma once
#include <vector>
#endif // _MSC_VER > 1000

// Handle to DRAWING
DECLARE_HANDLE(HDRAWING);

// Window Messages
#define	WM_PROGRESS		WM_USER+1

// Definitions
typedef DWORD OBJHANDLE;
#define	MAX_STRLEN 512

#ifndef PI
 #define PI 3.141592653589793f
#endif

// Entities and Objects Definitions
#define	ENT_LINE		1
#define	ENT_POINT		2
#define	ENT_CIRCLE		3
#define	ENT_SHAPE		4
#define	ENT_ELLIPSE		5
#define ENT_SPLINE		6
#define ENT_TEXT		7
#define ENT_ARC			8
#define ENT_TRACE		9
#define ENT_REGION		10
#define ENT_SOLID		11
#define ENT_BLOCK		12
#define ENT_ENDBLK		13
#define ENT_INSERT		14
#define ENT_ATTDEF		15
#define ENT_ATTRIB		16
#define ENT_SEQEND		17
#define ENT_LWPOLYLINE	18 //-- ADD LWPOLYLINE - 2022.04.16 JIP -
#define ENT_POLYLINE	19
#define ENT_VERTEX		20
#define ENT_LINE3D		21
#define ENT_FACE3D		22
#define ENT_DIMENSION	23
#define ENT_VIEWPORT	24
#define ENT_SOLID3D		25
#define ENT_RAY			26
#define ENT_XLINE		27
#define ENT_MTEXT		28
#define ENT_LEADER		29
#define ENT_TOLERANCE	30
#define ENT_MLINE		31
#define OBJ_DICTIONARY	32
#define OBJ_MLINESTYLE	33
#define OBJ_CLASS		34
#define ENT_BODY		35
#define OBJ_GROUP		36
#define OBJ_PROXY		100
#define OBJ_XRECORD		101
#define OBJ_IDBUFFER	102

// Flag Definitions
#define	POLYLINE_CLOSED			1
#define	POLYLINE_CURVEFIT		2
#define	POLYLINE_SPLINEFIT		4
#define	POLYLINE_3DPLINE		8
#define	POLYLINE_3DMESH			16
#define	POLYLINE_CLOSEDN		32
#define	POLYLINE_POLYFACEMESH	64
#define	POLYLINE_CONTINUELT		128

// FindType Definitions
#define FIND_FIRST				0x0001
#define FIND_LAST				0x0002
#define FIND_NEXT				0x0004
#define FIND_PREVIOUS			0x0008
#define FIND_BYTYPE				0x0010
#define FIND_BYLAYEROBJHANDLE	0x0020
#define FIND_BYLTYPEOBJHANDLE	0x0040
#define FIND_BYCOLOR			0x0080
#define FIND_BYTHICKNESS		0x0100
#define FIND_BYNAME				0x0200
#define FIND_BYHANDLE			0x0400

// Color Definitions
#define	COLOR_BYLAYER	256
#define	COLOR_BYBLOCK	0
#define	COLOR_RED		1
#define	COLOR_YELLOW	2
#define	COLOR_GREEN		3
#define	COLOR_CYAN		4
#define	COLOR_BLUE		5
#define	COLOR_MAGENTA	6
#define	COLOR_WHITE		7

// Tables Structures *******************************************

typedef struct tag_DIMSTYLE {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// name of dimension style
	char	StandardFlags;			// Standard flag values
	double	dimasz;					// Arrow size
//	OBJHANDLE dimblk1Objhandle;		// 1st Arrow head
//	OBJHANDLE dimblk2Objhandle;		// 2nd Arrow head
	char	dimblk1[16];			// 1st Arrow head
	char	dimblk2[16];			// 2nd Arrow head
	short	dimclrd;				// Dimension line & Arrow heads color
	short	dimclre;				// Extension line color
	short	dimclrt;				// Text color
	double	dimdle;					// Dimension line size after Extensionline
	double	dimexe;					// Extension line size after Dimline
	double	dimexo;					// Offset from origin
	double	dimgap;					// Offset from dimension line
	double	dimtxt;					// Text height
	char	dimtad;					// Vertical Text Placement
	OBJHANDLE dimtxstyObjhandle;	// Text style handle
} DIMSTYLE, *PDIMSTYLE;

typedef struct tag_LAYER {
	tag_LAYER()
	{
		Objhandle = 0;
		StandardFlags = 0;
		Color = 0;
		LineTypeObjhandle = 0;
		LineWeight = 0;
		PlotFlag = 0;
		PlotStyleObjhandle = 0;
	}

	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// Layer Name
	char	StandardFlags;			// Standard flags
	short	Color; 					// Layer Color (if negative, layer is Off)
	OBJHANDLE LineTypeObjhandle;	// Handle of linetype for this layer
	short	LineWeight;				// Layer's Line Weight                      (*2000*)
	BOOL	PlotFlag;				// TRUE=Plot this layer                     (*2000*)
	OBJHANDLE PlotStyleObjhandle;	// handle of PlotStyleName object           (*2000*)
} LAYER, *PLAYER;

typedef struct tag_LTYPE {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// Line Type Name
	char	StandardFlags;			// Standard flags
	char	DescriptiveText[512];	// Descriptive Text
	short	ElementsNumber;			// Line Elements Number
	double	Elements[30];			// Line Elements (Max=30)
	double	PatternLength;          // Length of linetype pattern
} LTYPE, *PLTYPE;

typedef struct tag_STYLE {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// Style Name
	char	StandardFlags;			// Standard flag values -> 4=Vertical text
	char	TextGenerationFlags;	// Text generation flags -> 2=Text is backward  4=Text is upside down
	char	PrimaryFontFilename[256];// Primary Font Filename
	char	BigFontFilename[256];	// Big font filename
	double	ObliqueAngle;			// Oblique angle
	double	FixedTextHeight;		// Fixed text height
	double	WidthFactor;			// Width Factor
	double	LastHeightUsed;			// Height
} STYLE, *PSTYLE;


// Entity Structures *******************************************
typedef struct tag_ENTLINE {
	REALPOINT	Point0;
	REALPOINT	Point1;
} ENTLINE, *PENTLINE;

typedef struct tag_ENTARC {
	REALPOINT	Point0;
	double		Radius;
	double		StartAngle;
	double		EndAngle;
} ENTARC, *PENTARC;

typedef struct tag_ENTCIRCLE {
	REALPOINT	Point0;
	double		Radius;
} ENTCIRCLE, *PENTCIRCLE;

typedef struct tag_ENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;
	REALPOINT	DimLineDefPoint;
	OBJHANDLE	BlockheaderObjhandle;
	OBJHANDLE	DimStyleObjhandle;
	char		DimText[1024];
} ENTDIMENSION, *PENTDIMENSION;

typedef struct tag_ENTINSERT {
	REALPOINT	Point0;
	double		XScale;
	double		YScale;
	double		ZScale;
	double		RotationAngle;
	OBJHANDLE	BlockHeaderObjhandle;
} ENTINSERT, *PENTINSERT;

typedef struct tag_ENTPOINT {
	REALPOINT	Point0;
} ENTPOINT, *PENTPOINT;

typedef struct tag_ENTSOLID {
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} ENTSOLID, *PENTSOLID;

typedef struct tag_TEXTDATA {
	double		Height;
	double		RotationAngle;
	double		WidthFactor;
	double		Oblique;
	char		GenerationFlag;
	short		Justification;
	REALPOINT	SecondAlignmentPoint;
	OBJHANDLE	TextStyleObjhandle;
} _TEXTDATA,*_PTEXTDATA;

typedef struct tag_ENTTEXT {
	REALPOINT	Point0;
	_TEXTDATA	TextData;
	char		strText[2048];
} ENTTEXT, *PENTTEXT;

typedef struct tag_ENTVERTEX {
  REALPOINT			Point;
  unsigned short	Flag;
  double			StartWidth;
  double			EndWidth;
  double			Bulge;
  double			TangentDir;
} ENTVERTEX, *PENTVERTEX;

//--------------this code is implemented by Tran duy Dung 2003/10/20
typedef struct tag_ENTPOLYLINE {
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTPOLYLINE, *PENTPOLYLINE;

typedef struct tag_ENTELLIPSE {
  REALPOINT		CenterPoint;
  REALPOINT		MajorAxisEndPoint;
  double		MinorToMajorRatio;
  double		StartParam;
  double		EndParam;
} ENTELLIPSE, *PENTELLIPSE;

//**************************************************************
typedef struct tag_ENTITYHEADER
{
	unsigned short	EntityType;			// Entity type
	OBJHANDLE	Objhandle;				// Handle 
	OBJHANDLE	LayerObjhandle;			// Used Layer's Handle
	OBJHANDLE	LTypeObjhandle;			// Used LineType's Handle
	BOOL		Deleted;				// 0 = Not Deleted
	short		Color;					// Color (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// Thickness (default=0)
	double		LineTypeScale;			// Linetype scale (default=1.0)
	double		ExtrusionDirection[3];	// Extrusion direction. (default = 0, 0, 1)
	short		LineWeight;				// Lineweight enum value (*2000*)
	DWORD		PreviousEntityPos;
	DWORD		NextEntityPos;
} ENTITYHEADER, *PENTITYHEADER;

typedef struct tag_ENTITIES
{
	HGLOBAL		hEntities;
	DWORD		TotalSize;
	DWORD		FreePos;
	DWORD		LastEntityPos;
	DWORD		CurrentEntityPos;
	DWORD		EntitiesNumber;
} ENTITIES, *PENTITIES;

// Blocks Structures *******************************************
typedef struct tag_BLOCKHEADER
{
	OBJHANDLE	Objhandle;				// Handle
	char		Name[MAX_STRLEN];		// Block name
	char		Flags;					// Block-type flags
	REALPOINT	BasePoint;				// Base point
	OBJHANDLE	LayerObjhandle;			// Layer's Handle
	ENTITIES	Entities;
} BLOCKHEADER, *PBLOCKHEADER;

//
typedef struct tag_DRW_ENTT_LSIT
{
	int    Entt_Type;
	LAYER  Layer;
	int    Object_Id;
	double Line_Pt_Res;

	std::vector<bool> Pen_Down;
	std::vector<double> Point_X;
	std::vector<double> Point_Y;

} DRW_ENTT_LSIT, *PDRW_ENTT_LSIT;

typedef struct tag_DRW_ENTT_OBJECTS
{
	int    Entt_Type;
	int    Object_Id;
	double Line_Pt_Res;

	int Pen_Down_Cnt;
	int Point_X_Cnt;
	int Point_Y_Cnt;

	bool* pPen_Down;
	double* pPoint_X;
	double* pPoint_Y;

	LAYER  Layer;
} DRW_ENTT_OBJECTS, *PDRW_ENTT_OBJECTS;


typedef struct tag_LAYER_COM {
	tag_LAYER_COM()
	{
		Objhandle = 0;
		StandardFlags = 0;
		Color = 0;
		LineTypeObjhandle = 0;
		LineWeight = 0;
		PlotFlag = 0;
		PlotStyleObjhandle = 0;
	}

	OBJHANDLE Objhandle;			// Handle
	char*	Name;		// Layer Name
	char	StandardFlags;			// Standard flags
	short	Color; 					// Layer Color (if negative, layer is Off)
	OBJHANDLE LineTypeObjhandle;	// Handle of linetype for this layer
	short	LineWeight;				// Layer's Line Weight                      (*2000*)
	BOOL	PlotFlag;				// TRUE=Plot this layer                     (*2000*)
	OBJHANDLE PlotStyleObjhandle;	// handle of PlotStyleName object           (*2000*)
} LAYER_COM, *PLAYER_COM;
typedef struct tag_DRW_ENTT_OBJECTS_COM
{
	int    Entt_Type;
	int    Object_Id;
	double Line_Pt_Res;

	int Pen_Down_Cnt;
	int Point_X_Cnt;
	int Point_Y_Cnt;

	bool* pPen_Down;
	double* pPoint_X;
	double* pPoint_Y;

	LAYER_COM  Layer;
} DRW_ENTT_OBJECTS_COM, *PDRW_ENTT_OBJECTS_COM;

// View Structure **********************************************
typedef struct tag_VIEW
{
	tag_VIEW()
	{
		Entt_Type = 0;
		Arc_Pt_Res = 0.1;
		Line_Pt_Res = 1.0;
		pExtEnttList = NULL;
	}

	BOOL	Viewable;		// TRUE = Drawing can be viewed

	// -- BY JIP --
	int     Min_X;
	int     Min_Y;
	int     Max_X;
	int     Max_Y;
	int     Entt_Type;
	bool    bAddEntt;
	double  Arc_Pt_Res;
	double  Line_Pt_Res;

	int		WindowLeft;		// Drawing Window Boundary Properties (In Pixels)
	int		WindowTop;		// Drawing Window Boundary Properties (In Pixels)
	int		WindowRight;	// Drawing Window Boundary Properties (In Pixels)
	int		WindowBottom;	// Drawing Window Boundary Properties (In Pixels)
	double	ViewLeft;		// Drawing View Properties (In Units)
	double	ViewBottom;		// Drawing View Properties (In Units)
	double	ViewRight;		// Drawing View Properties (In Units)
	double	ViewTop;		// Drawing View Properties (In Units)
	double	PPU;			// Pixels Per Unit
	double	ZoomLevel;		// Zoom Level

	/*
	 * dll <-> app. Data 교환시 memory 접근에 문제 생겨서
	 * pEnttList 형을 vector<DRW_ENTT_LSIT>에서 void* 형으로 선언 - JIP -
	 */
	void* pEnttList;

	/*
	 * pEnttList 사용시 내부에서 메모리 생성후 해제시 간헐적으로 memory access violation 발생
	 * 외부 컨테이너 사용하여 테스트
	 */
	std::vector<DRW_ENTT_LSIT> *pExtEnttList;

} VIEW, *PVIEW;


#endif // DRAWINGDEF_H

// Definitions
#define		MAX_DIMSTYLES	64
#define		MAX_LAYERS		128
#define		MAX_LTYPES		128
#define		MAX_STYLES		128
#define     MAX_OBJECT      1024

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

// Tables ***************************
typedef struct tag_TABLES
{
	LAYER		Layer[MAX_LAYERS];
	LTYPE		LType[MAX_LTYPES];
	STYLE		Style[MAX_STYLES];
	DIMSTYLE	DimStyle[MAX_DIMSTYLES];

	UINT		NumLayers;
	UINT		NumLTypes;
	UINT		NumStyles;
	UINT		NumDimStyles;

	UINT		CurrentLayer;
	UINT		CurrentLType;
	UINT		CurrentStyle;
	UINT		CurrentDimStyle;

	//	OBJHANDLE	CurrentLayerHandle;		// Current Layer for drawing
	//	OBJHANDLE	CurrentTextStyleHandle;	// Current TextStyle for drawing
	//	OBJHANDLE	CurrentDimStyleHandle;	// Current DimStyle for drawing
	//	OBJHANDLE	CurrentLineTypeHandle;	// Current LineType for drawing
	//	short		CurrentColor;			// Current Color for drawing
	//	short		CurrentLineWeight;		// Current LineWeight for drawing
} TABLES, *PTABLES;

// Drawing Structure ****************
typedef struct tag_DRAWING
{
	TABLES		Tables;
	ENTITIES	Entities;

	BLOCKHEADER	Blocks[256];
	DWORD		BlocksNumber;
	DWORD		CurrentBlock;

	OBJHANDLE	LastObjhandle;
	DWORD		LastDimBlkNum;

	VIEW		View;
} DRAWING, *PDRAWING;

// Drawing Objects
typedef struct tag_OBJECT
{
	tag_OBJECT()
	{
		Count = 0;
		Pen_Down = NULL;
		Point_X = NULL;
		Point_Y = NULL;
	}

	void Release_Object()
	{
		Count = 0;

		if (Pen_Down != NULL)
		{
			delete[] Pen_Down;
			Pen_Down = NULL;
		}
		if (Point_X != NULL)
		{
			delete[] Point_X;
			Point_X = NULL;
		}
		if (Point_Y != NULL)
		{
			delete[] Point_Y;
			Point_Y = NULL;
		}
	}
	
	long    Count;
	bool*   Pen_Down;
	double* Point_X;
	double* Point_Y;

}OBJECT, *POBJECT;

typedef struct tag_DRAWING_OBJECTS
{
	tag_DRAWING_OBJECTS()
	{
		Init_Data();
	}

	void Init_Data()
	{
		Layer_ID = 0;
		Object_Cnt = 0;

		memset(Name, 0, sizeof(char) * MAX_STRLEN);
		memset(Object_ID, 0, sizeof(long) * MAX_OBJECT);
		memset(Object_Type, 0, sizeof(short) * MAX_OBJECT);
	}

	DWORD    Layer_ID;
	char     Name[MAX_STRLEN];// Layer Name

	long     Object_Cnt;
	long     Object_ID[MAX_OBJECT];
	short    Object_Type[MAX_OBJECT];

	OBJECT   Object[MAX_OBJECT];

}DRW_OBJECTS, *PDRW_OBJECTS;

typedef struct tag_MARKING_PARAM
{
	tag_MARKING_PARAM()
	{
		mkSpeed = 50;
		cutFrq = 3;
		qGain = 0.3;
		OnDelay = 0;
		OffDelay = 0;
		ta = 0;
		td = 0;
	}

	double mkSpeed;
	double cutFrq;
	double qGain;
	double OnDelay;
	double OffDelay;
	double ta;
	double td;

}MK_PARAM, *PMK_PARAM;