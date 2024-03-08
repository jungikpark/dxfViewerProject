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
|  View.cpp                                                           |
\*-------------------------------------------------------------------*/

#include "../../CadIO/StdAfx.h"
#include "View.h"
#include "Colors.h"
#include "TextView.h"
#include "DimView.h"

#ifndef PI
#define PI 3.141592653589793f
#endif

#define DEGREE  180/PI
#define RADIAN  PI/180

/********************************************************************
 * Functions for calculating entities boundary rectangle            *
 ********************************************************************/
double Calcu_Radius_By_Bulge(double bulge, double x1, double y1, double x2, double y2);
void Caluc_Rotate_Org(double bulge, double radius, double x1, double y1, double x2, double y2, double *rotX, double *rotY);
BOOL GetEntityRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, LPVOID pEntityData, REALRECT* pRect);

/*-------------------------------------------------------------------*
 |  GetLineRect                                                      |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTLINE pLine = pointer to line data structure              |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetLineRect(PENTITYHEADER pEntityHeader, PENTLINE pLine, REALRECT* pRect)
{
	pRect->top = __max(pLine->Point0.y, pLine->Point1.y);
	pRect->left = __min(pLine->Point0.x, pLine->Point1.x);
	pRect->bottom = __min(pLine->Point0.y, pLine->Point1.y);
	pRect->right = __max(pLine->Point0.x, pLine->Point1.x);
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetPointRect                                                     |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTPOINT pPoint = pointer to point data structure           |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetPointRect(PENTITYHEADER pEntityHeader, PENTPOINT pPoint, REALRECT* pRect)
{
	pRect->top = pPoint->Point0.y;
	pRect->left = pPoint->Point0.x;
	pRect->bottom = pPoint->Point0.y;
	pRect->right = pPoint->Point0.x;
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetCircleRect                                                    |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTCIRCLE pCircle = pointer to circle data structure        |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetCircleRect(PENTITYHEADER pEntityHeader, PENTCIRCLE pCircle, REALRECT* pRect)
{
	pRect->top = pCircle->Point0.y + pCircle->Radius;
	pRect->left = pCircle->Point0.x - pCircle->Radius;
	pRect->bottom = pCircle->Point0.y - pCircle->Radius;
	pRect->right = pCircle->Point0.x + pCircle->Radius;
	return TRUE;
}

//-- 2022.07.24 JIP
BOOL GetPolyArcRect(PENTPOLYLINE pPolyline, REALRECT* pRect)
{
	//--
	for (int i = 1; i < pPolyline->nVertex; i++)
	{
		double x1 = pPolyline->pVertex[i].Point.x;
		double y1 = pPolyline->pVertex[i].Point.y;
		double x2 = pPolyline->pVertex[i - 1].Point.x;
		double y2 = pPolyline->pVertex[i - 1].Point.y;
		double bulge = pPolyline->pVertex[i].Bulge;

		//--
		double radius = Calcu_Radius_By_Bulge(bulge, x1, y1, x2, y2);

		//--
		if (radius == 0) continue;

		//-- 원점 구하기
		double rotX, rotY;
		Caluc_Rotate_Org(bulge, radius, x1, y1, x2, y2, &rotX, &rotY);

		double angle = 4 * atan(bulge) * DEGREE;
		double ptX1 = x1 - rotX;
		double ptY1 = y1 - rotY;
		double ptX2 = x2 - rotX;
		double ptY2 = y2 - rotY;
		double rot_X = 0;
		double rot_Y = 0;
		long   angle_Sign = 1;

		//--
		if (bulge == 0)
		{
			angle = 180.0;
		}
		else if (bulge == radius)
		{
			angle = 360.0;
		}

		//--
		if (angle < 0)
		{
			angle_Sign = -1;
		}

		//-- Display
		for (double ang = 0; ang < fabs(angle); ang += 1)
		{
			double ang_radian = ang * RADIAN * angle_Sign;
			rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
			rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));

			//--
			if (pRect->left > rot_X)
			{
				pRect->left = rot_X;
			}
			if (pRect->right < rot_X)
			{
				pRect->right = rot_X;
			}
			if (pRect->top < rot_Y)
			{
				pRect->top = rot_Y;
			}
			if (pRect->bottom > rot_Y)
			{
				pRect->bottom = rot_Y;
			}
		}
	}

	return TRUE;
}

//-- 2022.07.23 JIP
BOOL GetArcRect(PENTITYHEADER pEntityHeader, PENTARC pEntityData, REALRECT* pRect)
{
	//-- 
	double x1 = (pEntityData->Point0.x + pEntityData->Radius * cos(pEntityData->StartAngle*PI / 180));
	double y1 = (pEntityData->Point0.y + pEntityData->Radius * sin(pEntityData->StartAngle*PI / 180));
	double org_X = pEntityData->Point0.x;
	double org_Y = pEntityData->Point0.y;
	double stAng = pEntityData->StartAngle;
	double edAng = pEntityData->EndAngle;
	double angle = 0;

	double ptX = x1 - org_X;
	double ptY = y1 - org_Y;

	if (fabs(stAng) > fabs(edAng))
	{
		angle = 360 - (stAng - edAng);
	}
	else
	{
		angle = edAng - stAng;
	}

	//--
	for (double ang = 0; ang < fabs(angle); ang += 1)
	{
		double ang_radian = ang * (PI / 180);
		double rot_X = ((ptX)* cos(ang_radian)) - ((ptY)* sin(ang_radian)) + org_X;
		double rot_Y = ((ptY)* cos(ang_radian)) + ((ptX)* sin(ang_radian)) + org_Y;

		//--
		if (ang == 0)
		{
			pRect->left = rot_X;
			pRect->right = rot_X;
			pRect->top = rot_Y;
			pRect->bottom = rot_Y;
		}
		else
		{
			//--
			if (pRect->left > rot_X)
			{
				pRect->left = rot_X;
			}
			if (pRect->right < rot_X)
			{
				pRect->right = rot_X;
			}
			if (pRect->top < rot_Y)
			{
				pRect->top = rot_Y;
			}
			if (pRect->bottom > rot_Y)
			{
				pRect->bottom = rot_Y;
			}
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetSolidRect                                                     |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTSOLID pSolid = pointer to solid data structure           |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetSolidRect(PENTITYHEADER pEntityHeader, PENTSOLID pSolid, REALRECT* pRect)
{
	pRect->top = __max(pSolid->Point0.y, pSolid->Point1.y);
	pRect->top = __max(pRect->top, pSolid->Point2.y);
	pRect->top = __max(pRect->top, pSolid->Point3.y);
	pRect->left = __min(pSolid->Point0.x, pSolid->Point1.x);
	pRect->left = __min(pRect->left, pSolid->Point2.x);
	pRect->left = __min(pRect->left, pSolid->Point3.x);
	pRect->bottom = __min(pSolid->Point0.y, pSolid->Point1.y);
	pRect->bottom = __min(pRect->bottom, pSolid->Point2.y);
	pRect->bottom = __min(pRect->bottom, pSolid->Point3.y);
	pRect->right = __max(pSolid->Point0.x, pSolid->Point1.x);
	pRect->right = __max(pRect->right, pSolid->Point2.x);
	pRect->right = __max(pRect->right, pSolid->Point3.x);
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetInsertBlockRect                                               |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTINSERT pInsert = pointer to insert data structure        |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetInsertBlockRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, PENTINSERT pInsert, REALRECT* pRect)
{
	BLOCKHEADER		BlockHeader;
	ENTITYHEADER	BlockEntityHeader;
	char			BlockEntityData[4096];

	pRect->left = DBL_MAX;
	pRect->top = DBL_MIN;
	pRect->right = DBL_MIN;
	pRect->bottom = DBL_MAX;

	BlockHeader.Objhandle = pInsert->BlockHeaderObjhandle;
	if(drwFindBlock_Direct(pDrawing, FIND_BYHANDLE, &BlockHeader)>0)
	{
		if(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_FIRST)>0)
		{
			do{
				REALRECT	Rect;
				if(GetEntityRect(pDrawing, &BlockEntityHeader, BlockEntityData, &Rect))
				{
					pRect->top = __max(Rect.top*pInsert->YScale+pInsert->Point0.y, pRect->top);
					pRect->left = __min(Rect.left*pInsert->XScale+pInsert->Point0.x, pRect->left);
					pRect->bottom = __min(Rect.bottom*pInsert->YScale+pInsert->Point0.y, pRect->bottom);
					pRect->right = __max(Rect.right*pInsert->XScale+pInsert->Point0.x, pRect->right);
				}
			} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_NEXT)>0);
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetPolylineRect                                                  |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTPOLYLINE pPolyline = pointer to polyline data structure  |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetPolylineRect(PENTITYHEADER pEntityHeader, PENTPOLYLINE pPolyline, REALRECT* pRect)
{
	pRect->top = pPolyline->pVertex[0].Point.y;
	pRect->left = pPolyline->pVertex[0].Point.x;
	pRect->bottom = pPolyline->pVertex[0].Point.y;
	pRect->right = pPolyline->pVertex[0].Point.x;

	for(int i=1; i<pPolyline->nVertex; i++)
	{
		pRect->top = __max(pRect->top, pPolyline->pVertex[i].Point.y);
		pRect->left = __min(pRect->left, pPolyline->pVertex[i].Point.x);
		pRect->bottom = __min(pRect->bottom, pPolyline->pVertex[i].Point.y);
		pRect->right = __max(pRect->right, pPolyline->pVertex[i].Point.x);
	}

	//--- Arc가 있을경우...
	for (int i = 0; i < pPolyline->nVertex; i++)
	{
		//--
		if (pPolyline->pVertex[i].Bulge == 0) continue;

		//--
		GetPolyArcRect(pPolyline, pRect);
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetEntityRect                                                    |
 |  Descriptions:                                                    |
 |      Calculates entity boundary rectangle                         |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      LPVOID pEntityData = pointer to entity data structure        |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetEntityRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, LPVOID pEntityData, REALRECT* pRect)
{
	BOOL	result;

	switch( pEntityHeader->EntityType )
	{
	case ENT_LINE:
		result = GetLineRect(pEntityHeader, (PENTLINE)pEntityData, pRect);
		break;

	case ENT_POINT:
		result = GetPointRect(pEntityHeader, (PENTPOINT)pEntityData, pRect);
		break;

	case ENT_CIRCLE:
		result = GetCircleRect(pEntityHeader, (PENTCIRCLE)pEntityData, pRect);
		break;

//	case ENT_ELLIPSE:
//		break;

//	case ENT_TEXT:
//		break;

	case ENT_ARC:
		result = GetArcRect(pEntityHeader, (PENTARC)pEntityData, pRect);
		break;

	case ENT_SOLID:
		result = GetSolidRect(pEntityHeader, (PENTSOLID)pEntityData, pRect);
		break;

	case ENT_INSERT:
		result = GetInsertBlockRect(pDrawing, pEntityHeader, (PENTINSERT)pEntityData, pRect);
		break;

	case ENT_POLYLINE:
		result = GetPolylineRect(pEntityHeader, (PENTPOLYLINE)pEntityData, pRect);
		break;

	case ENT_DIMENSION:
		result = GetDimRect(pDrawing, pEntityHeader, (PENTDIMENSION)pEntityData, pRect);
		break;

	default: // Entity type is not accepted
		result = FALSE;
	}

	return result;
}

/********************************************************************
 * Functions for plot the drawing on DC                             *
 ********************************************************************/

