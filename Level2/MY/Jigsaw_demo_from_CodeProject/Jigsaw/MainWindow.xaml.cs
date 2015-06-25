using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Media.Effects;
using System.IO;
using Microsoft.Win32;
using System.Reflection;
using Jigsaw.Properties;
using System.Windows.Threading;

namespace Jigsaw
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region attributes
        List<Piece>     m_currentSelection = new List<Piece>();
        int             m_selectionAngle = 0;
        List<Piece>     m_pieces = new List<Piece>();
        List<Piece>     m_shadowPieces = new List<Piece>();
        int             m_columns = 5;
        int             m_rows = 4;
        double          m_scale = 1.0;
        BitmapImage     m_imageSource;
        string          m_srcFileName = "";
        string          m_destFileName;
        DropShadowBitmapEffect m_shadowEffect;
        Point           m_lastCell = new Point(-1, 0);
        ScaleTransform  m_stZoomed = new ScaleTransform() { ScaleX = 1.1, ScaleY = 1.1 };
        ViewMode        m_currentViewMode = ViewMode.Puzzle;
        PngBitmapEncoder m_png;
        double          m_offsetX = -1;
        double          m_offsetY = -1;
        double          m_lastMouseDownX = -1;
        double          m_lastMouseDownY = -1;
        bool            m_moving = false;
        double          m_initialRectangleX = 0;
        double          m_initialRectangleY = 0;
        Rectangle       m_rectSelection = new Rectangle();
        #endregion attributes

        #region constructor
        public MainWindow()
        {
            InitializeComponent();

            m_destFileName = Settings.Default.DestinationFile;

            cnvPuzzle.MouseLeftButtonUp += new MouseButtonEventHandler(cnvPuzzle_MouseLeftButtonUp);
            cnvPuzzle.MouseDown += new MouseButtonEventHandler(cnvPuzzle_MouseDown);
            cnvPuzzle.MouseMove += new MouseEventHandler(cnvPuzzle_MouseMove);
            cnvPuzzle.MouseWheel += new MouseWheelEventHandler(cnvPuzzle_MouseWheel);
            cnvPuzzle.MouseEnter += new MouseEventHandler(cnvPuzzle_MouseEnter);
            cnvPuzzle.MouseLeave += new MouseEventHandler(cnvPuzzle_MouseLeave);

            m_shadowEffect = new DropShadowBitmapEffect()
            {
                Color = Colors.Black,
                Direction = 320,
                ShadowDepth = 25,
                Softness = 1,
                Opacity = 0.5
            };
        }

        #endregion constructor

        #region methods

        private void CreatePuzzle(Stream streamSource)
        {
            Random rnd = new Random();
            var connections = new int[] { (int)ConnectionType.Tab, (int)ConnectionType.Blank };

            m_png = null;

            m_imageSource = null;
            var uri = new Uri(m_destFileName);

            //We do this to avoid memory leaks
            using (WrappingStream wrapper = new WrappingStream(streamSource))
            using (BinaryReader reader = new BinaryReader(wrapper))
            {
                m_imageSource = new BitmapImage();
                m_imageSource.BeginInit();
                m_imageSource.CacheOption = BitmapCacheOption.OnLoad;
                m_imageSource.StreamSource = reader.BaseStream; // streamSource;
                m_imageSource.EndInit();
                m_imageSource.Freeze();
            }

            imgShowImage.Source = m_imageSource;

            scvImage.Visibility = Visibility.Hidden;
            cnvPuzzle.Visibility = Visibility.Visible;

            var angles = new int[] { 0, 90, 180, 270 };



            int index = 0;
            for (var y = 0; y < m_rows; y++)
            {
                for (var x = 0; x < m_columns; x++)
                {
                    if (x != 1000)
                    {
                        int upperConnection = (int)ConnectionType.None;
                        int rightConnection = (int)ConnectionType.None;
                        int bottomConnection = (int)ConnectionType.None;
                        int leftConnection = (int)ConnectionType.None;

                        if (y != 0)
                            upperConnection = -1 * m_pieces[(y - 1) * m_columns + x].BottomConnection;

                        if (x != m_columns - 1)
                            rightConnection = connections[rnd.Next(2)];

                        if (y != m_rows - 1)
                            bottomConnection = connections[rnd.Next(2)];

                        if (x != 0)
                            leftConnection = -1 * m_pieces[y * m_columns + x - 1].RightConnection;

                        int angle = 0;

                        var piece = new Piece(m_imageSource, x, y, 0.1, 0.1, (int)upperConnection, (int)rightConnection, (int)bottomConnection, (int)leftConnection, false, index, m_scale);
                        piece.SetValue(Canvas.ZIndexProperty, 1000 + x * m_rows + y);
                        piece.MouseLeftButtonUp += new MouseButtonEventHandler(piece_MouseLeftButtonUp);
                        piece.MouseRightButtonUp += new MouseButtonEventHandler(piece_MouseRightButtonUp);
                        piece.Rotate(piece, angle);

                        var shadowPiece = new Piece(m_imageSource, x, y, 0.1, 0.1, (int)upperConnection, (int)rightConnection, (int)bottomConnection, (int)leftConnection, true, m_shadowPieces.Count(), m_scale);
                        shadowPiece.SetValue(Canvas.ZIndexProperty, x * m_rows + y);
                        shadowPiece.Rotate(piece, angle);

                        m_pieces.Add(piece);
                        m_shadowPieces.Add(shadowPiece);
                        index++;
                    }
                }
            }

            var tt = new TranslateTransform() { X = 20, Y = 20 };

            foreach (var p in m_pieces)
            {
                Random random = new Random();
                int i = random.Next(0, pnlPickUp.Children.Count);

                p.ScaleTransform.ScaleX = 1.0;
                p.ScaleTransform.ScaleY = 1.0;
                p.RenderTransform = tt;
                p.X = -1;
                p.Y = -1;
                p.IsSelected = false;

                pnlPickUp.Children.Insert(i, p);

                double angle = angles[rnd.Next(0, 4)];
                p.Rotate(p, angle);
                m_shadowPieces[p.Index].Rotate(p, angle);
            }


            m_rectSelection.SetValue(Canvas.ZIndexProperty, 5000);

            m_rectSelection.StrokeDashArray = new DoubleCollection(new double[] {4,4,4,4});
            cnvPuzzle.Children.Add(m_rectSelection);
        }

        private void SavePuzzle()
        {
            var sfd = new SaveFileDialog()
                {
                    Filter = "All Image Files ( JPEG,GIF,BMP,PNG)|" +
                    "*.jpg;*.jpeg;*.gif;*.bmp;*.png|JPEG Files ( *.jpg;*.jpeg )|" +
                    "*.jpg;*.jpeg|GIF Files ( *.gif )|*.gif|BMP Files ( *.bmp )|" +
                    "*.bmp|PNG Files ( *.png )|*.png",
                    Title = "Save the image of your completed puzzle",
                    FileName = m_srcFileName.Split('.')[0] + "_puzzle." +
                    m_srcFileName.Split('.')[1]
                };

            sfd.DefaultExt = "png";
            sfd.ShowDialog();

            var query = from p in m_pieces
                        select p;

            var minX = query.Min<Piece>(x => x.X);
            var maxX = query.Max<Piece>(x => x.X);
            var minY = query.Min<Piece>(x => x.Y);
            var maxY = query.Max<Piece>(x => x.Y);

            var rtb = new RenderTargetBitmap((int)(maxX - minX + 1) * 100 + 40,
                (int)(maxY - minY + 1) * 100 + 40, 100, 100, PixelFormats.Pbgra32);
            cnvPuzzle.Arrange(new Rect(-minX * 100, -minY * 100,
                (int)(maxX - minX + 1) * 100 + 40, (int)(maxY - minY + 1) * 100 + 40));
            rtb.Render(cnvPuzzle);

            m_png = new PngBitmapEncoder();
            m_png.Frames.Add(BitmapFrame.Create(rtb));

            using (StreamWriter sw = new StreamWriter(sfd.FileName))
            {
                m_png.Save(sw.BaseStream);
            }
        }

        private void DestroyReferences()
        {
            for (var i = cnvPuzzle.Children.Count - 1; i >= 0; i--)
            {
                if (cnvPuzzle.Children[i] is Piece)
                {
                    Piece p = (Piece)cnvPuzzle.Children[i];
                    p.MouseLeftButtonUp -= new MouseButtonEventHandler(piece_MouseLeftButtonUp);
                    p.ClearImage();
                    cnvPuzzle.Children.Remove(p);
                }
            }

            cnvPuzzle.Children.Clear();
            SetSelectionRectangle(-1, -1, -1, -1);

            for (var i = pnlPickUp.Children.Count - 1; i >= 0; i--)
            {
                Piece p = (Piece)pnlPickUp.Children[i];
                p.ClearImage();
                p.MouseLeftButtonUp -= new MouseButtonEventHandler(piece_MouseLeftButtonUp);
                pnlPickUp.Children.Remove(p);
            }

            pnlPickUp.Children.Clear();

            for (var i = m_pieces.Count - 1; i >= 0; i--)
            {
                m_pieces[i].ClearImage();
            }

            for (var i = m_shadowPieces.Count - 1; i >= 0; i--)
            {
                m_shadowPieces[i].ClearImage();
            }

            m_shadowPieces.Clear();
            m_pieces.Clear();
            imgShowImage.Source = null;
            m_imageSource = null;
        }

        private Stream LoadImage(string srcFileName)
        {
            m_imageSource = new BitmapImage(new Uri(srcFileName));
            m_columns = (int)Math.Ceiling(m_imageSource.PixelWidth / 100.0);
            m_rows = (int)Math.Ceiling(m_imageSource.PixelHeight / 100.0);

            var bi = new BitmapImage(new Uri(srcFileName));
            var imgBrush = new ImageBrush(bi);
            imgBrush.AlignmentX = AlignmentX.Left;
            imgBrush.AlignmentY = AlignmentY.Top;
            imgBrush.Stretch = Stretch.UniformToFill;

            RenderTargetBitmap rtb = new RenderTargetBitmap((m_columns + 1) * 100, (m_rows + 1) * 100, bi.DpiX, bi.DpiY, PixelFormats.Pbgra32);

            var rectBlank = new Rectangle();
            rectBlank.Width = m_columns * 100;
            rectBlank.Height = m_rows * 100;
            rectBlank.HorizontalAlignment = HorizontalAlignment.Left;
            rectBlank.VerticalAlignment = VerticalAlignment.Top;
            rectBlank.Fill = new SolidColorBrush(Colors.White);
            rectBlank.Arrange(new Rect(0, 0, m_columns * 100, m_rows * 100));

            var rectImage = new Rectangle();
            rectImage.Width = m_imageSource.PixelWidth;
            rectImage.Height = m_imageSource.PixelHeight;
            rectImage.HorizontalAlignment = HorizontalAlignment.Left;
            rectImage.VerticalAlignment = VerticalAlignment.Top;
            rectImage.Fill = imgBrush;
            rectImage.Arrange(new Rect((m_columns * 100 - m_imageSource.PixelWidth) / 2, (m_rows * 100 - m_imageSource.PixelHeight) / 2, m_imageSource.PixelWidth, m_imageSource.PixelHeight));

            rectImage.Margin = new Thickness(
                (m_columns * 100 - m_imageSource.PixelWidth) / 2,
                (m_rows * 100 - m_imageSource.PixelHeight) / 2,
                (m_rows * 100 - m_imageSource.PixelHeight) / 2,
                (m_columns * 100 - m_imageSource.PixelWidth) / 2);

            rtb.Render(rectBlank);
            rtb.Render(rectImage);

            m_png = new PngBitmapEncoder();
            m_png.Frames.Add(BitmapFrame.Create(rtb));

            Stream ret = new MemoryStream();

            m_png.Save(ret);

            return ret;
        }

        private bool IsPuzzleCompleted()
        {
            //All pieces must have rotation of 0 degrees
            var query = from p in m_pieces
                        where p.Angle != 0
                        select p;

            if (query.Any())
                return false;

            //All pieces must be connected horizontally
            query = from p1 in m_pieces
                    join p2 in m_pieces on p1.Index equals p2.Index - 1
                    where (p1.Index % m_columns < m_columns - 1) && (p1.X + 1 != p2.X)
                    select p1;

            if (query.Any())
                return false;

            //All pieces must be connected vertically
            query = from p1 in m_pieces
                    join p2 in m_pieces on p1.Index equals p2.Index - m_columns
                    where (p1.Y + 1 != p2.Y)
                    select p1;

            if (query.Any())
                return false;

            return true;
        }

        private void ResetZIndexes()
        {
            int zIndex = 0;
            foreach (var p in m_shadowPieces)
            {
                p.SetValue(Canvas.ZIndexProperty, zIndex);
                zIndex++;
            }
            foreach (var p in m_pieces)
            {
                p.SetValue(Canvas.ZIndexProperty, zIndex);
                zIndex++;
            }
        }

        private bool TrySetCurrentPiecePosition(double newX, double newY)
        {
            bool ret = true;

            double cellX = (int)((newX) / 100);
            double cellY = (int)((newY) / 100);

            var firstPiece = m_currentSelection[0];

            foreach (var currentPiece in m_currentSelection)
            {
                var relativeCellX = currentPiece.X - firstPiece.X;
                var relativeCellY = currentPiece.Y - firstPiece.Y;

                double rotatedCellX = 0;
                double rotatedCellY = 0;
                rotatedCellX = relativeCellX;
                rotatedCellY = relativeCellY;

                var q = from p in m_pieces
                        where (
                                (p.Index != currentPiece.Index) &&
                                (!p.IsSelected) &&
                                (cellX + rotatedCellX > 0) &&
                                (cellY + rotatedCellY > 0) &&
                                (
                                ((p.X == cellX + rotatedCellX) && (p.Y == cellY + rotatedCellY))
                                || ((p.X == cellX + rotatedCellX - 1) && (p.Y == cellY + rotatedCellY) && 
                                (p.RightConnection + currentPiece.LeftConnection != 0))
                                || ((p.X == cellX + rotatedCellX + 1) && (p.Y == cellY + rotatedCellY) && 
                                (p.LeftConnection + currentPiece.RightConnection != 0))
                                || ((p.X == cellX + rotatedCellX) && (p.Y == cellY - 1 + rotatedCellY) && 
                                (p.BottomConnection + currentPiece.UpperConnection != 0))
                                || ((p.X == cellX + rotatedCellX) && (p.Y == cellY + 1 + rotatedCellY) && 
                                (p.UpperConnection + currentPiece.BottomConnection != 0))
                                )
                              )
                        select p;

                if (q.Any())
                {
                    ret = false;
                    break;
                }
            }

            return ret;
        }

        private Point SetCurrentPiecePosition(Piece currentPiece, double newX, double newY)
        {
            double cellX = (int)((newX) / 100);
            double cellY = (int)((newY) / 100);

            var firstPiece = m_currentSelection[0];

            var relativeCellX = currentPiece.X - firstPiece.X;
            var relativeCellY = currentPiece.Y - firstPiece.Y;

            double rotatedCellX = relativeCellX;
            double rotatedCellY = relativeCellY;

            currentPiece.X = cellX + rotatedCellX;
            currentPiece.Y = cellY + rotatedCellY;

            currentPiece.SetValue(Canvas.LeftProperty, currentPiece.X * 100);
            currentPiece.SetValue(Canvas.TopProperty, currentPiece.Y * 100);

            m_shadowPieces[currentPiece.Index].SetValue(Canvas.LeftProperty, currentPiece.X * 100);
            m_shadowPieces[currentPiece.Index].SetValue(Canvas.TopProperty, currentPiece.Y * 100);

            return new Point(cellX, cellY);
        }

        private void SetSelectionRectangle(double x1, double y1, double x2, double y2)
        {
            double x = (x2 >= x1) ? x1 : x2;
            double y = (y2 >= y1) ? y1 : y2;
            double width = Math.Abs(x2 - x1);
            double height = Math.Abs(y2 - y1);
            m_rectSelection.Visibility = System.Windows.Visibility.Visible;
            m_rectSelection.Width = width;
            m_rectSelection.Height = height;
            m_rectSelection.StrokeThickness = 4;
            m_rectSelection.Stroke = new SolidColorBrush(Colors.Red);

            m_rectSelection.SetValue(Canvas.LeftProperty, x);
            m_rectSelection.SetValue(Canvas.TopProperty, y);
        }

        private void MouseUp()
        {
            if (m_currentSelection.Count == 0)
            {
                double x1 = (double)m_rectSelection.GetValue(Canvas.LeftProperty) - 20;
                double y1 = (double)m_rectSelection.GetValue(Canvas.TopProperty) - 20;
                double x2 = x1 + m_rectSelection.Width;
                double y2 = y1 + m_rectSelection.Height;

                int cellX1 = (int)(x1 / 100);
                int cellY1 = (int)(y1 / 100);
                int cellX2 = (int)(x2 / 100);
                int cellY2 = (int)(y2 / 100);

                var query = from p in m_pieces
                            where
                            (p.X >= cellX1) && (p.X <= cellX2) &&
                            (p.Y >= cellY1) && (p.Y <= cellY2)
                            select p;

                //all pieces within that area will be selected
                foreach (var currentPiece in query)
                {
                    m_currentSelection.Add(currentPiece);

                    currentPiece.SetValue(Canvas.ZIndexProperty, 5000);
                    m_shadowPieces[currentPiece.Index].SetValue(Canvas.ZIndexProperty, 4999);
                    currentPiece.BitmapEffect = m_shadowEffect;

                    currentPiece.RenderTransform = m_stZoomed;
                    currentPiece.IsSelected = true;
                    m_shadowPieces[currentPiece.Index].RenderTransform = m_stZoomed;
                }
                SetSelectionRectangle(-1, -1, -1, -1);
            }
            else
            {
                var newX = Mouse.GetPosition(cnvPuzzle).X - 20;
                var newY = Mouse.GetPosition(cnvPuzzle).Y - 20;
                if (TrySetCurrentPiecePosition(newX, newY))
                {
                    int count = m_currentSelection.Count;
                    for (int i = count - 1; i >= 0; i--)
                    {
                        var currentPiece = m_currentSelection[i];

                        currentPiece.BitmapEffect = null;
                        ScaleTransform st = new ScaleTransform() { ScaleX = 1.0, ScaleY = 1.0 };
                        currentPiece.RenderTransform = st;
                        currentPiece.IsSelected = false;
                        m_shadowPieces[currentPiece.Index].RenderTransform = st;

                        m_lastCell = SetCurrentPiecePosition(currentPiece, newX, newY);

                        ResetZIndexes();

                        currentPiece = null;
                    }

                    m_currentSelection.Clear();

                    if (IsPuzzleCompleted())
                    {
                        var result = MessageBox.Show("Congratulations! You have solved the puzzle!\r\nWanna save it in a file now?", "Puzzle Completed", MessageBoxButton.YesNo, MessageBoxImage.Information);

                        if (result == MessageBoxResult.Yes)
                        {
                            SavePuzzle();
                        }
                    }
                }
                m_selectionAngle = 0;
            }
        }

        #endregion methods

        #region events

        void piece_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (m_currentSelection.Count > 0)
            {
                var axisPiece = m_currentSelection[0];
                foreach (var currentPiece in m_currentSelection)
                {
                    double deltaX = axisPiece.X - currentPiece.X;
                    double deltaY = axisPiece.Y - currentPiece.Y;

                    double targetCellX = deltaY;
                    double targetCellY = -deltaX;

                    currentPiece.Rotate(axisPiece, 90);
                    m_shadowPieces[currentPiece.Index].Rotate(axisPiece, 90);
                }
                m_selectionAngle += 90;
                if (m_selectionAngle == 360)
                    m_selectionAngle = 0;
            }
        }

        void piece_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            var chosenPiece = (Piece)sender;

            if (chosenPiece.Parent is WrapPanel)
            {
                if (m_currentSelection.Count() > 0)
                {
                    var p = m_currentSelection[0];
                    cnvPuzzle.Children.Remove(p);
                    cnvPuzzle.Children.Remove(m_shadowPieces[p.Index]);

                    var tt = new TranslateTransform() { X = 20, Y = 20 };
                    p.ScaleTransform.ScaleX = 1.0;
                    p.ScaleTransform.ScaleY = 1.0;
                    p.RenderTransform = tt;
                    p.X = -1;
                    p.Y = -1;
                    p.IsSelected = false;
                    p.SetValue(Canvas.ZIndexProperty, 0);
                    p.BitmapEffect = null;
                    p.Visibility = System.Windows.Visibility.Visible;
                    pnlPickUp.Children.Add(p);

                    m_currentSelection.Clear();
                }
                else
                {
                    pnlPickUp.Children.Remove(chosenPiece);
                    cnvPuzzle.Children.Add(m_shadowPieces[chosenPiece.Index]);
                    chosenPiece.SetValue(Canvas.ZIndexProperty, 5000);
                    m_shadowPieces[chosenPiece.Index].SetValue(Canvas.ZIndexProperty, 4999);
                    chosenPiece.BitmapEffect = m_shadowEffect;
                    chosenPiece.RenderTransform = m_stZoomed;
                    m_shadowPieces[chosenPiece.Index].RenderTransform = m_stZoomed;
                    cnvPuzzle.Children.Add(chosenPiece);
                    chosenPiece.Visibility = Visibility.Hidden;
                    m_shadowPieces[chosenPiece.Index].Visibility = Visibility.Hidden;
                    chosenPiece.IsSelected = true;
                    m_currentSelection.Add(chosenPiece);
                }
            }
        }

        void cnvPuzzle_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            MouseUp();
        }

        void cnvPuzzle_MouseEnter(object sender, MouseEventArgs e)
        {
            if (m_currentSelection.Count > 0)
            {
                foreach (var currentPiece in m_currentSelection)
                {
                    currentPiece.Visibility = Visibility.Visible;
                    if (m_shadowPieces.Count > currentPiece.Index)
                        m_shadowPieces[currentPiece.Index].Visibility = Visibility.Visible;
                }
            }
        }

        void cnvPuzzle_MouseWheel(object sender, MouseWheelEventArgs e)
        {

        }

        void cnvPuzzle_MouseDown(object sender, MouseButtonEventArgs e)
        {
            m_initialRectangleX = Mouse.GetPosition((IInputElement)sender).X;
            m_initialRectangleY = Mouse.GetPosition((IInputElement)sender).Y;
            SetSelectionRectangle(m_initialRectangleX, m_initialRectangleY, m_initialRectangleX, m_initialRectangleY);
        }

        void cnvPuzzle_MouseMove(object sender, MouseEventArgs e)
        {
            MouseMoving();
        }

        private void MouseMoving()
        {
            var newX = Mouse.GetPosition((IInputElement)cnvPuzzle).X - 20;
            var newY = Mouse.GetPosition((IInputElement)cnvPuzzle).Y - 20;

            int cellX = (int)((newX) / 100);
            int cellY = (int)((newY) / 100);

            if (Mouse.LeftButton == MouseButtonState.Pressed)
            {
                SetSelectionRectangle(m_initialRectangleX, m_initialRectangleY, newX, newY);
            }
            else
            {
                if (m_currentSelection.Count > 0)
                {
                    var firstPiece = m_currentSelection[0];

                    //This can move around more than one piece at the same time
                    foreach (var currentPiece in m_currentSelection)
                    {
                        var relativeCellX = currentPiece.X - firstPiece.X;
                        var relativeCellY = currentPiece.Y - firstPiece.Y;

                        double rotatedCellX = relativeCellX;
                        double rotatedCellY = relativeCellY;

                        currentPiece.SetValue(Canvas.LeftProperty, newX - 50 + rotatedCellX * 100);
                        currentPiece.SetValue(Canvas.TopProperty, newY - 50 + rotatedCellY * 100);

                        m_shadowPieces[currentPiece.Index].SetValue(Canvas.LeftProperty, newX - 50 + rotatedCellX * 100);
                        m_shadowPieces[currentPiece.Index].SetValue(Canvas.TopProperty, newY - 50 + rotatedCellY * 100);
                    }
                }
            }
        }

        void cnvPuzzle_MouseLeave(object sender, MouseEventArgs e)
        {
            SetSelectionRectangle(-1, -1, -1, -1);
            if (m_currentSelection.Count() > 0)
            {
                int count = m_currentSelection.Count();
                for (var i = count - 1; i >= 0; i--)
                {
                    var p = m_currentSelection[i];
                    cnvPuzzle.Children.Remove(p);
                    cnvPuzzle.Children.Remove(m_shadowPieces[p.Index]);

                    var tt = new TranslateTransform() { X = 20, Y = 20 };
                    p.ScaleTransform.ScaleX = 1.0;
                    p.ScaleTransform.ScaleY = 1.0;
                    p.RenderTransform = tt;
                    p.X = -1;
                    p.Y = -1;
                    p.IsSelected = false;
                    p.SetValue(Canvas.ZIndexProperty, 0);
                    p.BitmapEffect = null;
                    p.Visibility = System.Windows.Visibility.Visible;
                    pnlPickUp.Children.Add(p);
                }
                m_currentSelection.Clear();
            }
            MouseUp();
        }

        private void btnNewPuzzle_Click(object sender, RoutedEventArgs e)
        {
            var ofd = new OpenFileDialog()
            {
                Filter = "All Image Files ( JPEG,GIF,BMP,PNG)|*.jpg;*.jpeg;*.gif;*.bmp;*.png|JPEG Files ( *.jpg;*.jpeg )|*.jpg;*.jpeg|GIF Files ( *.gif )|*.gif|BMP Files ( *.bmp )|*.bmp|PNG Files ( *.png )|*.png",
                Title = "Select an image file for generating the puzzle"
            };

            bool? result = ofd.ShowDialog(this);

            if (result.Value)
            {
                try
                {
                    DestroyReferences();
                    m_srcFileName = ofd.FileName;
                    using (Stream streamSource = LoadImage(m_srcFileName))
                    {
                        CreatePuzzle(streamSource);
                    }
                    btnShowImage.IsEnabled = true;
                }
                catch (Exception exc)
                {
                    MessageBox.Show(exc.ToString());
                }
            }
        }

        private void btnShowImage_Click(object sender, RoutedEventArgs e)
        {
            grdPuzzle.Visibility = Visibility.Hidden;
            scvImage.Visibility = Visibility.Visible;
            m_currentViewMode = ViewMode.Picture;
            btnShowImage.Visibility = System.Windows.Visibility.Collapsed;
            btnShowPuzzle.Visibility = System.Windows.Visibility.Visible;
        }

        private void btnShowPuzzle_Click(object sender, RoutedEventArgs e)
        {
            grdPuzzle.Visibility = Visibility.Visible;
            scvImage.Visibility = Visibility.Hidden;
            m_currentViewMode = ViewMode.Puzzle;
            btnShowImage.Visibility = System.Windows.Visibility.Visible;
            btnShowPuzzle.Visibility = System.Windows.Visibility.Collapsed;
        }

        private void grdTop_MouseEnter(object sender, MouseEventArgs e)
        {
            this.WindowStyle = System.Windows.WindowStyle.ThreeDBorderWindow;
            grdWindow.RowDefinitions[0].Height = new GridLength(0);
        }

        private void StackPanel_MouseEnter(object sender, MouseEventArgs e)
        {
            this.WindowStyle = System.Windows.WindowStyle.None;
            grdWindow.RowDefinitions[0].Height = new GridLength(32);
        }

        private void DockPanel_MouseEnter(object sender, MouseEventArgs e)
        {
            this.WindowStyle = System.Windows.WindowStyle.None;
            grdWindow.RowDefinitions[0].Height = new GridLength(32);
        }

        private void brdWindow_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            m_moving = !m_moving;

            if (m_moving)
            {
                m_offsetX = Mouse.GetPosition((IInputElement)sender).X - (double)this.GetValue(Canvas.LeftProperty);
                m_offsetY = Mouse.GetPosition((IInputElement)sender).Y - (double)this.GetValue(Canvas.TopProperty);
                m_lastMouseDownX = Mouse.GetPosition((IInputElement)sender).X;
                m_lastMouseDownY = Mouse.GetPosition((IInputElement)sender).Y;
            }
        }

        private void txtMinimize_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this.WindowState = System.Windows.WindowState.Minimized;
        }

        private void txtMaximize_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (txtMaximize.Text == "1")
            {
                txtMaximize.Text = "2";
                this.WindowState = System.Windows.WindowState.Maximized;
            }
            else
            {
                txtMaximize.Text = "1";
                this.WindowState = System.Windows.WindowState.Normal;
            }
        }

        private void txtClose_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this.Close();

            if (m_currentSelection.Count > 0)
            {
                var axisPiece = m_currentSelection[0];
                foreach (var currentPiece in m_currentSelection)
                {
                    double offsetCellX = axisPiece.X - currentPiece.X;
                    double offsetCellY = axisPiece.Y - currentPiece.Y;

                    double rotatedCellX = 0;
                    double rotatedCellY = 0;

                    rotatedCellX = offsetCellX;
                    rotatedCellY = offsetCellY;

                    int a = (int)currentPiece.Angle;
                    switch (a)
                    {
                        case 0:
                            rotatedCellX = offsetCellX;
                            rotatedCellY = offsetCellY;
                            break;
                        case 90:
                            rotatedCellX = offsetCellY;
                            rotatedCellY = -offsetCellX;
                            break;
                        case 180:
                            rotatedCellX = offsetCellX;
                            rotatedCellY = offsetCellY;
                            break;
                        case 270:
                            rotatedCellX = -offsetCellY;
                            rotatedCellY = offsetCellX;
                            break;
                    }

                    currentPiece.Rotate(axisPiece, 90);
                    m_shadowPieces[currentPiece.Index].Rotate(axisPiece, 90);
                }
            }
        }

        #endregion events
    }

    public enum ViewMode
    {
        Picture,
        Puzzle
    }
}
