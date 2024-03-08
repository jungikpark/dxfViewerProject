using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using DxfViewer.DxfViewer.dxflib;

namespace DxfViewer.DxfViewer.dxfDrawing
{
    public partial class dxfDraw : UserControl
    {
        private bool bLoad_Dxf = false;
        private bool bLBtnDown = false;
        private bool bRBtnDown = false;
        private int LDown_X = 0;
        private int LDown_Y = 0;
        private int RDown_X = 0;
        private int RDown_Y = 0;
        private const double maxZoom_Lvl = 200;

        public List<ENTT_OBJECTS> Entt_List;
        public tag_VIEW orgViewProp;
        public Entt_Layer_Objects layerObject;

        public dxfDraw()
        {
            InitializeComponent();

            //--
            orgViewProp = new tag_VIEW();
            layerObject = new Entt_Layer_Objects();
        }
        ~dxfDraw()
        {
            dxf_Lib.Destroy();
        }
        public void Set_Parents_Location(ControlCollection parentsCtrl, Point Position)
        {
            //--
            this.Location = Position;
            parentsCtrl.Add(this);
        }
        public bool Open_Dxf_File(string fileName, double lineGap = 1, double arcGap = 1)
        {
            //--
            bLoad_Dxf = false;
            txtFilePath.Text = "[Open Failed] " + fileName;

            //--
            if (dxf_Lib.Create() == false) return false;
            if (dxf_Lib.isReady() == false) return false;
            if (dxf_Lib.LoadDXFFile(fileName) == false) return false;

            //--
            dxf_Lib.InitView(0, 0, pnlDrawing.Width, pnlDrawing.Height);
            dxf_Lib.Set_PPU(1);
            dxf_Lib.ZoomExtents();

            //--
            if (arcGap <= 0) arcGap = 1;
            if (lineGap <= 0) lineGap = 1;

            //--
            dxf_Lib.Set_Arc_Pt_Res(arcGap);
            dxf_Lib.Set_Line_Pt_Res(lineGap);
            dxf_Lib.GetViewProperties(ref orgViewProp);

            if (dxf_Lib.Paint(pnlDrawing, false) == true)
            {
                //--
                if (dxf_Lib.GetEnttObjectList(ref Entt_List) == true)
                {
                    bLoad_Dxf = true;
                    txtFilePath.Text = "[Opened] " + fileName;
                    Display_Draw_Scale();

                    //--
                    layerObject.Clear_Layer_Objects();
                    for (int i = 0; i < Entt_List.Count; i++)
                    {
                        layerObject.Add_Entt_Object(Entt_List[i]);
                    }
                    
                    Display_Entt_List(layerObject);
                }
            }

            return bLoad_Dxf;
        }
        
