using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Numerics;

namespace DxfViewer.DxfViewer.dxflib
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct tag_ENTITYHEADER
    {
	    public ushort EntityType;          // Entity type
        public System.UInt32 Objhandle;                // Handle 
        public System.UInt32 LayerObjhandle;           // Used Layer's Handle
        public System.UInt32 LTypeObjhandle;           // Used LineType's Handle
        public int Deleted;               // 0 = Not Deleted
        public short Color;                    // Color (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
        public double Thickness;               // Thickness (default=0)
        public double LineTypeScale;           // Linetype scale (default=1.0)
        public fixed double ExtrusionDirection[3];   // Extrusion direction. (default = 0, 0, 1)
        public short LineWeight;               // Lineweight enum value (*2000*)
        public System.Int32 PreviousEntityPos;
        public System.Int32 NextEntityPos;
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct tag_VIEW
    {
        public int Viewable;      // TRUE = Drawing can be viewed

        // -- BY JIP --
        public int Min_X;
        public int Min_Y;
        public int Max_X;
        public int Max_Y;
        public int Entt_Type;
        public bool bAddEntt;
        public double Arc_Pt_Res;
        public double Line_Pt_Res;

        public int WindowLeft;     // Drawing Window Boundary Properties (In Pixels)
        public int WindowTop;      // Drawing Window Boundary Properties (In Pixels)
        public int WindowRight;    // Drawing Window Boundary Properties (In Pixels)
        public int WindowBottom;   // Drawing Window Boundary Properties (In Pixels)
        public double ViewLeft;        // Drawing View Properties (In Units)
        public double ViewBottom;      // Drawing View Properties (In Units)
        public double ViewRight;       // Drawing View Properties (In Units)
        public double ViewTop;     // Drawing View Properties (In Units)
        public double PPU;         // Pixels Per Unit
        public double ZoomLevel;       // Zoom Level

        /*
         * dll <-> app. Data 교환시 memory 접근에 문제 생겨서
         * pEnttList 형을 vector<DRW_ENTT_LSIT>에서 void* 형으로 선언 - JIP -
         */
        public IntPtr pEnttList;

        /*
         * pEnttList 사용시 내부에서 메모리 생성후 해제시 간헐적으로 memory access violation 발생
         * 외부 컨테이너 사용하여 테스트
         */
        public IntPtr pExtEnttList;
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct tag_LAYER
    {
        public System.UInt32 Objhandle;            // Handle
        //public fixed char Name[256];      // Layer Name
        public IntPtr Name;      // Layer Name
        public char StandardFlags;         // Standard flags
        public short Color;                    // Layer Color (if negative, layer is Off)
        public System.UInt32 LineTypeObjhandle;    // Handle of linetype for this layer
        public short LineWeight;               // Layer's Line Weight                      (*2000*)
        public int PlotFlag;              // TRUE=Plot this layer                     (*2000*)
        public System.UInt32 PlotStyleObjhandle;   // handle of PlotStyleName object           (*2000*)
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct tag_DRW_ENTT_LSIT
    {
        public int Entt_Type;
        public tag_LAYER Layer;
        public int Object_Id;
        public double Line_Pt_Res;

        public IntPtr Pen_Down;
        public IntPtr Point_X;
        public IntPtr Point_Y;
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct tag_DRW_ENTT_OBJECTS
    {
        public int Entt_Type;
        public int Object_Id;
        public double Line_Pt_Res;

        public int Pen_Down_Cnt;
        public int Point_X_Cnt;
        public int Point_Y_Cnt;

        public IntPtr pPen_Down;
        public IntPtr pPoint_X;
        public IntPtr pPoint_Y;

        public tag_LAYER Layer;
    }

    /////////////////////////////////////////////////////////////////////////
    ////////// .Net 변환 자료형
    ////////////////////////////////////////////////////////////////////////////
    public unsafe struct ENTT_LAYER
    {
        public System.UInt32 Objhandle;            // Handle
        public string Name;      // Layer Name
        public char StandardFlags;         // Standard flags
        public short Color;                    // Layer Color (if negative, layer is Off)
        public System.UInt32 LineTypeObjhandle;    // Handle of linetype for this layer
        public short LineWeight;               // Layer's Line Weight                      (*2000*)
        public int PlotFlag;              // TRUE=Plot this layer                     (*2000*)
        public System.UInt32 PlotStyleObjhandle;   // handle of PlotStyleName object           (*2000*)
    }
    public struct ENTT_OBJECTS
    {
        public int Entt_Type;
        public int Object_Id;
        public double Line_Pt_Res;

        public int Pen_Down_Cnt;
        public int Point_X_Cnt;
        public int Point_Y_Cnt;

        public byte[] pPen_Down;
        public double[] pPoint_X;
        public double[] pPoint_Y;

        public ENTT_LAYER Layer;
    }


    public enum TagType
    {
        tagLayer,
        tagObject,
        tagPoint
    }
    public class Object_Points
    {
        private int Object_Id = -1;
        private List<bool> Pen_Down = new List<bool>();
        private List<double> Point_X = new List<double>();
        private List<double> Point_Y = new List<double>();

        private void Add_Pen_Down(byte[] pPen_Down)
        {
            for (int i = 0; i < pPen_Down.Length; i++)
            {
                bool bPenDown = false;
                if (pPen_Down[i] == 1) bPenDown = true;
                Pen_Down.Add(bPenDown);
            }
        }
        private void Add_Point_X(double[] pPoint_X)
        {
            for (int i = 0; i < pPoint_X.Length; i++)
            {
                Point_X.Add(pPoint_X[i]);
            }
        }
        private void Add_Point_Y(double[] pPoint_Y)
        {
            for (int i = 0; i < pPoint_Y.Length; i++)
            {
                Point_Y.Add(pPoint_Y[i]);
            }
        }

        public void Clear()
        {
            Object_Id = -1;
            Pen_Down.Clear();
            Point_X.Clear();
            Point_Y.Clear();
        }
        public void Add_Object_Points(ENTT_OBJECTS enttObject)
        {
            if(Object_Id == -1)
            {
                Object_Id = enttObject.Object_Id;
            }
            if (Object_Id == enttObject.Object_Id)
            {
                Add_Pen_Down(enttObject.pPen_Down);
                Add_Point_X(enttObject.pPoint_X);
                Add_Point_Y(enttObject.pPoint_Y);
            }
        }
        public int ObjectID => Object_Id;
        public int PointX_Count => Point_X.Count;
        public int PointY_Count => Point_Y.Count;
        public int PenDown_Count => Pen_Down.Count;
        public List<bool> PenDown_List => Pen_Down;
        public List<double> PointX_List => Point_X;
        public List<double> PointY_List => Point_Y;
    }
    public class Layer
    {
        private string Name = string.Empty;
        private List<Object_Points> Layer_Objects = new List<Object_Points>();

        public bool isSameLayer(string layerName)
        {
            if (Name != layerName) return false;
            return true;
        }
        public void Clear()
        {
            for (int i = 0; i < Layer_Objects.Count; i++)
            {
                Layer_Objects[i].Clear();
            }

            Layer_Objects.Clear();
        }
        public bool Create_Layer(ENTT_OBJECTS enttObject)
        {
            if (Name.Length > 0) return false;//이미 설정되어 있음

            //--
            Name = enttObject.Layer.Name;
            Add_enttObject(enttObject);
            return true;
        }
        public void Add_enttObject(ENTT_OBJECTS enttObject)
        {
            for (int i = 0; i < Layer_Objects.Count; i++)
            {
                if(Layer_Objects[i].ObjectID == enttObject.Object_Id)
                {
                    Layer_Objects[i].Add_Object_Points(enttObject);
                    return;
                }
            }

            //--
            Object_Points objPoint = new Object_Points();
            objPoint.Add_Object_Points(enttObject);
            Layer_Objects.Add(objPoint);
        }
        public string Layer_Name => Name;
        public int Object_Count => Layer_Objects.Count;
        public List<Object_Points> ObjectList => Layer_Objects;
    }
    public class Entt_Layer_Objects
    {
        private List<Layer> Layer_List = new List<Layer>();

        public void Add_Entt_Object(ENTT_OBJECTS enttObject)
        {
            //-- 등록된 Layer가 있는지 비교
            for (int i = 0; i < Layer_List.Count; i++)
            {
                if (Layer_List[i].isSameLayer(enttObject.Layer.Name) == true)
                {
                    //-- Add
                    Layer_List[i].Add_enttObject(enttObject);
                    return;
                }
            }

            //-- Add
            Layer layer = new Layer();
            if (layer.Create_Layer(enttObject) == true)
            {
                Layer_List.Add(layer);
            }
        }
        public void Clear_Layer_Objects()
        {
            for (int i = 0; i < Layer_List.Count; i++)
            {
                Layer_List[i].Clear();
            }

            Layer_List.Clear();
        }
        public int Layer_Count => Layer_List.Count;

        public bool Check_Index(int idx)
        {
            if (idx < 0) return false;
            if (Layer_List.Count == 0) return false;
            if (Layer_List.Count <= idx) return false;
            return true;
        }
        public string Get_Layer_Name(int idx)
        {
            if (Check_Index(idx) == false) return "";
            return Layer_List[idx].Layer_Name;
        }
        public int Get_Obects_Count_In_Layer(int idx)
        {
            if (Check_Index(idx) == false) return 0;
            return Layer_List[idx].Object_Count;
        }
        public List<Object_Points> Get_ObectList_In_Layer(int idx)
        {
            if (Check_Index(idx) == false) return null;
            return Layer_List[idx].ObjectList;
        }
    }
}