// Units to pixel conversions
#define	U2PX(units)	pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x+(units)*xScale) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PY(units)	pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y+(units)*yScale) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PXROT(xUnits,yUnits)	pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x+((xUnits)*xScale*cos(Rotation*PI/180)-(yUnits)*yScale*sin(Rotation*PI/180))*1) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PYROT(xUnits,yUnits)	pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y+((xUnits)*xScale*sin(Rotation*PI/180)+(yUnits)*yScale*cos(Rotation*PI/180))*1) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)

long   g_Object_Id;
LAYER  g_Layer_Entt;

//---- By JIP -----//
void Set_Min_Max_X(PDRAWING pDrawing, int x1, int x2)
{
	pDrawing->View.Min_X = __min(x1, pDrawing->View.Min_X);
	pDrawing->View.Min_X = __min(x2, pDrawing->View.Min_X);

	pDrawing->View.Max_X = __max(x1, pDrawing->View.Max_X);
	pDrawing->View.Max_X = __max(x2, pDrawing->View.Max_X);
}

void Set_Min_Max_Y(PDRAWING pDrawing, int y1, int y2)
{
	pDrawing->View.Min_Y = __min(y1, pDrawing->View.Min_Y);
	pDrawing->View.Min_Y = __min(y2, pDrawing->View.Min_Y);

	pDrawing->View.Max_Y = __max(y1, pDrawing->View.Max_Y);
	pDrawing->View.Max_Y = __max(y2, pDrawing->View.Max_Y);
}
//----  End   ----//
//--
void addEntt_Point(PDRAWING pDrawing, double ptx, double pty, long Object_Id, bool Pen_Down)
{
	if (pDrawing->View.bAddEntt == false) return;

	//--
	DRW_ENTT_LSIT entt_list;

	//--
	entt_list.Object_Id = Object_Id;
	entt_list.Entt_Type = pDrawing->View.Entt_Type;
	entt_list.Line_Pt_Res = pDrawing->View.Line_Pt_Res;

	//--
	entt_list.Point_X.push_back(ptx);
	entt_list.Point_Y.push_back(pty);

	//--
	entt_list.Layer = g_Layer_Entt;
	entt_list.Pen_Down.push_back(Pen_Down);

	//-- Add
	if (pDrawing->View.pExtEnttList == NULL)
	{
		std::vector<DRW_ENTT_LSIT> *pDrwEntt;

		if (pDrawing->View.pEnttList == NULL)
		{
			pDrwEntt = new std::vector<DRW_ENTT_LSIT>;
			pDrawing->View.pEnttList = (void*)pDrwEntt;
		}
		else
		{
			pDrwEntt = static_cast<std::vector<DRW_ENTT_LSIT> *> (pDrawing->View.pEnttList);
		}

		//--
		pDrwEntt->push_back(entt_list);
	}
	else
	{
		pDrawing->View.pExtEnttList->push_back(entt_list);
	}
}

//--
void addEntt_Solid(PDRAWING pDrawing, PENTSOLID pSolidData, long Object_Id)
{
	if (pDrawing->View.bAddEntt == false) return;

	//--
	DRW_ENTT_LSIT entt_list;

	//--
	entt_list.Object_Id = Object_Id;
	entt_list.Layer = g_Layer_Entt;
	entt_list.Entt_Type = pDrawing->View.Entt_Type;
	entt_list.Line_Pt_Res = pDrawing->View.Line_Pt_Res;

	//--
	double X0 = pSolidData->Point0.x;
	double Y0 = pSolidData->Point0.y;

	double X1 = pSolidData->Point1.x;
	double Y1 = pSolidData->Point1.y;

	double X2 = pSolidData->Point2.x;
	double Y2 = pSolidData->Point2.y;

	double X3 = pSolidData->Point3.x;
	double Y3 = pSolidData->Point3.y;

	entt_list.Point_X.push_back(X0);
	entt_list.Point_Y.push_back(Y0);
	entt_list.Pen_Down.push_back(true);

	entt_list.Point_X.push_back(X1);
	entt_list.Point_Y.push_back(Y1);
	entt_list.Pen_Down.push_back(true);

	entt_list.Point_X.push_back(X2);
	entt_list.Point_Y.push_back(Y2);
	entt_list.Pen_Down.push_back(true);

	entt_list.Point_X.push_back(X3);
	entt_list.Point_Y.push_back(Y3);
	//-- 마지막 포인트
	entt_list.Pen_Down.push_back(false);

	//-- Add
	if (pDrawing->View.pExtEnttList == NULL)
	{
		std::vector<DRW_ENTT_LSIT> *pDrwEntt;

		if (pDrawing->View.pEnttList == NULL)
		{
			pDrwEntt = new std::vector<DRW_ENTT_LSIT>;
			pDrawing->View.pEnttList = (void*)pDrwEntt;
		}
		else
		{
			pDrwEntt = static_cast<std::vector<DRW_ENTT_LSIT> *> (pDrawing->View.pEnttList);
		}

		//--
		pDrwEntt->push_back(entt_list);
	}
	else
	{
		pDrawing->View.pExtEnttList->push_back(entt_list);
	}
}

//--
void addLine_Point(PDRAWING pDrawing, double org_x1, double org_y1, double org_x2, double org_y2, long Object_Id, bool Pen_Down, short enttType)
{
	if (pDrawing->View.bAddEntt == false) return;

	//--
	DRW_ENTT_LSIT entt_list;
	double m, line_pt_res;

	line_pt_res = pDrawing->View.Line_Pt_Res;

	entt_list.Object_Id = Object_Id;
	entt_list.Layer = g_Layer_Entt;
	entt_list.Entt_Type = enttType;
	entt_list.Line_Pt_Res = line_pt_res;

	//--
	if (fabs(org_x2 - org_x1) > fabs(org_y2 - org_y1))
	{
		m = (double)(org_y2 - org_y1) / (org_x2 - org_x1);

		if (org_x2 > org_x1)
		{
			for (double i = org_x1; i < org_x2; i += line_pt_res)
			{
				double j = (m * (i - org_x1)) + org_y1;

				//--
				entt_list.Point_X.push_back(i);
				entt_list.Point_Y.push_back(j);

				if (enttType == ENT_POLYLINE || enttType == ENT_LWPOLYLINE)
				{
					entt_list.Pen_Down.push_back(Pen_Down);
				}
				else
				{
					entt_list.Pen_Down.push_back(true);
				}
			}

			//-- 마지막 Point
			if (enttType != ENT_POLYLINE && enttType != ENT_LWPOLYLINE)
			{
				entt_list.Point_X.push_back(org_x2);
				entt_list.Point_Y.push_back(org_y2);
				entt_list.Pen_Down.push_back(false);
			}
		}
		else
		{
			for (double i = org_x1; i > org_x2; i -= line_pt_res)
			{
				double j = (m * (i - org_x1)) + org_y1;

				//--
				entt_list.Point_X.push_back(i);
				entt_list.Point_Y.push_back(j);

				if (enttType == ENT_POLYLINE || enttType == ENT_LWPOLYLINE)
				{
					entt_list.Pen_Down.push_back(Pen_Down);
				}
				else
				{
					entt_list.Pen_Down.push_back(true);
				}
			}

			//-- 마지막 Point
			if (enttType != ENT_POLYLINE && enttType != ENT_LWPOLYLINE)
			{
				entt_list.Point_X.push_back(org_x2);
				entt_list.Point_Y.push_back(org_y2);
				entt_list.Pen_Down.push_back(false);
			}
		}
	}
	else
	{
		m = (double)(org_x2 - org_x1) / (org_y2 - org_y1);

		if (org_y2 > org_y1)
		{
			for (double i = org_y1; i < org_y2; i += line_pt_res)
			{
				double j = (m * (i - org_y1)) + org_x1;

				//--
				entt_list.Point_X.push_back(j);
				entt_list.Point_Y.push_back(i);

				if (enttType == ENT_POLYLINE || enttType == ENT_LWPOLYLINE)
				{
					entt_list.Pen_Down.push_back(Pen_Down);
				}
				else
				{
					entt_list.Pen_Down.push_back(true);
				}
			}

			//-- 마지막 Point
			if (enttType != ENT_POLYLINE && enttType != ENT_LWPOLYLINE)
			{
				entt_list.Point_X.push_back(org_x2);
				entt_list.Point_Y.push_back(org_y2);
				entt_list.Pen_Down.push_back(false);
			}
		}
		else
		{
			for (double i = org_y1; i > org_y2; i -= line_pt_res)
			{
				double j = (m * (i - org_y1)) + org_x1;

				//--
				entt_list.Point_X.push_back(j);
				entt_list.Point_Y.push_back(i);

				if (enttType == ENT_POLYLINE || enttType == ENT_LWPOLYLINE)
				{
					entt_list.Pen_Down.push_back(Pen_Down);
				}
				else
				{
					entt_list.Pen_Down.push_back(true);
				}
			}

			//-- 마지막 Point
			if (enttType != ENT_POLYLINE && enttType != ENT_LWPOLYLINE)
			{
				entt_list.Point_X.push_back(org_x2);
				entt_list.Point_Y.push_back(org_y2);
				entt_list.Pen_Down.push_back(false);
			}
		}
	}

	//-- Add
	if (pDrawing->View.pExtEnttList == NULL)
	{
		std::vector<DRW_ENTT_LSIT> *pDrwEntt;

		if (pDrawing->View.pEnttList == NULL)
		{
			pDrwEntt = new std::vector<DRW_ENTT_LSIT>;
			pDrawing->View.pEnttList = (void*)pDrwEntt;
		}
		else
		{
			pDrwEntt = static_cast<std::vector<DRW_ENTT_LSIT> *> (pDrawing->View.pEnttList);
		}

		//--
		pDrwEntt->push_back(entt_list);
	}
	else
	{
		pDrawing->View.pExtEnttList->push_back(entt_list);
	}
}

