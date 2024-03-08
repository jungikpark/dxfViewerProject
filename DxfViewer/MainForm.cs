using System.Drawing;
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
