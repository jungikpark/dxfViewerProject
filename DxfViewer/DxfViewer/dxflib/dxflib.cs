using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace DxfViewer.DxfViewer.dxflib
{
    public static class dxf_Lib
    {
        //--
        private static IntPtr m_hDrawing = IntPtr.Zero;
        
        // Construction & Destruction
        [DllImport("CadIO")]
        private static extern IntPtr drwCreate();
        [DllImport("CadIO")]
        private static extern bool drwDestroy(IntPtr hDrawing);

        // Datafile dataexchange
        [DllImport("CadIO")]
        private static extern bool drwLoadDataFromFile(IntPtr hDrawing, System.UInt32 BlockObjhandle, System.UInt32 Reserved, IntPtr strFileName, IntPtr hWndProgress);

        // Drawing Window View
        [DllImport("CadIO")]
        private static extern bool drwInitView(IntPtr hDrawing, int x, int y, int nWidth, int nHeight);
        [DllImport("CadIO")]
        private static extern bool drwGetViewProperties(IntPtr hDrawing, IntPtr pView);
        [DllImport("CadIO")]
        private static extern bool drwSetViewProperties(IntPtr hDrawing, IntPtr pView);
        [DllImport("CadIO")]
        private static extern bool drwZoomExtents(IntPtr hDrawing);
        [DllImport("CadIO")]
        private static extern bool drwPaint(IntPtr hDrawing, System.Int32 hDC, bool bRedraw);
        [DllImport("CadIO")]
        private static extern bool drwGetEnttObjectCount(IntPtr hDrawing, out int enttCount);
        [DllImport("CadIO")]
        private static extern bool drwGetEnttObjects(IntPtr hDrawing, IntPtr pEnttObj);
        [DllImport("CadIO")]
        private static extern bool drwReleaseEnttObjects(IntPtr pEnttObj, int enttCount);



        public static bool Create()
        {
            Destroy();

            //--
            m_hDrawing = drwCreate();
            if (m_hDrawing != IntPtr.Zero)
            {
                tag_ENTITYHEADER EntityHeader = new tag_ENTITYHEADER();
                EntityHeader.LayerObjhandle = 0;// Layer 0
                EntityHeader.LTypeObjhandle = 0;// ByLayer
                EntityHeader.Color = 256;                // ByLayer
                EntityHeader.Thickness = 0;
                EntityHeader.LineTypeScale = 1.0;
                return true;
            }

            return false;
        }
        public static bool Destroy()
        {
            if (m_hDrawing == IntPtr.Zero) return false;
            return drwDestroy(m_hDrawing);
        }
        public static bool isReady()
        {
            if (m_hDrawing == IntPtr.Zero) return false;
            return true;
        }
        public static bool LoadDXFFile(String FileName)
        {
            if (m_hDrawing == IntPtr.Zero) return false;
            return drwLoadDataFromFile(m_hDrawing, 0, 0, GetStringToIntPtr(FileName), IntPtr.Zero);
        }
        public static bool InitView(int x, int y, int nWidth, int nHeight)
        {
            if (m_hDrawing == IntPtr.Zero) return false;
            return drwInitView(m_hDrawing, x, y, nWidth, nHeight);
        }
        private static IntPtr AllocHGlobal_View()
        {
            tag_VIEW view = new tag_VIEW();
            int bufferSize = Marshal.SizeOf(view);
            IntPtr pView = Marshal.AllocHGlobal(bufferSize);
            Marshal.StructureToPtr(view, pView, false);
            return pView;
        }
        public static bool Set_PPU(double ppu)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                view.PPU = ppu;
                bRet = SetViewProperties(view);
            }

            return bRet;
        }
        public static bool Set_Arc_Pt_Res(double arc_pt_Res)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                view.Arc_Pt_Res = arc_pt_Res;
                bRet = SetViewProperties(view);
            }

            return bRet;
        }
        public static bool Set_Line_Pt_Res(double line_pt_Res)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                view.Line_Pt_Res = line_pt_Res;
                bRet = SetViewProperties(view);
            }

            return bRet;
        }
        public static bool Convert_UnitToScreen(double unitX, double unitY, ref double srcX, ref double srcY)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                bRet = true;
                srcX = view.WindowLeft + ((-view.ViewLeft + unitX) * view.PPU * view.ZoomLevel);
                srcY = view.WindowBottom - ((-view.ViewBottom + unitY) * view.PPU * view.ZoomLevel);
            }

            return bRet;
        }
        public static bool Convert_UnitToScreen(double unitX, double unitY, ref int srcX, ref int srcY)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                bRet = true;
                srcX = view.WindowLeft + (int)((-view.ViewLeft + unitX) * view.PPU * view.ZoomLevel);
                srcY = view.WindowBottom - (int)((-view.ViewBottom + unitY) * view.PPU * view.ZoomLevel);
            }

            return bRet;
        }
        public static bool Convert_ScreenToUnit(double scrX, double scrY, ref double unitX, ref double unitY)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                bRet = true;
                unitX = (scrX - view.WindowLeft) / (view.PPU * view.ZoomLevel) - (-view.ViewLeft);
                unitY = (view.WindowBottom - scrY) / (view.PPU * view.ZoomLevel) - (-view.ViewBottom);
            }

            return bRet;
        }
        public static bool Convert_ScreenToUnit(int scrX, int scrY, ref double unitX, ref double unitY)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                bRet = true;
                unitX = (scrX - view.WindowLeft) / (view.PPU * view.ZoomLevel) - (-view.ViewLeft);
                unitY = (view.WindowBottom - scrY) / (view.PPU * view.ZoomLevel) - (-view.ViewBottom);
            }

            return bRet;
        }
        public static double GetZoomLevel()
        {
            double Zoom = 0;
            if (m_hDrawing == IntPtr.Zero) return Zoom;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                Zoom = view.ZoomLevel;
            }

            return Zoom;
        }
        public static void Span_Dxf_Drawing(int Span_X, int Span_Y)
        {
            if (m_hDrawing == IntPtr.Zero) return;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == false) return;

            double radio = view.ZoomLevel * view.PPU;
            double dSpanX = Span_X / radio;
            double dSpanY = Span_Y / radio;

            view.ViewLeft = view.ViewLeft + dSpanX;
            view.ViewRight = view.ViewRight + dSpanX;

            view.ViewTop = view.ViewTop + dSpanY;
            view.ViewBottom = view.ViewBottom + dSpanY;

            if (SetViewProperties(view) == false) return;
        }
        public static bool GetViewProperties(ref tag_VIEW view)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            IntPtr pView = AllocHGlobal_View();
            if (drwGetViewProperties(m_hDrawing, pView) == true)
            {
                bRet = true;
                view = (tag_VIEW)Marshal.PtrToStructure(pView, typeof(tag_VIEW));
            }

            Marshal.FreeHGlobal(pView);
            return bRet;
        }
        public static bool SetZoomLevel(double ZoomLevel)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            tag_VIEW view = new tag_VIEW();
            if (GetViewProperties(ref view) == true)
            {
                double cx;
                double cy;
                cx = view.ViewLeft + ((view.WindowRight - view.WindowLeft) / 2) * (1 / (view.PPU * view.ZoomLevel));
                cy = view.ViewBottom + ((view.WindowBottom - view.WindowTop) / 2) * (1 / (view.PPU * view.ZoomLevel));

                view.ZoomLevel = ZoomLevel;
                view.ViewLeft = cx - ((view.WindowRight - view.WindowLeft) / 2) * (1 / (view.PPU * view.ZoomLevel));
                view.ViewBottom = cy - ((view.WindowBottom - view.WindowTop) / 2) * (1 / (view.PPU * view.ZoomLevel));

                bRet = SetViewProperties(view);
            }

            return bRet;
        }
        public static bool SetViewProperties(tag_VIEW view)
        {
            bool bRet = false;
            if (m_hDrawing == IntPtr.Zero) return false;

            IntPtr pView = AllocHGlobal_View();
            Marshal.StructureToPtr(view, pView, false);
            bRet = drwSetViewProperties(m_hDrawing, pView);

            Marshal.FreeHGlobal(pView);
            return bRet;
        }

        public static bool GetEnttObjectList(ref List<ENTT_OBJECTS> enttObjects)
        {
            //--
            if(enttObjects == null)
            {
                enttObjects = new List<ENTT_OBJECTS>();
            }
            else
            {
                enttObjects.Clear();
                GC.Collect();
            }

            //--
            int objCnt = 0;
            if (drwGetEnttObjectCount(m_hDrawing, out objCnt) == false) return false;

            tag_DRW_ENTT_OBJECTS tmpEntt = new tag_DRW_ENTT_OBJECTS();
            int bufferSize = Marshal.SizeOf(tmpEntt) * objCnt;
            IntPtr pEntt = Marshal.AllocHGlobal(bufferSize);
            Marshal.StructureToPtr(tmpEntt, pEntt, false);
            IntPtr pEnttRelease = pEntt;

            //--
            if (drwGetEnttObjects(m_hDrawing, pEntt) == false) return false;

            //--
            for (int i = 0; i < objCnt; i++)
            {
                //-- Alloc
                byte[] Name = new byte[512];
                ENTT_OBJECTS EnttObjects = new ENTT_OBJECTS();
            
                //-- Set Values
                tag_DRW_ENTT_OBJECTS enttObj = (tag_DRW_ENTT_OBJECTS)Marshal.PtrToStructure(pEntt, typeof(tag_DRW_ENTT_OBJECTS));
                pEntt += Marshal.SizeOf(typeof(tag_DRW_ENTT_OBJECTS));
            
                //-- Set Layer
                EnttObjects.Layer.Objhandle = enttObj.Layer.Objhandle;
                Marshal.Copy(enttObj.Layer.Name, Name, 0, 512);
                EnttObjects.Layer.Name = Encoding.Default.GetString(Name);
                EnttObjects.Layer.StandardFlags = enttObj.Layer.StandardFlags;
                EnttObjects.Layer.Color = enttObj.Layer.Color;
                EnttObjects.Layer.LineTypeObjhandle = enttObj.Layer.LineTypeObjhandle;
                EnttObjects.Layer.LineWeight = enttObj.Layer.LineWeight;
                EnttObjects.Layer.PlotFlag = enttObj.Layer.PlotFlag;
                EnttObjects.Layer.PlotStyleObjhandle = enttObj.Layer.PlotStyleObjhandle;
            
                //-- Set EnttObject
                EnttObjects.Entt_Type = enttObj.Entt_Type;
                EnttObjects.Object_Id = enttObj.Object_Id;
                EnttObjects.Line_Pt_Res = enttObj.Line_Pt_Res;
                EnttObjects.Pen_Down_Cnt = enttObj.Pen_Down_Cnt;
                EnttObjects.Point_X_Cnt = enttObj.Point_X_Cnt;
                EnttObjects.Point_Y_Cnt = enttObj.Point_Y_Cnt;
            
                EnttObjects.pPen_Down = new byte[enttObj.Pen_Down_Cnt];
                Marshal.Copy(enttObj.pPen_Down, EnttObjects.pPen_Down, 0, enttObj.Pen_Down_Cnt);
            
                EnttObjects.pPoint_X = new double[enttObj.Point_X_Cnt];
                Marshal.Copy(enttObj.pPoint_X, EnttObjects.pPoint_X, 0, enttObj.Point_X_Cnt);
            
                EnttObjects.pPoint_Y = new double[enttObj.Point_Y_Cnt];
                Marshal.Copy(enttObj.pPoint_Y, EnttObjects.pPoint_Y, 0, enttObj.Point_Y_Cnt);
            
                //-- Add to the list
                enttObjects.Add(EnttObjects);
            }

            //-- Release Memory
            drwReleaseEnttObjects(pEnttRelease, objCnt);
            Marshal.FreeHGlobal(pEnttRelease);

            return true;
        }
        public static bool Paint(Panel pnlDrawing, bool bRedraw)
        {
            if (m_hDrawing == IntPtr.Zero) return false;

            Graphics gp = Graphics.FromHwnd(pnlDrawing.Handle);
            return drwPaint(m_hDrawing, gp.GetHdc().ToInt32(), bRedraw);
        }
        public static bool ZoomExtents()
        {
            if (m_hDrawing == IntPtr.Zero) return false;
            return drwZoomExtents(m_hDrawing);
        }
        public static IntPtr GetStringToIntPtr(String strData)
        {
            return Marshal.StringToHGlobalUni(strData);
        }
    }
}