//-- Arc 방향 계산
int Get_Radius_Sign(double X_Dir, double Y_Dir, double ptAngle, double bulge)
{
	//--
	bool bDir_Up = true;

	//--
	if (X_Dir == 0)
	{
		if (Y_Dir < 0 && bulge < 0)
		{
			return -1;
		}
		else if (Y_Dir > 0 && bulge > 0)
		{
			return -1;
		}

		return 1;
	}

	if (Y_Dir == 0)
	{
		if (X_Dir > 0 && bulge < 0)
		{
			return -1;
		}
		else if (X_Dir < 0 && bulge > 0)
		{
			return -1;
		}

		return 1;
	}

	//--
	if (Y_Dir < 0)
	{
		bDir_Up = false;
	}

	//--
	if (bDir_Up == true)
	{
		if (ptAngle > 0 && bulge < 0)
		{
			return -1;
		}
		else if (ptAngle < 0 && bulge > 0)
		{
			return -1;
		}
	}
	else if (bDir_Up == false)
	{
		if (ptAngle > 0 && bulge > 0)
		{
			return -1;
		}
		else if (ptAngle < 0 && bulge < 0)
		{
			return -1;
		}
	}

	return 1;
}

//-- bulge 와 두 Point를 이용하여 radius를 계산한다 - 2022.04.18 JIP -
double Calcu_Radius_By_Bulge(double bulge, double x1, double y1, double x2, double y2)
{
	//--
	if (bulge == 0) return 0;

	//-- angle, distance
	double xDist = x2 - x1;
	double yDist = y2 - y1;
	double angle = 4 * atan(bulge);
	double ptAng = 0;

	if (xDist == 0) ptAng = 90;
	else if (yDist == 0) ptAng = 0;
	else ptAng = atan(yDist / xDist);

	double dist = sqrt(pow(xDist, 2) + pow(yDist, 2)) / 2.0;
	double radius = fabs(dist / sin(angle / 2.0));
	int radius_sign = 1;

	radius_sign = Get_Radius_Sign(xDist, yDist, ptAng, bulge);
	
	return radius * radius_sign;
}

//-- Arc 회전을 위한 원점좌표를 구한다 - 2022.04.18 JIP -
void Caluc_Rotate_Org(double bulge, double radius, double x1, double y1, double x2, double y2, double *rotX, double *rotY)
{
	//-- 직선의 센터 좌표
	double xDist = x2 - x1;
	double yDist = y2 - y1;
	double dist = sqrt(pow(xDist, 2) + pow(yDist, 2));
	double line_CenterX = x1 + (xDist / 2.0);
	double line_CenterY = y1 + (yDist / 2.0);
	double line_Angle = atan(yDist / xDist);

	//--
	if (radius < 0 && bulge > 0)
	{
		bulge = bulge * -1;
	}
	else if (radius > 0 && bulge < 0)
	{
		bulge = bulge * -1;
	}

	//-- i
	double i = (dist * bulge) / 2.0;

	//-- 회전
	double rot_X = 0;
	double rot_Y = radius - i;

	if (bulge == radius)
	{
		rot_Y = 0;
	}

	double rotedX = (rot_X * cos(line_Angle)) - (rot_Y * sin(line_Angle));
	double rotedY = (rot_Y * cos(line_Angle)) + (rot_X * sin(line_Angle));

	//--
	*rotX = line_CenterX + rotedX;
	*rotY = line_CenterY + rotedY;
}

//-- 원점 그리기
void DrawOrgCross(PDRAWING pDrawing, HDC hdc, int x1, int y1, int x2, int y2)
{
	//--
	int x_len = (x2 - x1) / 2;
	int y_len = (y1 - y2) / 2;
	int orgX = x1 + x_len;
	int orgY = y2 + y_len;
	int arrowW = 5;
	int arrowH = 3;

	//-- X
	MoveToEx(hdc, orgX - x_len, orgY, NULL);
	LineTo(hdc, orgX + x_len, orgY);

	MoveToEx(hdc, orgX + x_len - arrowW, orgY + arrowH, NULL);
	LineTo(hdc, orgX + x_len - arrowW, orgY - arrowH);

	MoveToEx(hdc, orgX + x_len - arrowW, orgY + arrowH, NULL);
	LineTo(hdc, orgX + x_len, orgY);

	MoveToEx(hdc, orgX + x_len - arrowW, orgY - arrowH, NULL);
	LineTo(hdc, orgX + x_len, orgY);

	//-- Y
	MoveToEx(hdc, orgX, orgY + y_len, NULL);
	LineTo(hdc, orgX, orgY - y_len);

	MoveToEx(hdc, orgX - arrowH, orgY - y_len + arrowW, NULL);
	LineTo(hdc, orgX + arrowH, orgY - y_len + arrowW);

	MoveToEx(hdc, orgX - arrowH, orgY - y_len + arrowW, NULL);
	LineTo(hdc, orgX, orgY - y_len);

	MoveToEx(hdc, orgX + arrowH, orgY - y_len + arrowW, NULL);
	LineTo(hdc, orgX, orgY - y_len);
}

//--
void DrawArc(PDRAWING pDrawing, HDC hdc, COLORREF rgbcolor, double rotX, double rotY, double radius, double stX, double stY, double edX, double edY, double stAng, double edAng, 
	double x, double y, double xScale, double yScale, double Rotation, long Object_Id, bool Pen_Down, short enttType)
{
	//--
	double ptX1 = stX - rotX;
	double ptY1 = stY - rotY;
	double ptX2 = edX - rotX;
	double ptY2 = edY - rotY;
	double rot_X = 0;
	double rot_Y = 0;
	long   angle_Sign = 1;

	//---
	double angle = 0;

	if (fabs(stAng) > fabs(edAng))
	{
		angle = 360 - (stAng - edAng);
	}
	else
	{
		angle = edAng - stAng;
	}

	//--
	double stepAng = 1.0;
	double arcLen = (2 * PI * radius) * (angle / 360.0);
	double pixRes;

	//--
	double scrX1 = U2PXROT(stX, stY);
	double scrY1 = U2PYROT(stX, stY);
	double scrX2 = U2PXROT(edX, edY);
	double scrY2 = U2PYROT(edX, edY);
	double srcRotX = U2PXROT(rotX, rotY);
	double srcRotY = U2PYROT(rotX, rotY);

	//-- 화면 좌표와 실좌표와의 비율 계산
	double srcRadius = sqrt(pow(scrX1 - srcRotX, 2) + pow(scrY1 - srcRotY, 2));
	double srcLen = (2 * PI * srcRadius) * (angle / 360.0);

	pixRes = arcLen / srcLen;//-- 화면 한픽셀당 분해능
	stepAng = fabs(pixRes / (2 * PI * radius) * 360.0);

	//--
	double enttAng = 0;
	DRW_ENTT_LSIT entt_list;

	if (pDrawing->View.bAddEntt == true)
	{
		//--
		double line_pt_res = pDrawing->View.Arc_Pt_Res;

		entt_list.Entt_Type = pDrawing->View.Entt_Type;
		entt_list.Line_Pt_Res = line_pt_res;

		enttAng = fabs(line_pt_res / (2 * PI * radius) * 360.0);
	}

	//-- Display
	for (double ang = 0; ang < fabs(angle); ang += stepAng)
	{
		double ang_radian = ang * RADIAN * angle_Sign;
		rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
		rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));

		//-- 회전된 위치
		SetPixel(hdc, U2PXROT(rotX + rot_X, rotY + rot_Y), U2PYROT(rotX + rot_X, rotY + rot_Y), rgbcolor);
	}

	//--
	entt_list.Object_Id = Object_Id;
	entt_list.Layer = g_Layer_Entt;

	//-- Entt Data
	for (double ang = 0; ang < fabs(angle); ang += enttAng)
	{
		if (pDrawing->View.bAddEntt == false) break;

		double ang_radian = ang * RADIAN * angle_Sign;
		rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
		rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));

		//--
		double pointX;
		double pointY;

		pointX = rotX + rot_X;
		pointY = rotY + rot_Y;
		entt_list.Point_X.push_back(pointX);
		entt_list.Point_Y.push_back(pointY);
		entt_list.Pen_Down.push_back(true);
	}

	//-- Arc 마지막 포인트 - 2022.05.19 JIP -
	if (pDrawing->View.bAddEntt == true)
	{
		double ang_radian = fabs(angle) * RADIAN * angle_Sign;
		rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
		rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));

		//--
		double pointX;
		double pointY;

		pointX = rotX + rot_X;
		pointY = rotY + rot_Y;
		entt_list.Point_X.push_back(pointX);
		entt_list.Point_Y.push_back(pointY);

		//--
		if (enttType == ENT_POLYLINE || enttType == ENT_LWPOLYLINE)
		{
			//--
			entt_list.Pen_Down.push_back(Pen_Down);
		}
		else
		{
			entt_list.Pen_Down.push_back(false);
		}
	}

	//-- x2, y2 위치 : Last Point
	SetPixel(hdc, U2PXROT(edX, edY), U2PYROT(edX, edY), rgbcolor);

	//-- Add
	if (pDrawing->View.pExtEnttList == NULL)
	{
		std::vector<DRW_ENTT_LSIT> *pDrwEntt;

		if (pDrawing->View.pEnttList == NULL)
		{
			pDrwEntt = new std::vector<DRW_ENTT_LSIT>;
			pDrawing->View.pEnttList = (void*)pDrwEntt;
		}
		else
		{
			pDrwEntt = static_cast<std::vector<DRW_ENTT_LSIT>*> (pDrawing->View.pEnttList);
		}

		//--
		pDrwEntt->push_back(entt_list);
	}
	else
	{
		pDrawing->View.pExtEnttList->push_back(entt_list);
	}
}