        private void DrawObject(Object_Points objPoint)
        {
            for (int i = 0; i < objPoint.PenDown_Count; i++)
            {
                DrawPoint(objPoint.PointX_List[i], objPoint.PointY_List[i], objPoint.PenDown_List[i]);
            }
        }
        private void DrawPoint(double pointX, double pointY, bool bPenDown)
        {
            float size = 1;
            double ptX = 0, ptY = 0;
            Graphics gp = Graphics.FromHwnd(pnlDrawing.Handle);
            Pen drawPen = new Pen(Color.LawnGreen, 2);
            Pen penDown = new Pen(Color.Red, 4);

            dxf_Lib.Convert_UnitToScreen(pointX, pointY, ref ptX, ref ptY);

            if (bPenDown == false)
            {
                gp.DrawEllipse(penDown, (float)(ptX - (size / 2)), (float)(ptY - (size / 2)), size, size);
            }
            else
            {
                gp.DrawEllipse(drawPen, (float)(ptX - (size / 2)), (float)(ptY - (size / 2)), size, size);
            }
        }
        private void Draw_Selected_Layer(int layerID)
        {
            dxf_Lib.Paint(pnlDrawing, true);
            List<Object_Points> Obj_List = layerObject.Get_ObectList_In_Layer(layerID);
            for (int i = 0; i < Obj_List.Count; i++)
            {
                DrawObject(Obj_List[i]);
            }
        }
        private void Draw_Selected_Object(int layerID, int objectID)
        {
            dxf_Lib.Paint(pnlDrawing, true);
            List<Object_Points> Obj_List = layerObject.Get_ObectList_In_Layer(layerID);
            for (int i = 0; i < Obj_List.Count; i++)
            {
                if (Obj_List[i].ObjectID == objectID)
                {
                    DrawObject(Obj_List[i]);
                    return;
                }
            }
        }
        private void Draw_Selected_Object_Point(int layerID, int objectID, int pointID)
        {
            dxf_Lib.Paint(pnlDrawing, true);
            List<Object_Points> Obj_List = layerObject.Get_ObectList_In_Layer(layerID);
            for (int i = 0; i < Obj_List.Count; i++)
            {
                if (Obj_List[i].ObjectID == objectID)
                {
                    DrawPoint(Obj_List[i].PointX_List[pointID], Obj_List[i].PointY_List[pointID], Obj_List[i].PenDown_List[pointID]);
                    return;
                }
            }
        }
        private void Clear_TreeView()
        {
            for (int i = 0; i < tvLayer.Nodes.Count; i++)
            {
                for (int j = 0; j < tvLayer.Nodes[i].Nodes.Count; j++)
                {
                    tvLayer.Nodes[i].Nodes.Clear();
                }

                tvLayer.Nodes.Clear();
            }

            tvLayer.Nodes.Clear();
        }
        private void Display_Entt_List(Entt_Layer_Objects layerObject)
        {
            //-- Clear
            Clear_TreeView();

            //-- Add Layer
            TagType tagtype = TagType.tagLayer;
            for (int layer = 0; layer < layerObject.Layer_Count; layer++)
            {
                string strLayerName = string.Format("Layer) {0}", layerObject.Get_Layer_Name(layer));

                //-- Add Node
                TreeNode rootNode = new TreeNode(strLayerName, 0, 0);
                rootNode.Name = layer.ToString();
                rootNode.Tag = (object)tagtype;
                tvLayer.Nodes.Add(rootNode);

                //-- Add Object in Layer
                Add_Object_Node(rootNode, layerObject.Get_ObectList_In_Layer(layer));
            }
        }
        private void Add_Object_Node(TreeNode rootNode, List<Object_Points> objList)
        {
            string strObject = string.Empty;
            TagType tagtype = TagType.tagObject;
            for (int i = 0; i < objList.Count; i++)
            {
                strObject = string.Format("{0}) ObjectID {1} [ptCnt: {2}]", i+1,  objList[i].ObjectID, objList[i].PenDown_Count);

                //-- Add Obect Node
                TreeNode ObjNode = new TreeNode(strObject, 0, 0);
                ObjNode.Name = objList[i].ObjectID.ToString();
                ObjNode.Tag = (object)tagtype;
                rootNode.Nodes.Add(ObjNode);

                //-- Add Object Points
                Add_Object_Point_Node(ObjNode, objList[i]);
            }
        }
        private void Add_Object_Point_Node(TreeNode rootNode, Object_Points objPoints)
        {
            string strObjectPt = string.Empty;
            TagType tagtype = TagType.tagPoint;
            for (int i = 0; i < objPoints.PenDown_Count; i++)
            {
                strObjectPt = string.Format("{0}) X: {1:F3}, Y: {2:F3}, On: {3}", 
                    i + 1, objPoints.PointX_List[i], objPoints.PointY_List[i], objPoints.PenDown_List[i]);

                //-- Add Obect Node
                TreeNode ObjNode = new TreeNode(strObjectPt, 0, 0);
                ObjNode.Name = i.ToString();
                ObjNode.Tag = (object)tagtype;
                rootNode.Nodes.Add(ObjNode);
            }
        }
        private void Display_Draw_Scale()
        {
            if (bLoad_Dxf == false) return;
            lbZoom.Text = string.Format("Zoom: x {0:F1}", dxf_Lib.GetZoomLevel());
        }
        private void Draw_User_ROI(int stX, int stY, int edX, int edY)
        {
            if (bLoad_Dxf == false) return;
        }
        private void Draw_Dxf_Fit_Scale()
        {
            if (bLoad_Dxf == false) return;
            if (dxf_Lib.SetViewProperties(orgViewProp) == false) return;
            if (dxf_Lib.ZoomExtents() == false) return;
            if (reDrawing_Paint() == false) return;

            Display_Draw_Scale();
        }
        private void Set_Dxf_Drawing_Zoom(double ratio)
        {
            if (bLoad_Dxf == false) return;
            double zoom = dxf_Lib.GetZoomLevel() + ratio;

            //--
            if (zoom > maxZoom_Lvl) zoom = maxZoom_Lvl;
            if (zoom <= 0) zoom = 0.1;

            if (dxf_Lib.SetZoomLevel(zoom) == false) return;
            if (reDrawing_Paint() == false) return;

            Display_Draw_Scale();
        }
        private bool reDrawing_Paint()
        {
            if (bLoad_Dxf == false) return false;
            return dxf_Lib.Paint(pnlDrawing, true);
        }
        private void pnlDrawing_Paint(object sender, PaintEventArgs e)
        {
            if (bLoad_Dxf == false) return;
            dxf_Lib.Paint(pnlDrawing, true);
        }


