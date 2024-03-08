using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DxfViewer.DxfViewer.dxfDrawing;

namespace DxfViewer
{
    public partial class MainForm : Form
    {
        public dxfDraw dxfViewer;
        public MainForm()
        {
            InitializeComponent();

            //--
            dxfViewer = new dxfDraw();
            dxfViewer.Set_Parents_Location(this.Controls, new Point(1, 1));
            dxfViewer.Open_Dxf_File("D:\\Dxf_Data\\circle.dxf");
        }
    }
}