//-- Set Pixel이용하여 Arc 그림 - 2022.04.18 JIP -
void DrawArcByBulge(PDRAWING pDrawing, HDC hdc, COLORREF rgbcolor,double bulge, double radius, double rotX, double rotY, double x1, double y1, double x2, double y2,
	double x, double y, double xScale, double yScale, double Rotation, long Object_Id, bool Pen_Down, short enttType)
{
	//--
	double angle = 4 * atan(bulge) * DEGREE;
	double ptX1 = x1 - rotX;
	double ptY1 = y1 - rotY;
	double ptX2 = x2 - rotX;
	double ptY2 = y2 - rotY;
	double rot_X = 0;
	double rot_Y = 0;
	long   angle_Sign = 1;

	//--
	double stepAng = 1.0;
	double arcLen = (2 * PI * radius) * (angle / 360.0);
	double pixRes;

	//--
	double scrX1 = U2PXROT(x1, y1);
	double scrY1 = U2PYROT(x1, y1);
	double scrX2 = U2PXROT(x2, y2);
	double scrY2 = U2PYROT(x2, y2);

	//--Pixel 분해능 구한 후 Pixel 당 degree 계산
	double scrDist = sqrt(pow(scrX2 - scrX1, 2) + pow(scrY2 - scrY1, 2));
	double realDist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

	pixRes = realDist / scrDist;
	stepAng = fabs(pixRes / (2 * PI * radius) * 360.0);
	
	//--
	double enttAng = 0;
	DRW_ENTT_LSIT entt_list;

	if (pDrawing->View.bAddEntt == true)
	{
		//--
		double line_pt_res = pDrawing->View.Arc_Pt_Res;

		entt_list.Entt_Type = pDrawing->View.Entt_Type;
		entt_list.Line_Pt_Res = line_pt_res;

		enttAng = fabs(line_pt_res / (2 * PI * radius) * 360.0);
	}

	//--
	if (bulge == 0)
	{
		angle = 180.0;
	}
	else if (bulge == radius)
	{
		angle = 360.0;
	}

	//--
	if(angle < 0)
	{
		angle_Sign = -1;
	}

	//-- Display
	for (double ang = 0; ang < fabs(angle); ang += stepAng)
	{
		double ang_radian = ang * RADIAN * angle_Sign;
		rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
		rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));

		//-- 회전된 위치
		SetPixel(hdc, U2PXROT(rotX + rot_X, rotY + rot_Y), U2PYROT(rotX + rot_X, rotY + rot_Y), rgbcolor);
	}

	//--
	entt_list.Object_Id = Object_Id;
	entt_list.Layer = g_Layer_Entt;

	//-- Entt Data
	for (double ang = 0; ang < fabs(angle); ang += enttAng)
	{
		if (pDrawing->View.bAddEntt == false) break;

		double ang_radian = ang * RADIAN * angle_Sign;
		rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
		rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));

		//--
		double pointX;
		double pointY;

		pointX = rotX + rot_X;
		pointY = rotY + rot_Y;
		entt_list.Point_X.push_back(pointX);
		entt_list.Point_Y.push_back(pointY);
		entt_list.Pen_Down.push_back(true);
	}

	//-- Arc 마지막 포인트 - 2022.05.19 JIP -
	if (pDrawing->View.bAddEntt == true)
	{
		double ang_radian = fabs(angle) * RADIAN * angle_Sign;
		rot_X = ((ptX1)* cos(ang_radian)) - ((ptY1)* sin(ang_radian));
		rot_Y = ((ptY1)* cos(ang_radian)) + ((ptX1)* sin(ang_radian));
	
		//--
		double pointX;
		double pointY;
	
		pointX = rotX + rot_X;
		pointY = rotY + rot_Y;
		entt_list.Point_X.push_back(pointX);
		entt_list.Point_Y.push_back(pointY);
		
		//--
		if (enttType == ENT_POLYLINE || enttType == ENT_LWPOLYLINE)
		{
			//--
			entt_list.Pen_Down.push_back(Pen_Down);
		}
		else
		{
			entt_list.Pen_Down.push_back(false);
		}
	}

	//-- x2, y2 위치 : Last Point
	SetPixel(hdc, U2PXROT(x2, y2), U2PYROT(x2, y2), rgbcolor);

	//-- Add
	if (pDrawing->View.pExtEnttList == NULL)
	{
		std::vector<DRW_ENTT_LSIT> *pDrwEntt;

		if (pDrawing->View.pEnttList == NULL)
		{
			pDrwEntt = new std::vector<DRW_ENTT_LSIT>;
			pDrawing->View.pEnttList = (void*)pDrwEntt;
		}
		else
		{
			pDrwEntt = static_cast<std::vector<DRW_ENTT_LSIT>*> (pDrawing->View.pEnttList);
		}

		//--
		pDrwEntt->push_back(entt_list);
	}
	else
	{
		pDrawing->View.pExtEnttList->push_back(entt_list);
	}
}