        private void btnFileDlg_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Select File";
            ofd.Filter = "dxf File (*dxf) | *dxf";

            if(ofd.ShowDialog() == DialogResult.OK)
            {
                txtFilePath.Text = ofd.FileName;
            }
        }
        private void btnLoad_Click(object sender, EventArgs e)
        {
            if (txtFilePath.Text.Length > 0 && txtFilePath.Text.Substring(0, 1) != "[")
            {
                Open_Dxf_File(txtFilePath.Text);
            }
        }
        private void pnlDrawing_MouseMove(object sender, MouseEventArgs e)
        {
            //--
            if (bLoad_Dxf == false) return;

            //--
            if (bLBtnDown == true)
            {
                Draw_User_ROI(LDown_X, LDown_Y, e.X, e.Y);
            }
            else if (bRBtnDown == true)
            {
                dxf_Lib.Span_Dxf_Drawing(RDown_X - e.X, e.Y - RDown_Y);
                dxf_Lib.Paint(pnlDrawing, true);
                RDown_X = e.X;
                RDown_Y = e.Y;
            }

            //--
            double posX = 0, posY = 0;
            if (dxf_Lib.Convert_ScreenToUnit(e.X, e.Y, ref posX, ref posY) == true)
            {
                lbMousePos.Text = string.Format("X: {0:F3}, Y: {1:F3}", posX, posY);
            }
        }
        private void pnlDrawing_MouseWheel(object sender, MouseEventArgs e)
        {
            //--
            int lines = e.Delta * SystemInformation.MouseWheelScrollLines / 120;

            if (lines > 0)
                Set_Dxf_Drawing_Zoom(1);
            else if (lines < 0)
                Set_Dxf_Drawing_Zoom(-1);
        }
        private void btnFit_Click(object sender, EventArgs e)
        {
            //--
            Draw_Dxf_Fit_Scale();
        }
        private void btnZoomIn_Click(object sender, EventArgs e)
        {
            //--
            Set_Dxf_Drawing_Zoom(0.1);
        }
        private void btnZoomOut_Click(object sender, EventArgs e)
        {
            //--
            Set_Dxf_Drawing_Zoom(-0.1);
        }
        private void pnlDrawing_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right && bRBtnDown == false)
            {
                bRBtnDown = true;
                RDown_X = e.X;
                RDown_Y = e.Y;
                pnlDrawing.Cursor = Cursors.Hand;
            }
            else if (e.Button == MouseButtons.Left && bLBtnDown == false)
            {
                bLBtnDown = true;
            }
        }
        private void pnlDrawing_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && bRBtnDown == true)
            {
                bRBtnDown = false;
                pnlDrawing.Cursor = Cursors.Default;
            }
            else if (e.Button == MouseButtons.Left && bLBtnDown == true)
            {
                bLBtnDown = false;
            }
        }
        private void pnlDrawing_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //--
            if (bLoad_Dxf == false) return;

            if (e.Button == MouseButtons.Left)
            {
                int CenterX = this.Width / 2;
                int CenterY = this.Height / 2;
                int spanX = e.X - CenterX;
                int spanY = CenterY - e.Y;
                dxf_Lib.Span_Dxf_Drawing(spanX, spanY);

                //--
                Set_Dxf_Drawing_Zoom(2);
            }
        }
        private void tvLayer_AfterSelect(object sender, TreeViewEventArgs e)
        {
            int layerID = -1;
            int objectID = -1;
            int pointID = -1;

            TagType tagtype = (TagType)e.Node.Tag;
            if (tagtype == TagType.tagLayer)
            {
                if (Int32.TryParse(e.Node.Name, out layerID) == false) return;
                Draw_Selected_Layer(layerID);
            }
            else if (tagtype == TagType.tagObject)
            {
                if (Int32.TryParse(e.Node.Name, out objectID) == false) return;
                if (Int32.TryParse(e.Node.Parent.Name, out layerID) == false) return;
                Draw_Selected_Object(layerID, objectID);
            }
            if (tagtype == TagType.tagPoint)
            {
                if (Int32.TryParse(e.Node.Name, out pointID) == false) return;
                if (Int32.TryParse(e.Node.Parent.Name, out objectID) == false) return;
                if (Int32.TryParse(e.Node.Parent.Parent.Name, out layerID) == false) return;
                Draw_Selected_Object_Point(layerID, objectID, pointID);
            }
        }
    }
}
