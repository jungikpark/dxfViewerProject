namespace DxfViewer.DxfViewer.dxfDrawing
{
    partial class dxfDraw
    {
        /// <summary> 
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 구성 요소 디자이너에서 생성한 코드

        /// <summary> 
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.pnlDrawing = new System.Windows.Forms.Panel();
            this.btnLoad = new System.Windows.Forms.Button();
            this.btnFileDlg = new System.Windows.Forms.Button();
            this.txtFilePath = new System.Windows.Forms.TextBox();
            this.lbProcSeqStep = new System.Windows.Forms.Label();
            this.lbSelEnttCnt = new System.Windows.Forms.Label();
            this.lbMousePos = new System.Windows.Forms.Label();
            this.lbProcSeq = new System.Windows.Forms.Label();
            this.btnFit = new System.Windows.Forms.Button();
            this.btnZoomOut = new System.Windows.Forms.Button();
            this.btnZoomIn = new System.Windows.Forms.Button();
            this.lbZoom = new System.Windows.Forms.Label();
            this.tvLayer = new System.Windows.Forms.TreeView();
            this.SuspendLayout();
            // 
            // pnlDrawing
            // 
            this.pnlDrawing.BackColor = System.Drawing.Color.Black;
            this.pnlDrawing.Location = new System.Drawing.Point(1, 32);
            this.pnlDrawing.Name = "pnlDrawing";
            this.pnlDrawing.Size = new System.Drawing.Size(1024, 768);
            this.pnlDrawing.TabIndex = 0;
            this.pnlDrawing.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlDrawing_Paint);
            this.pnlDrawing.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.pnlDrawing_MouseDoubleClick);
            this.pnlDrawing.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pnlDrawing_MouseDown);
            this.pnlDrawing.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pnlDrawing_MouseMove);
            this.pnlDrawing.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pnlDrawing_MouseUp);
            this.pnlDrawing.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.pnlDrawing_MouseWheel);
            // 
            // btnLoad
            // 
            this.btnLoad.BackColor = System.Drawing.Color.DarkGray;
            this.btnLoad.Cursor = System.Windows.Forms.Cursors.Default;
            this.btnLoad.Location = new System.Drawing.Point(965, 7);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(60, 23);
            this.btnLoad.TabIndex = 5;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = false;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // btnFileDlg
            // 
            this.btnFileDlg.BackColor = System.Drawing.Color.DarkGray;
            this.btnFileDlg.Location = new System.Drawing.Point(924, 7);
            this.btnFileDlg.Name = "btnFileDlg";
            this.btnFileDlg.Size = new System.Drawing.Size(40, 23);
            this.btnFileDlg.TabIndex = 4;
            this.btnFileDlg.Text = "...";
            this.btnFileDlg.UseVisualStyleBackColor = false;
            this.btnFileDlg.Click += new System.EventHandler(this.btnFileDlg_Click);
            // 
            // txtFilePath
            // 
            this.txtFilePath.BackColor = System.Drawing.Color.White;
            this.txtFilePath.Location = new System.Drawing.Point(374, 9);
            this.txtFilePath.Name = "txtFilePath";
            this.txtFilePath.ReadOnly = true;
            this.txtFilePath.Size = new System.Drawing.Size(547, 21);
            this.txtFilePath.TabIndex = 3;
            // 
            // lbProcSeqStep
            // 
            this.lbProcSeqStep.Location = new System.Drawing.Point(452, 803);
            this.lbProcSeqStep.Name = "lbProcSeqStep";
            this.lbProcSeqStep.Size = new System.Drawing.Size(148, 23);
            this.lbProcSeqStep.TabIndex = 47;
            this.lbProcSeqStep.Text = "unit[1, 1] entt[1, 1]";
            this.lbProcSeqStep.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lbProcSeqStep.Visible = false;
            // 
            // lbSelEnttCnt
            // 
            this.lbSelEnttCnt.Location = new System.Drawing.Point(309, 803);
            this.lbSelEnttCnt.Name = "lbSelEnttCnt";
            this.lbSelEnttCnt.Size = new System.Drawing.Size(137, 23);
            this.lbSelEnttCnt.TabIndex = 46;
            this.lbSelEnttCnt.Text = "Selected Points: 0";
            this.lbSelEnttCnt.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lbSelEnttCnt.Visible = false;
            // 
            // lbMousePos
            // 
            this.lbMousePos.Location = new System.Drawing.Point(163, 803);
            this.lbMousePos.Name = "lbMousePos";
            this.lbMousePos.Size = new System.Drawing.Size(144, 23);
            this.lbMousePos.TabIndex = 45;
            this.lbMousePos.Text = "X:  Y: ";
            this.lbMousePos.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbProcSeq
            // 
            this.lbProcSeq.Location = new System.Drawing.Point(604, 803);
            this.lbProcSeq.Name = "lbProcSeq";
            this.lbProcSeq.Size = new System.Drawing.Size(127, 23);
            this.lbProcSeq.TabIndex = 44;
            this.lbProcSeq.Text = "[1/100000] 0%";
            this.lbProcSeq.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lbProcSeq.Visible = false;
            // 
            // btnFit
            // 
            this.btnFit.BackColor = System.Drawing.Color.DarkGray;
            this.btnFit.FlatAppearance.MouseOverBackColor = System.Drawing.Color.DeepSkyBlue;
            this.btnFit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFit.Location = new System.Drawing.Point(737, 803);
            this.btnFit.Name = "btnFit";
            this.btnFit.Size = new System.Drawing.Size(60, 23);
            this.btnFit.TabIndex = 43;
            this.btnFit.Text = "Fit";
            this.btnFit.UseVisualStyleBackColor = false;
            this.btnFit.Click += new System.EventHandler(this.btnFit_Click);
            // 
            // btnZoomOut
            // 
            this.btnZoomOut.BackColor = System.Drawing.Color.DarkGray;
            this.btnZoomOut.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Red;
            this.btnZoomOut.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnZoomOut.Location = new System.Drawing.Point(860, 803);
            this.btnZoomOut.Name = "btnZoomOut";
            this.btnZoomOut.Size = new System.Drawing.Size(60, 23);
            this.btnZoomOut.TabIndex = 42;
            this.btnZoomOut.Text = "x -0.1";
            this.btnZoomOut.UseVisualStyleBackColor = false;
            this.btnZoomOut.Click += new System.EventHandler(this.btnZoomOut_Click);
            // 
            // btnZoomIn
            // 
            this.btnZoomIn.BackColor = System.Drawing.Color.DarkGray;
            this.btnZoomIn.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Red;
            this.btnZoomIn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnZoomIn.Location = new System.Drawing.Point(799, 803);
            this.btnZoomIn.Name = "btnZoomIn";
            this.btnZoomIn.Size = new System.Drawing.Size(60, 23);
            this.btnZoomIn.TabIndex = 41;
            this.btnZoomIn.Text = "x 0.1";
            this.btnZoomIn.UseVisualStyleBackColor = false;
            this.btnZoomIn.Click += new System.EventHandler(this.btnZoomIn_Click);
            // 
            // lbZoom
            // 
            this.lbZoom.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbZoom.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.lbZoom.Location = new System.Drawing.Point(922, 803);
            this.lbZoom.Name = "lbZoom";
            this.lbZoom.Size = new System.Drawing.Size(103, 23);
            this.lbZoom.TabIndex = 40;
            this.lbZoom.Text = "Zoom: x 0.0";
            this.lbZoom.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // tvLayer
            // 
            this.tvLayer.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tvLayer.Location = new System.Drawing.Point(1026, 32);
            this.tvLayer.Name = "tvLayer";
            this.tvLayer.Size = new System.Drawing.Size(293, 768);
            this.tvLayer.TabIndex = 48;
            this.tvLayer.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvLayer_AfterSelect);
            // 
            // dxfDraw
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DarkGray;
            this.Controls.Add(this.tvLayer);
            this.Controls.Add(this.lbProcSeqStep);
            this.Controls.Add(this.lbSelEnttCnt);
            this.Controls.Add(this.lbMousePos);
            this.Controls.Add(this.lbProcSeq);
            this.Controls.Add(this.btnFit);
            this.Controls.Add(this.btnZoomOut);
            this.Controls.Add(this.btnZoomIn);
            this.Controls.Add(this.lbZoom);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnFileDlg);
            this.Controls.Add(this.txtFilePath);
            this.Controls.Add(this.pnlDrawing);
            this.Font = new System.Drawing.Font("Arial", 9F);
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Name = "dxfDraw";
            this.Size = new System.Drawing.Size(1326, 830);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel pnlDrawing;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.Button btnFileDlg;
        private System.Windows.Forms.TextBox txtFilePath;
        private System.Windows.Forms.Label lbProcSeqStep;
        private System.Windows.Forms.Label lbSelEnttCnt;
        private System.Windows.Forms.Label lbMousePos;
        private System.Windows.Forms.Label lbProcSeq;
        private System.Windows.Forms.Button btnFit;
        private System.Windows.Forms.Button btnZoomOut;
        private System.Windows.Forms.Button btnZoomIn;
        private System.Windows.Forms.Label lbZoom;
        private System.Windows.Forms.TreeView tvLayer;
    }
}