/*-------------------------------------------------------------------*
 |  DrawLine                                                         |
 |  Descriptions:                                                    |
 |      Draws a line in view area                                    |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      int x1 = x1 of line                                          |
 |      int y1 = y1 of line                                          |
 |      int x2 = x2 of line                                          |
 |      int y2 = y2 of line                                          |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void DrawLine(PDRAWING pDrawing, HDC hdc, COLORREF rgbcolor, double org_x1, double org_y1, double org_x2, double org_y2,
	double x, double y, double xScale, double yScale, double Rotation, long Object_Id, bool Pen_Down, short enttType)
{
	double m;
	int x1, x2, y1, y2;
	
	x1 = U2PXROT(org_x1, org_y1);
	y1 = U2PYROT(org_x1, org_y1);
	x2 = U2PXROT(org_x2, org_y2);
	y2 = U2PYROT(org_x2, org_y2);
	
	//-- By JIP
	//--Set_Min_Max_X(pDrawing, x1, x2);
	//--Set_Min_Max_Y(pDrawing, y1, y2);
	
	if(abs(x2-x1)>abs(y2-y1))
	{
		if(x1>x2)
		{
			int dummy;
			dummy = x1; x1 = x2; x2 = dummy;
			dummy = y1; y1 = y2; y2 = dummy;
		}
		//--if((y2>pDrawing->View.WindowTop) && (y1<pDrawing->View.WindowBottom))
		{
			m = (double)(y2-y1)/(x2-x1);
			for(int i=__max(x1,pDrawing->View.WindowLeft); i<=__min(x2,pDrawing->View.WindowRight); i++)
			{
				int j = int(m*(i-x1))+y1;
				if((j>=pDrawing->View.WindowTop) && (j<=pDrawing->View.WindowBottom))
				  SetPixel( hdc, i, j, rgbcolor );
			}
		}
	}
	else
	{
		if(y1>y2)
		{
			int dummy;
			dummy = x1; x1 = x2; x2 = dummy;
			dummy = y1; y1 = y2; y2 = dummy;
		}
		//--if((x2>pDrawing->View.WindowLeft) && (x1<pDrawing->View.WindowRight))
		{
			m = (double)(x2-x1)/(y2-y1);
			for(int j=__max(y1,pDrawing->View.WindowTop); j<=__min(y2,pDrawing->View.WindowBottom); j++)
			{
				int i = int(m*(j-y1))+x1;
				if((i>=pDrawing->View.WindowLeft) && (i<=pDrawing->View.WindowRight))
				  SetPixel( hdc, i, j, rgbcolor );
			}
		}
	}

	//--
	addLine_Point(pDrawing, org_x1, org_y1, org_x2, org_y2, Object_Id, Pen_Down, enttType);
}


/*-------------------------------------------------------------------*
 |  DrawDashLine                                                     |
 |  Descriptions:                                                    |
 |      Draws a line in view area                                    |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      int x1 = x1 of line                                          |
 |      int y1 = y1 of line                                          |
 |      int x2 = x2 of line                                          |
 |      int y2 = y2 of line                                          |
 |      double PatternStartPoint                                     |
 |      double x = x position of insertion point (default = 0)       |
 |      double y = y position of insertion point (default = 0)       |
 |      double xScale = x scale factor (default = 1)                 |
 |      double yScale = y scale factor (default = 1)                 |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void DrawDashLine(PDRAWING pDrawing, HDC hdc, LTYPE* pLType, double x1, double y1, double x2, double y2, COLORREF rgbcolor, double Scale, double PatternStartPoint,
	double x, double y, double xScale, double yScale, double Rotation, long Object_Id, bool Pen_Down, short enttType)
{
	double x11, y11;
	double x12, y12;
	double m;
	double cosalpha;
	double l1, l2=0;
	double xend, yend;
	double MoveDirection = 1;
	int ElementNum;

	if(fabs(x2-x1)>fabs(y2-y1))
	{
		if(x1>x2)
			MoveDirection = -1;

		m = (y2-y1)/(x2-x1);
		cosalpha = 1 / sqrt(1+m*m);

		x11 = x1;//__max(x1,pDrawing->View.ViewLeft);
		y11 = y1 + m*(x11-x1);
		l1 = sqrt((x1-x11)*(x1-x11)+(y1-y11)*(y1-y11));
		l2 = fmod(PatternStartPoint + l1, pLType->PatternLength*Scale); 
		for(ElementNum=0; ElementNum<pLType->ElementsNumber; ElementNum++)
		{
			if(l2<fabs(pLType->Elements[ElementNum]*Scale))
				break;
			l2 = l2 - fabs(pLType->Elements[ElementNum]*Scale);
		}

		x12 = x11 + MoveDirection * (fabs(pLType->Elements[ElementNum]*Scale)-l2)*(cosalpha);
		y12 = y11 + (x12-x11)*(m);

		xend = x2;//__min(x2,pDrawing->View.ViewRight);
		while(MoveDirection*x12<MoveDirection*xend)
		{
			if(pLType->Elements[ElementNum]>=0)
//				DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x12), U2PY(y12) );
				DrawLine( pDrawing, hdc, rgbcolor, x11, y11, x12, y12, x, y, xScale, yScale, Rotation, Object_Id, Pen_Down, enttType);
	
			ElementNum++;
			if(ElementNum>=pLType->ElementsNumber)
				ElementNum=0;

			x11 = x12;
			y11 = y12;
			x12 = x11 + MoveDirection * fabs(pLType->Elements[ElementNum]*Scale)*(cosalpha);
			y12 = y11 + (x12-x11)*(m);
		}
		if(pLType->Elements[ElementNum]>=0)
//			DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x2), U2PY(y2) );
			DrawLine( pDrawing, hdc, rgbcolor, x11, y11, x2, y2, x, y, xScale, yScale, Rotation, Object_Id, Pen_Down, enttType);
	//*****************************************************************************************
	}
	else
	{
		if(y1>y2)
			MoveDirection = -1;

		m = (x2-x1)/(y2-y1);
		cosalpha = 1 / sqrt(1+m*m);

		y11 = y1;//__max(y1,pDrawing->View.WindowBottom);
		x11 = x1 + m*(y11-y1);
		l1 = sqrt((x1-x11)*(x1-x11)+(y1-y11)*(y1-y11));
		l2 = fmod(PatternStartPoint + l1, pLType->PatternLength*Scale); 
		for(ElementNum=0; ElementNum<pLType->ElementsNumber; ElementNum++)
		{
			if(l2<fabs(pLType->Elements[ElementNum]*Scale))
				break;
			l2 = l2 - fabs(pLType->Elements[ElementNum]*Scale);
		}

		y12 = y11 + MoveDirection * (fabs(pLType->Elements[ElementNum]*Scale)-l2)*(cosalpha);
		x12 = x11 + (y12-y11)*(m);

		yend = y2;//__min(y2,pDrawing->View.ViewTop);
		while(MoveDirection*y12<MoveDirection*yend)
		{
			if(pLType->Elements[ElementNum]>=0)
//				DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x12), U2PY(y12) );
				DrawLine( pDrawing, hdc, rgbcolor, x11, y11, x12, y12, x, y, xScale, yScale, Rotation, Object_Id, Pen_Down, enttType);
	
			ElementNum++;
			if(ElementNum>=pLType->ElementsNumber)
				ElementNum=0;

			y11 = y12;
			x11 = x12;
			y12 = y11 + MoveDirection * fabs(pLType->Elements[ElementNum]*Scale)*(cosalpha);
			x12 = x11 + (y12-y11)*(m);
		}
		if(pLType->Elements[ElementNum]>=0)
//			DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x2), U2PY(y2) );
			DrawLine( pDrawing, hdc, rgbcolor, x11, y11, x2, y2, x, y, xScale, yScale, Rotation, Object_Id, Pen_Down, enttType);
	}
}


/*-------------------------------------------------------------------*
 |  PlotEntity                                                       |
 |  Descriptions:                                                    |
 |      Plots an entity into given Drawing Context                   |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      LPVOID pEntityData = pointer to entity data structure        |
 |      double x = x position of insertion point (default = 0)       |
 |      double y = y position of insertion point (default = 0)       |
 |      double xScale = x scale factor (default = 1)                 |
 |      double yScale = y scale factor (default = 1)                 |
 |      double Rotation = rotation angle (default = 0)               |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void PlotEntity(PDRAWING pDrawing, HDC hdc, PENTITYHEADER pEntityHeader, LPVOID pEntityData, double x=0, double y=0, double xScale=1, double yScale=1, double Rotation=0)
{
	LAYER		Layer;
	LTYPE		LType;
	COLORREF	rgbcolor;
	short		color;
	HPEN		pen, oldpen;
	HBRUSH		brush, oldbrush;
	int			i;
	BOOL		isLTypeAvailable = FALSE;

	// Exit and do not plot, if this object is deleted
	if(pEntityHeader->Deleted)
		return;

	if(pEntityHeader->EntityType!=ENT_TEXT)
	{
		// Initialize LayerName & LineTypeName & Color
		color = pEntityHeader->Color;

		Layer.Objhandle = pEntityHeader->LayerObjhandle;
		if(drwFindTableType_Direct(pDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer)>0)
		{
			if((pEntityHeader->Color==COLOR_BYBLOCK) || (pEntityHeader->Color==COLOR_BYLAYER))
				color = Layer.Color;
			else if (Layer.Color<0)
			{
				// Layer is off
				return;
			}
		}
		else if(color>=COLOR_BYLAYER)
			color = 0;

		rgbcolor = CadColor[color];

		pen = CreatePen(PS_SOLID, 0, rgbcolor);
		oldpen = (HPEN)SelectObject(hdc, pen);
	
		LType.Objhandle = pEntityHeader->LTypeObjhandle;
		isLTypeAvailable = drwFindTableType_Direct(pDrawing, TAB_LTYPE, FIND_BYHANDLE, &LType);

		//--
		g_Object_Id++;
		g_Layer_Entt = Layer;
	}

	//--
	bool Pen_Down = true;

	// Plot entities data on DC
	switch(pEntityHeader->EntityType)
	{
	case ENT_LINE:
/*		if( !(((x1*xScale+x<pDrawing->View.ViewLeft) && (x2*xScale+x<pDrawing->View.ViewLeft)) || 
			((x1*xScale+x>pDrawing->View.ViewRight) && (x2*xScale+x>pDrawing->View.ViewRight)) || 
			((y1*yScale+y<pDrawing->View.ViewBottom) && (y2*yScale+y<pDrawing->View.ViewBottom)) || 
			((y1*yScale+y>pDrawing->View.ViewTop) && (y2*yScale+y>pDrawing->View.ViewTop))) )
*/		{
			double x1 = ((PENTLINE)pEntityData)->Point0.x;	
			double y1 = ((PENTLINE)pEntityData)->Point0.y;
			double x2 = ((PENTLINE)pEntityData)->Point1.x;
			double y2 = ((PENTLINE)pEntityData)->Point1.y;

			//--
			Pen_Down = true;

			if((LType.PatternLength==0) || (!isLTypeAvailable))
			{ // Solid Line
				DrawLine(pDrawing, hdc, rgbcolor, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LINE);
			}
			else
			{ // Line has Pattern
				double scale = pEntityHeader->LineTypeScale;

				double linelen = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
				double PatternStartPoint = fabs(LType.Elements[0]*scale)/2;
				double LinePatternStartPoint = fmod(linelen,LType.PatternLength*scale)/2;

				if((linelen<LType.PatternLength*scale) || (LType.PatternLength*scale*pDrawing->View.PPU*pDrawing->View.ZoomLevel<5))
				{
					DrawLine(pDrawing, hdc, rgbcolor, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LINE);
				}
				else
				{
					double m, cosalpha;
					double x11, y11;
					double x21, y21;

					if(fabs(x2-x1)>fabs(y2-y1))
					{
						if(x1>x2)
						{
							double dummy;
							dummy = x1; x1 = x2; x2 = dummy;
							dummy = y1; y1 = y2; y2 = dummy;
						}
						m = (y2-y1)/(x2-x1);
						cosalpha = 1 / sqrt(1+m*m);
						x11 = x1 + LinePatternStartPoint*cosalpha;
						y11 = y1 + (x11-x1)*m;
						x21 = x2 - LinePatternStartPoint*cosalpha;
						y21 = y2 + (x21-x2)*m;
					}
					else
					{
						if(y1>y2)
						{
							double dummy;
							dummy = x1; x1 = x2; x2 = dummy;
							dummy = y1; y1 = y2; y2 = dummy;
						}
						m = (x2-x1)/(y2-y1);
						cosalpha = 1 / sqrt(1+m*m);
						y11 = y1 + LinePatternStartPoint*cosalpha;
						x11 = x1 + (y11-y1)*m;
						y21 = y2 - LinePatternStartPoint*cosalpha;
						x21 = x2 + (y21-y2)*m;
					}

					DrawLine( pDrawing, hdc, rgbcolor, x1, y1, x11, y11, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LINE);
					DrawDashLine(pDrawing, hdc, &LType, x11, y11, x21, y21, rgbcolor, scale, PatternStartPoint, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LINE);
					DrawLine( pDrawing, hdc, rgbcolor, x21, y21, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LINE);
				}
			}
		}
		break;

	case ENT_POINT:
		SetPixel(
			hdc,
			U2PX(((PENTPOINT)pEntityData)->Point0.x),
			U2PY(((PENTPOINT)pEntityData)->Point0.y),
			rgbcolor
		);

		//-- 포인트 하나는 일단 false로 설정
		Pen_Down = false;

		//--
		pDrawing->View.Entt_Type = pEntityHeader->EntityType;
		addEntt_Point(pDrawing, ((PENTPOINT)pEntityData)->Point0.x, ((PENTPOINT)pEntityData)->Point0.y, g_Object_Id, Pen_Down);

		break;

	case ENT_CIRCLE:
		Pen_Down = true;
		double x1,y1,x2,y2,xc,yc,r;

		x1 = ((PENTCIRCLE)pEntityData)->Point0.x-((PENTCIRCLE)pEntityData)->Radius;
		y1 = ((PENTCIRCLE)pEntityData)->Point0.y-((PENTCIRCLE)pEntityData)->Radius;
		x2 = ((PENTCIRCLE)pEntityData)->Point0.x+((PENTCIRCLE)pEntityData)->Radius;
		y2 = ((PENTCIRCLE)pEntityData)->Point0.y+((PENTCIRCLE)pEntityData)->Radius;
		xc = ((PENTCIRCLE)pEntityData)->Point0.x;
		yc = ((PENTCIRCLE)pEntityData)->Point0.y;
		r = ((PENTCIRCLE)pEntityData)->Radius;

		if((LType.PatternLength==0) || (!isLTypeAvailable))
		{ 
			// Solid Line Circle
			//--
			x1 = ((PENTCIRCLE)pEntityData)->Point0.x - ((PENTCIRCLE)pEntityData)->Radius;
			y1 = ((PENTCIRCLE)pEntityData)->Point0.y;
			x2 = ((PENTCIRCLE)pEntityData)->Point0.x + ((PENTCIRCLE)pEntityData)->Radius;
			y2 = ((PENTCIRCLE)pEntityData)->Point0.y;

			//--
			double rotX, rotY;
			double radius = ((PENTCIRCLE)pEntityData)->Radius;

			//--
			rotX = ((PENTCIRCLE)pEntityData)->Point0.x;
			rotY = ((PENTCIRCLE)pEntityData)->Point0.y;

			//-- Draw
			pDrawing->View.Entt_Type = pEntityHeader->EntityType;
			DrawArcByBulge(pDrawing, hdc, rgbcolor, radius, radius, rotX, rotY, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_CIRCLE);
		}
		else
		{ // Circle has Pattern
			double scale = pEntityHeader->LineTypeScale;
			double primeter = 2*PI*r*xScale;

			if((primeter<LType.PatternLength*scale) || (LType.PatternLength*scale*pDrawing->View.PPU*pDrawing->View.ZoomLevel<5))
			{
				//--
				x1 = ((PENTCIRCLE)pEntityData)->Point0.x - ((PENTCIRCLE)pEntityData)->Radius;
				y1 = ((PENTCIRCLE)pEntityData)->Point0.y;
				x2 = ((PENTCIRCLE)pEntityData)->Point0.x + ((PENTCIRCLE)pEntityData)->Radius;
				y2 = ((PENTCIRCLE)pEntityData)->Point0.y;

				//--
				double rotX, rotY;
				double radius = ((PENTCIRCLE)pEntityData)->Radius;

				//--
				rotX = ((PENTCIRCLE)pEntityData)->Point0.x;
				rotY = ((PENTCIRCLE)pEntityData)->Point0.y;

				//-- Draw
				pDrawing->View.Entt_Type = pEntityHeader->EntityType;
				DrawArcByBulge(pDrawing, hdc, rgbcolor, radius, radius, rotX, rotY, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_CIRCLE);
			}
			else
			{
				double n, alpha;
				double x11, y11;
				double x21, y21;
				double PatternStartPoint=0;
				double len = 0;

				scale = primeter/ceil(primeter/(LType.PatternLength*scale))/LType.PatternLength;
				n = ceil((primeter*pDrawing->View.ZoomLevel*pDrawing->View.PPU) / 5);
				if(n<8)	n = 8;
				if(n>100)
					n=100;
				alpha = 2*PI/n;

				x21 = xc + r;
				y21 = yc;
				for(int i=1; i<=n; i++)
				{
					x11 = x21;
					y11 = y21;
					x21 = xc + r*cos(alpha*double(i));
					y21 = yc + r*sin(alpha*double(i));
					len = sqrt((x11-x21)*(x11-x21)+(y11-y21)*(y11-y21));
					DrawDashLine(pDrawing, hdc, &LType, x11, y11, x21, y21, rgbcolor, scale, PatternStartPoint, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_CIRCLE);
					PatternStartPoint = fmod(PatternStartPoint+len,LType.PatternLength*scale);
				}
			}
		}
		
		break;

	case ENT_ELLIPSE:
		double bulge, radius, rotX, rotY;
		double radiusX, radiusY, rd1, rd2;
		double ratio, startAngle, endAngle;

		Pen_Down = true;
		ratio = ((PENTELLIPSE)pEntityData)->MinorToMajorRatio;
		radiusX = ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x;
		radiusY = ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y;

		startAngle = ((PENTELLIPSE)pEntityData)->StartParam;
		endAngle = ((PENTELLIPSE)pEntityData)->EndParam;

		if (radiusX == 0)
		{
			//-- 세로
			rd1 = fabs(radiusY * ratio);
			rd2 = fabs(radiusY);
		}
		else
		{
			//-- 가로
			rd1 = fabs(radiusX);
			rd2 = fabs(radiusX * ratio);
		}

		//--
		if (radiusX == 0)
		{
			//-- 세로
			double u = (rd2 * 2);
			double i = rd1;
			
			bulge = i / (u / 2.0);
			x1 = ((PENTELLIPSE)pEntityData)->CenterPoint.x;
			y1 = ((PENTELLIPSE)pEntityData)->CenterPoint.y + rd2;
			x2 = ((PENTELLIPSE)pEntityData)->CenterPoint.x;
			y2 = ((PENTELLIPSE)pEntityData)->CenterPoint.y - rd2;
		}
		else
		{
			//-- 가로
			double u = (rd1 * 2);
			double i = rd2;

			bulge = i / (u / 2.0);
			x1 = ((PENTELLIPSE)pEntityData)->CenterPoint.x + rd1;
			y1 = ((PENTELLIPSE)pEntityData)->CenterPoint.y;
			x2 = ((PENTELLIPSE)pEntityData)->CenterPoint.x - rd1;
			y2 = ((PENTELLIPSE)pEntityData)->CenterPoint.y;
		}

		//--
		pDrawing->View.Entt_Type = pEntityHeader->EntityType;
		radius = Calcu_Radius_By_Bulge(bulge, x1, y1, x2, y2);
		Caluc_Rotate_Org(bulge, radius, x1, y1, x2, y2, &rotX, &rotY);
		DrawArcByBulge(pDrawing, hdc, rgbcolor, bulge, radius, rotX, rotY, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_ELLIPSE);

		bulge *= -1;
		radius = Calcu_Radius_By_Bulge(bulge, x1, y1, x2, y2);
		Caluc_Rotate_Org(bulge, radius, x1, y1, x2, y2, &rotX, &rotY);
		DrawArcByBulge(pDrawing, hdc, rgbcolor, bulge, radius, rotX, rotY, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_ELLIPSE);
		
		break;

	case ENT_TEXT:
		PlotText(
			pDrawing,
			hdc, 
			pEntityHeader,
			(PENTTEXT)pEntityData,
			x, y,
			xScale, yScale,
			Rotation
		);
		break;

	case ENT_ARC:
		//--
		Pen_Down = true;
		radius = ((PENTARC)pEntityData)->Radius;
		x1 = (((PENTARC)pEntityData)->Point0.x + ((PENTARC)pEntityData)->Radius * cos(((PENTARC)pEntityData)->StartAngle*PI / 180));
		y1 = (((PENTARC)pEntityData)->Point0.y + ((PENTARC)pEntityData)->Radius * sin(((PENTARC)pEntityData)->StartAngle*PI / 180));
		x2 = (((PENTARC)pEntityData)->Point0.x + ((PENTARC)pEntityData)->Radius * cos(((PENTARC)pEntityData)->EndAngle*PI / 180));
		y2 = (((PENTARC)pEntityData)->Point0.y + ((PENTARC)pEntityData)->Radius * sin(((PENTARC)pEntityData)->EndAngle*PI / 180));

		//-- 
		rotX = ((PENTARC)pEntityData)->Point0.x;
		rotY = ((PENTARC)pEntityData)->Point0.y;

		pDrawing->View.Entt_Type = pEntityHeader->EntityType;
		DrawArc(pDrawing, hdc, rgbcolor, rotX, rotY, radius, x1, y1, x2, y2, ((PENTARC)pEntityData)->StartAngle, ((PENTARC)pEntityData)->EndAngle, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_ARC);

		break;

	case ENT_SOLID:
		POINT	p[4];

		if(Rotation==0)
		{
			p[0].x = U2PX(((PENTSOLID)pEntityData)->Point0.x);
			p[0].y = U2PY(((PENTSOLID)pEntityData)->Point0.y);
			p[1].x = U2PX(((PENTSOLID)pEntityData)->Point1.x);
			p[1].y = U2PY(((PENTSOLID)pEntityData)->Point1.y);
			p[3].x = U2PX(((PENTSOLID)pEntityData)->Point2.x);
			p[3].y = U2PY(((PENTSOLID)pEntityData)->Point2.y);
			p[2].x = U2PX(((PENTSOLID)pEntityData)->Point3.x);
			p[2].y = U2PY(((PENTSOLID)pEntityData)->Point3.y);
		}
		else
		{
			p[0].x = U2PXROT(((PENTSOLID)pEntityData)->Point0.x, ((PENTSOLID)pEntityData)->Point0.y);
			p[0].y = U2PYROT(((PENTSOLID)pEntityData)->Point0.x, ((PENTSOLID)pEntityData)->Point0.y);
			p[1].x = U2PXROT(((PENTSOLID)pEntityData)->Point1.x, ((PENTSOLID)pEntityData)->Point1.y);
			p[1].y = U2PYROT(((PENTSOLID)pEntityData)->Point1.x, ((PENTSOLID)pEntityData)->Point1.y);
			p[3].x = U2PXROT(((PENTSOLID)pEntityData)->Point2.x, ((PENTSOLID)pEntityData)->Point2.y);
			p[3].y = U2PYROT(((PENTSOLID)pEntityData)->Point2.x, ((PENTSOLID)pEntityData)->Point2.y);
			p[2].x = U2PXROT(((PENTSOLID)pEntityData)->Point3.x, ((PENTSOLID)pEntityData)->Point3.y);
			p[2].y = U2PYROT(((PENTSOLID)pEntityData)->Point3.x, ((PENTSOLID)pEntityData)->Point3.y);
		}
		
		brush = CreateSolidBrush(rgbcolor);
		oldbrush = (HBRUSH)SelectObject(hdc, brush);
		Polygon(hdc, p, 4);
		SelectObject(hdc, oldbrush);
		DeleteObject(brush);

		//--
		pDrawing->View.Entt_Type = pEntityHeader->EntityType;
		addEntt_Solid(pDrawing, ((PENTSOLID)pEntityData), g_Object_Id);
		
		break;

	case ENT_INSERT:
		BLOCKHEADER		BlockHeader;
		ENTITYHEADER	BlockEntityHeader;
		char			BlockEntityData[4096];
		BlockHeader.Objhandle = ((PENTINSERT)pEntityData)->BlockHeaderObjhandle;
		if(drwFindBlock_Direct(pDrawing, FIND_BYHANDLE, &BlockHeader)>0)
		{
//			LAYER	BlockLayer;
//			Layer.Objhandle = BlockHeader.LayerObjhandle;
//			drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer);

			if(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_FIRST)>0)
			{
				do{
					if(BlockEntityHeader.Color==0)
						BlockEntityHeader.Color = color;

					PlotEntity(
						pDrawing,
						hdc,
						&BlockEntityHeader,
						&BlockEntityData,
						((PENTINSERT)pEntityData)->Point0.x*xScale+x,
						((PENTINSERT)pEntityData)->Point0.y*yScale+y,
						((PENTINSERT)pEntityData)->XScale*xScale,
						((PENTINSERT)pEntityData)->YScale*yScale,
						((PENTINSERT)pEntityData)->RotationAngle+Rotation
					);
				} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_NEXT)>0);
			}
		}
		break;

	case ENT_POLYLINE:
	case ENT_LWPOLYLINE:
		
		//--
		Pen_Down = true;

		//--
		for(i = 1; i < ((PENTPOLYLINE)pEntityData)->nVertex; i++)
		{
			//--
			if(int(((PENTPOLYLINE)pEntityData)->pVertex[i].Bulge*100) == 0)
			{
				//--
				x1 = ((PENTPOLYLINE)pEntityData)->pVertex[i - 1].Point.x;
				y1 = ((PENTPOLYLINE)pEntityData)->pVertex[i - 1].Point.y;
				x2 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.x;
				y2 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.y;

				DrawLine(pDrawing, hdc, rgbcolor, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LWPOLYLINE);
			}
			else
			{
				//--
				x1 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.x;
				y1 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.y;
				x2 = ((PENTPOLYLINE)pEntityData)->pVertex[i - 1].Point.x;
				y2 = ((PENTPOLYLINE)pEntityData)->pVertex[i - 1].Point.y;
				bulge = ((PENTPOLYLINE)pEntityData)->pVertex[i].Bulge;

				//--
				radius = Calcu_Radius_By_Bulge(bulge, x1, y1, x2, y2);

				//-- 원점 구하기
				Caluc_Rotate_Org(bulge, radius, x1, y1, x2, y2, &rotX, &rotY);

				//--
				pDrawing->View.Entt_Type = pEntityHeader->EntityType;
				DrawArcByBulge(pDrawing, hdc, rgbcolor, bulge, radius, rotX, rotY, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LWPOLYLINE);
			}
		}

		if(((PENTPOLYLINE)pEntityData)->Flag & POLYLINE_CLOSED)
		{
			//--
			int i = ((PENTPOLYLINE)pEntityData)->nVertex - 1;

			Pen_Down = true;
			x1 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.x;
			y1 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.y;
			x2 = ((PENTPOLYLINE)pEntityData)->pVertex[0].Point.x;
			y2 = ((PENTPOLYLINE)pEntityData)->pVertex[0].Point.y;

			//--
			DrawLine(pDrawing, hdc, rgbcolor, x1, y1, x2, y2, x, y, xScale, yScale, Rotation, g_Object_Id, Pen_Down, ENT_LINE);
		}
		break;

	case ENT_DIMENSION:
		BLOCKHEADER		DimBlockHeader;
		ENTITYHEADER	DimBlockEntityHeader;
		char			DimBlockEntityData[4096];
		DimBlockHeader.Objhandle = ((PENTDIMENSION)pEntityData)->BlockheaderObjhandle;
		if(drwFindBlock_Direct(pDrawing, FIND_BYHANDLE, &DimBlockHeader)>0)
		{
//			LAYER	BlockLayer;
//			Layer.Objhandle = BlockHeader.LayerObjhandle;
//			drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer);

			if(drwFindEntity_Direct(pDrawing, DimBlockHeader.Name, &DimBlockEntityHeader, &DimBlockEntityData, FIND_FIRST)>0)
			{
				do{
					if(DimBlockEntityHeader.Color==0)
						DimBlockEntityHeader.Color = color;

					PlotEntity(
						pDrawing,
						hdc,
						&DimBlockEntityHeader,
						&DimBlockEntityData,
						0+x,
						0+y,
						1*xScale,
						1*yScale,
						0+Rotation
					);
				} while(drwFindEntity_Direct(pDrawing, DimBlockHeader.Name, &DimBlockEntityHeader, &DimBlockEntityData, FIND_NEXT)>0);
			}
		}
		break;
	}

	//--
	DrawOrgCross(pDrawing, hdc, U2PXROT(-10, 0), U2PYROT(0, -10), U2PXROT(10, 0), U2PYROT(0, 10));

	if(pEntityHeader->EntityType!=ENT_TEXT)
	{
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
	}
}


/*-------------------------------------------------------------------*
 |  drwPlot                                                          |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      HDC hdc= handle of DC for drawing                            |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwPlot( HDRAWING hDrawing, HDC hdc, bool bRedraw)
{
	PDRAWING		pDrawing;
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	if(pDrawing->View.Viewable)
	{
		//-- Block Head - 2022.04.05 JIP -
		//-- for (long i = 0; i < pDrawing->BlocksNumber; i++)
		//-- {
		//-- 	BLOCKHEADER blockHD = pDrawing->Blocks[i];
		//-- 
		//-- 	//--
		//-- 	if (drwFindEntity_Direct(pDrawing, blockHD.Name, &EntityHeader, &EntityData, FIND_FIRST) > 0)
		//-- 	{
		//-- 		PlotEntity(pDrawing, hdc, &EntityHeader, &EntityData);
		//-- 	}
		//-- }

		//--
		pDrawing->View.bAddEntt = !bRedraw;

		//-- Init
		g_Object_Id = 0;

		// Entities Section ----------------------------------
		if(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_FIRST)>0)
		{
			//--
			do{
				PlotEntity(pDrawing, hdc, &EntityHeader, &EntityData);
			} while(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_NEXT) > 0);
		}
	}

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwInitView                                                      |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      int x = horizontal position of drawing window                |
 |      int y = vertical position of drawing window                  |
 |      int Width = width of drawing window                          |
 |      int Height = height of drawing window                        |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwInitView( HDRAWING hDrawing, int x, int y, int Width, int Height )
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return NULL;

	pDrawing->View.ZoomLevel = 1;
	pDrawing->View.PPU = 10;

	pDrawing->View.WindowLeft = x;
	pDrawing->View.WindowTop = y;
	pDrawing->View.WindowRight = x + Width;
	pDrawing->View.WindowBottom = y + Height;
	pDrawing->View.ViewLeft = 0;
	pDrawing->View.ViewBottom = 0;

	//-- By JIP
	pDrawing->View.Min_X = Width;
	pDrawing->View.Min_Y = Height;
	pDrawing->View.Max_X = 0;
	pDrawing->View.Max_Y = 0;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

//--
BOOL drwAdd_Selected_Points(HDRAWING hDrawing, RECT selRoi, double* Entt_Data_X, double* Entt_Data_Y, int* Entt_Data_Cnt)
{
	PDRAWING	pDrawing;
	HRGN		ViewRgn;
	BOOL		result = TRUE;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL)
		return FALSE;

	//--
	double line_pt_res = pDrawing->View.Line_Pt_Res;
	double* tmpEntt_X = new double[*Entt_Data_Cnt];
	double* tmpEntt_Y = new double[*Entt_Data_Cnt];

	//--
	memcpy(tmpEntt_X, Entt_Data_X, sizeof(double) * (*Entt_Data_Cnt));
	memcpy(tmpEntt_Y, Entt_Data_Y, sizeof(double) * (*Entt_Data_Cnt));

	//--
	int enttCnt = 0;
	double x = 0, y = 0, xScale = 1, yScale = 1, Rotation = 0;

	for (long i = 0; i < *Entt_Data_Cnt; i++)
	{
		//--
		int x1 = U2PXROT(tmpEntt_X[i], tmpEntt_Y[i]);
		int y1 = U2PYROT(tmpEntt_X[i], tmpEntt_Y[i]);

		//--
		if (selRoi.left <= x1 && selRoi.right >= x1
			&& selRoi.top <= y1 && selRoi.bottom >= y1)
		{
			//--
			if (enttCnt == 0)
			{
				//--
				Entt_Data_X[enttCnt] = tmpEntt_X[i];
				Entt_Data_Y[enttCnt] = tmpEntt_Y[i];
				enttCnt++;
			}
			//-- 중복 좌표 제거(소수점 3자리) - 2022.05.19 JIP -
			else if (fabs(Entt_Data_X[enttCnt - 1] - tmpEntt_X[i]) > 0.001
				|| fabs(Entt_Data_Y[enttCnt - 1] - tmpEntt_Y[i]) > 0.001)
			{
				//--
				Entt_Data_X[enttCnt] = tmpEntt_X[i];
				Entt_Data_Y[enttCnt] = tmpEntt_Y[i];
				enttCnt++;
			}
		}
	}

	//--
	*Entt_Data_Cnt = enttCnt;

	//--
	delete[] tmpEntt_X;
	delete[] tmpEntt_Y;

	// UnInitilize pDrawing -----------------
	UnInitilizePDRAWING(hDrawing);

	return result;
}

//--
BOOL drwDelete_Selected_Points(HDRAWING hDrawing, RECT selRoi, double* Entt_Data_X, double* Entt_Data_Y, int* Entt_Data_Cnt)
{
	PDRAWING	pDrawing;
	HRGN		ViewRgn;
	BOOL		result = TRUE;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL)
		return FALSE;

	//--
	double* tmpEntt_X = new double[*Entt_Data_Cnt];
	double* tmpEntt_Y = new double[*Entt_Data_Cnt];

	//--
	memcpy(tmpEntt_X, Entt_Data_X, sizeof(double) * (*Entt_Data_Cnt));
	memcpy(tmpEntt_Y, Entt_Data_Y, sizeof(double) * (*Entt_Data_Cnt));

	//--
	int enttCnt = 0;
	double x = 0, y = 0, xScale = 1, yScale = 1, Rotation = 0;

	for (long i = 0; i < *Entt_Data_Cnt; i++)
	{
		//--
		int x1 = U2PXROT(tmpEntt_X[i], tmpEntt_Y[i]);
		int y1 = U2PYROT(tmpEntt_X[i], tmpEntt_Y[i]);
		
		//--
		if (selRoi.left <= x1 && selRoi.right >= x1
			&& selRoi.top <= y1 && selRoi.bottom >= y1)
		{
			//--
			continue;
		}

		//--
		Entt_Data_X[enttCnt] = tmpEntt_X[i];
		Entt_Data_Y[enttCnt] = tmpEntt_Y[i];
		enttCnt++;
	}

	//--
	*Entt_Data_Cnt = enttCnt;

	// UnInitilize pDrawing -----------------
	UnInitilizePDRAWING(hDrawing);

	return result;
}

//--
BOOL drwDrawEnttSelectedData(HDRAWING hDrawing, HDC hdc, RECT selRoi, int* selIdx, double* ptX, double* ptY, int count, int size, COLORREF rgbcolor)
{
	PDRAWING	pDrawing;
	HRGN		ViewRgn;
	BOOL		result = TRUE;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL)
		return FALSE;

	HBRUSH brush = CreateSolidBrush(rgbcolor);
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);

	//--
	int idx = 0, preX, preY;
	double x = 0, y = 0, xScale = 1, yScale = 1, Rotation = 0;

	//--
	for (int i = 0; i < count; i++)
	{
		int x1 = U2PXROT(ptX[i], ptY[i]);
		int y1 = U2PYROT(ptX[i], ptY[i]);
		
		//--
		if (selRoi.left <= x1 && selRoi.right >= x1
			&& selRoi.top <= y1 && selRoi.bottom >= y1)
		{
			//--
			selIdx[idx++] = i;

			//--
			if (i > 0)
			{
				if (abs(preX - x1) < size && abs(preY - y1) < size)
				{
					continue;
				}
			}

			//--
			Ellipse(hdc, x1 - size, y1 - size, x1 + size, y1 + size);

			//--
			preX = x1;
			preY = y1;
		}
		else
		{
			preX = x1;
			preY = y1;
		}
	}

	SelectObject(hdc, oldbrush);
	DeleteObject(brush);

	// UnInitilize pDrawing -----------------
	UnInitilizePDRAWING(hDrawing);

	return result;
}

//--
BOOL drwDrawEnttData(HDRAWING hDrawing, HDC hdc, double* ptX, double* ptY, int count, int size, COLORREF rgbcolor)
{
	PDRAWING	pDrawing;
	HRGN		ViewRgn;
	BOOL		result = TRUE;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL)
		return FALSE;

	HBRUSH brush = CreateSolidBrush(rgbcolor);
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);

	//--
	double x = 0, y = 0, xScale = 1, yScale = 1, Rotation = 0;

	//--
	int preX, preY;

	//--
	for (int i = 0; i < count; i++)
	{
		int x1 = U2PXROT(ptX[i], ptY[i]);
		int y1 = U2PYROT(ptX[i], ptY[i]);
		
		if (x1 >= pDrawing->View.WindowLeft && x1 < pDrawing->View.WindowRight
			&& y1 >= pDrawing->View.WindowTop && y1 < pDrawing->View.WindowBottom)
		{
			if (i > 0)
			{
				if (abs(preX - x1) < size && abs(preY - y1) < size)
				{
					continue;
				}
			}

			//--
			Ellipse(hdc, x1 - size, y1 - size, x1 + size, y1 + size);

			preX = x1;
			preY = y1;
		}
		else
		{
			preX = x1;
			preY = y1;
		}
	}
	
	SelectObject(hdc, oldbrush);
	DeleteObject(brush);
	
	// UnInitilize pDrawing -----------------
	UnInitilizePDRAWING(hDrawing);

	return result;
}

/*-------------------------------------------------------------------*
 |  drwPaint                                                         |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |	    HDC hdc	= handle to view window DC                           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwPaint( HDRAWING hDrawing, HDC hdc, bool bRedraw)
{
	PDRAWING	pDrawing;
	HRGN		ViewRgn;
	BOOL		result;
	
	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	Rectangle(
		hdc,
		pDrawing->View.WindowLeft,
		pDrawing->View.WindowTop,
		pDrawing->View.WindowRight,
		pDrawing->View.WindowBottom
	);

	ViewRgn = CreateRectRgn(
					pDrawing->View.WindowLeft,
					pDrawing->View.WindowTop,
					pDrawing->View.WindowRight,
					pDrawing->View.WindowBottom
				);
	SelectObject(hdc, ViewRgn);

	SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
	result = drwPlot(hDrawing, hdc, bRedraw);
	DeleteObject(ViewRgn);

	// UnInitilize pDrawing -----------------
	UnInitilizePDRAWING(hDrawing);

	return result;
}

/********************************************************************
 * Functions for changing view properties                           *
 ********************************************************************/

/*-------------------------------------------------------------------*
 |  drwGetViewProperties                                             |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      PVIEW pView = pointer to view properties structure           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwGetViewProperties( HDRAWING hDrawing, PVIEW pView )
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	memcpy(pView, &pDrawing->View, sizeof(VIEW));

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}
BOOL drwGetEnttObjectCount(HDRAWING hDrawing, int* EnttCnt)
{
	PDRAWING pDrawing;
	*EnttCnt = 0;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL) return FALSE;
	if (pDrawing->View.pExtEnttList->size() == 0) return FALSE;

	*EnttCnt = pDrawing->View.pExtEnttList->size();

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}
void drwReleaseEnttObjects(DRW_ENTT_OBJECTS_COM* pEnttObj, int EnttCnt)
{
	for (int i = 0; i < EnttCnt; i++)
	{
		delete[] pEnttObj[i].Layer.Name;
		delete[] pEnttObj[i].pPen_Down;
		delete[] pEnttObj[i].pPoint_X;
		delete[] pEnttObj[i].pPoint_Y;
	}
}
BOOL drwGetEnttObjects(HDRAWING hDrawing, DRW_ENTT_OBJECTS_COM* pEnttObj)
{
	PDRAWING pDrawing;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL) return FALSE;
	if (pDrawing->View.pExtEnttList->size() == 0) return FALSE;

	//--
	int index = 0;
	std::vector<DRW_ENTT_LSIT>::const_iterator itrEnttList;
	for (itrEnttList = pDrawing->View.pExtEnttList->begin(); itrEnttList != pDrawing->View.pExtEnttList->end(); itrEnttList++)
	{
		DRW_ENTT_LSIT DrwEntt = *itrEnttList;

		//
		pEnttObj[index].Entt_Type = DrwEntt.Entt_Type;
		pEnttObj[index].Object_Id = DrwEntt.Object_Id;
		pEnttObj[index].Line_Pt_Res = DrwEntt.Line_Pt_Res;

		//copy layer
		//memcpy(&pEnttObj[index].Layer, &DrwEntt.Layer, sizeof(LAYER));
		//-- copy Layer
		pEnttObj[index].Layer.Objhandle = DrwEntt.Layer.Objhandle;

		//-- copy layer name
		pEnttObj[index].Layer.Name = new char[MAX_STRLEN];
		memset(pEnttObj[index].Layer.Name, 0x00, sizeof(char) * MAX_STRLEN);
		strncpy(pEnttObj[index].Layer.Name, DrwEntt.Layer.Name, strlen(DrwEntt.Layer.Name));

		pEnttObj[index].Layer.StandardFlags = DrwEntt.Layer.StandardFlags;
		pEnttObj[index].Layer.Color = DrwEntt.Layer.Color;
		pEnttObj[index].Layer.LineTypeObjhandle = DrwEntt.Layer.LineTypeObjhandle;
		pEnttObj[index].Layer.LineWeight = DrwEntt.Layer.LineWeight;
		pEnttObj[index].Layer.PlotFlag = DrwEntt.Layer.PlotFlag;
		pEnttObj[index].Layer.PlotStyleObjhandle = DrwEntt.Layer.PlotStyleObjhandle;

		//copy vector
		pEnttObj[index].Pen_Down_Cnt = 0;
		pEnttObj[index].pPen_Down = new bool[DrwEntt.Pen_Down.size()];
		std::vector<bool>::const_iterator itrPenDown;
		for (itrPenDown = DrwEntt.Pen_Down.begin(); itrPenDown != DrwEntt.Pen_Down.end(); itrPenDown++)
		{
			pEnttObj[index].pPen_Down[pEnttObj[index].Pen_Down_Cnt] = *itrPenDown;
			pEnttObj[index].Pen_Down_Cnt++;
		}

		pEnttObj[index].Point_X_Cnt = 0;
		pEnttObj[index].pPoint_X = new double[DrwEntt.Point_X.size()];

		std::vector<double>::const_iterator itrPoint_X;
		for (itrPoint_X = DrwEntt.Point_X.begin(); itrPoint_X != DrwEntt.Point_X.end(); itrPoint_X++)
		{
			pEnttObj[index].pPoint_X[pEnttObj[index].Point_X_Cnt] = *itrPoint_X;
			pEnttObj[index].Point_X_Cnt++;
		}

		pEnttObj[index].Point_Y_Cnt = 0;
		pEnttObj[index].pPoint_Y = new double[DrwEntt.Point_Y.size()];
		std::vector<double>::const_iterator itrPoint_Y;
		for (itrPoint_Y = DrwEntt.Point_Y.begin(); itrPoint_Y != DrwEntt.Point_Y.end(); itrPoint_Y++)
		{
			pEnttObj[index].pPoint_Y[pEnttObj[index].Point_Y_Cnt] = *itrPoint_Y;
			pEnttObj[index].Point_Y_Cnt++;
		}

		//--
		index++;
	}

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

BOOL drwGetDrawingObject(HDRAWING hDrawing, void* pDrwObj)
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if ((pDrawing = InitilizePDRAWING(hDrawing)) == NULL)
		return FALSE;

	memcpy(pDrwObj, pDrawing, sizeof(DRAWING));

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwSetViewProperties                                             |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      PVIEW pView = pointer to view properties structure           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwSetViewProperties( HDRAWING hDrawing, PVIEW pView )
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	//-- C#에서 호출시... - 2024.02.23 JIP -
	if (pView->pExtEnttList == NULL)
	{
		pView->pExtEnttList = new std::vector<DRW_ENTT_LSIT>();
	}

	memcpy(&pDrawing->View, pView, sizeof(VIEW));

	pDrawing->View.ViewTop = pDrawing->View.ViewBottom + (pDrawing->View.WindowBottom-pDrawing->View.WindowTop)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;
	pDrawing->View.ViewRight = pDrawing->View.ViewLeft + (pDrawing->View.WindowRight-pDrawing->View.WindowLeft)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/********************************************************************
 * Zoom Functions                                                   *
 ********************************************************************/

/*-------------------------------------------------------------------*
 |  drwGetDrawingBorder                                              |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      PREALRECT pRect = pointer to drawing boundary rect           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwGetDrawingBorder( HDRAWING hDrawing, PREALRECT pRect )
{
	PDRAWING		pDrawing;
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];
	REALRECT		Rect;
	double			x1 = DBL_MAX;
	double			y1 = DBL_MIN;
	double			x2 = DBL_MIN;
	double			y2 = DBL_MAX;

	//-- Init
	Rect.left = x1;
	Rect.right = x2;
	Rect.top = y1;
	Rect.bottom = y2;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return NULL;

//	if(pDrawing->View.Viewable)
	{
		// Entities Section ----------------------------------
		if(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_FIRST)>0)
		{
			do{
				if(GetEntityRect(pDrawing, &EntityHeader, EntityData, &Rect))
				{
					y1 = __max(Rect.top, y1);
					x1 = __min(Rect.left, x1);
					y2 = __min(Rect.bottom, y2);
					x2 = __max(Rect.right, x2);
				}
			} while(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_NEXT)>0);
		}
	}
	
	pRect->left = x1;
	pRect->right = x2;
	pRect->bottom = y2;
	pRect->top = y1;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwZoomExtents                                                   |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwZoomExtents( HDRAWING hDrawing )
{
	PDRAWING		pDrawing;
	REALRECT		rect;
	double			ZoomLevelX, ZoomLevelY;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return NULL;

	drwGetDrawingBorder(hDrawing, &rect);

	// Change Drawing View Properties
	ZoomLevelX = (pDrawing->View.WindowBottom-pDrawing->View.WindowTop-20)/((rect.top-rect.bottom)*pDrawing->View.PPU);
	ZoomLevelY = (pDrawing->View.WindowRight-pDrawing->View.WindowLeft-20)/((rect.right-rect.left)*pDrawing->View.PPU);
	pDrawing->View.ZoomLevel = (ZoomLevelX < ZoomLevelY ? ZoomLevelX : ZoomLevelY);

	pDrawing->View.ViewBottom = rect.bottom - ((pDrawing->View.WindowBottom-pDrawing->View.WindowTop)/(pDrawing->View.ZoomLevel*pDrawing->View.PPU) - (rect.top-rect.bottom))/2;
	pDrawing->View.ViewLeft = rect.left - ((pDrawing->View.WindowRight-pDrawing->View.WindowLeft)/(pDrawing->View.ZoomLevel*pDrawing->View.PPU) - (rect.right-rect.left))/2;

	pDrawing->View.ViewTop = pDrawing->View.ViewBottom + (pDrawing->View.WindowBottom-pDrawing->View.WindowTop)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;
	pDrawing->View.ViewRight = pDrawing->View.ViewLeft + (pDrawing->View.WindowRight-pDrawing->View.WindowLeft)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}
