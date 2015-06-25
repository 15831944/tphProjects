using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Shapes;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using System.Windows;
using System.Windows.Media.Animation;
using System.Windows.Threading;

namespace Jigsaw
{
    public class Piece : Grid
    {
        #region attributes
        Path m_path;
        Path m_shadowPath;
        string m_imageUri;
        double m_initialX;
        double m_initialY;
        double m_x;
        double m_y;
        double m_xPercent;
        double m_yPercent;
        int m_upperConnection;
        int m_rightConnection;
        int m_bottomConnection;
        int m_leftConnection;

        int m_initialUpperConnection;
        int m_initialRightConnection;
        int m_initialBottomConnection;
        int m_initialLeftConnection;

        double m_angle = 0;
        int m_index = 0;
        TranslateTransform m_tt1;
        TranslateTransform m_tt2;

        TransformGroup m_tgPiece = new TransformGroup();
        TransformGroup m_tg1 = new TransformGroup();
        TransformGroup m_tg2 = new TransformGroup();

        #endregion attributes

        #region constructor
        public Piece(ImageSource imageSource, double x, double y, double xPercent, double yPercent, int upperConnection, int rightConnection, int bottomConnection, int leftConnection, bool isShadow, int index, double scale)
        {
            this.ImageUri = m_imageUri;
            this.InitialX = x;
            this.InitialY = y;
            this.X = x;
            this.Y = y;
            this.XPercent = xPercent;
            this.YPercent = yPercent;

            m_initialUpperConnection =
            this.m_upperConnection = upperConnection;

            m_initialRightConnection =
            this.m_rightConnection = rightConnection;

            m_initialBottomConnection =
            this.m_bottomConnection = bottomConnection;

            m_initialLeftConnection =
            this.m_leftConnection = leftConnection;

            this.Index = index;
            this.ScaleTransform = new ScaleTransform() { ScaleX = 1.0, ScaleY = 1.0 };

            m_path = new Path()
            {
                Stroke = new SolidColorBrush(Colors.Gray),
                StrokeThickness = 0
            };

            m_shadowPath = new Path()
            {
                Stroke = new SolidColorBrush(Colors.Black),
                StrokeThickness = 2 * scale
            };

            var lgb = new LinearGradientBrush();
            lgb.GradientStops.Add(new GradientStop() { Offset = 0.0, Color = Color.FromArgb(0xFF, 0xFF, 0xFF, 0xFF) });
            lgb.GradientStops.Add(new GradientStop() { Offset = 0.0, Color = Color.FromArgb(0x00, 0x80, 0x80, 0x80) });
            lgb.GradientStops.Add(new GradientStop() { Offset = 1.0, Color = Color.FromArgb(0xFF, 0x00, 0x00, 0x00) });

            var imageScaleTransform = ScaleTransform;


            m_path.Fill = new ImageBrush()
            {
                ImageSource = imageSource,
                Stretch = Stretch.None,
                Viewport = new Rect(-20, -20, 140, 140),
                ViewportUnits = BrushMappingMode.Absolute,
                Viewbox = new Rect(
                    x * 100 - 10,
                    y * 100 - 10,
                    120,
                    120
                    ),
                ViewboxUnits = BrushMappingMode.Absolute,
                Transform = imageScaleTransform
            };

            var curvyCoords = new double[]
                {
                    0, 0, 35, 15, 37, 5,
                    37, 5, 40, 0, 38, -5,
                    38, -5, 20, -20, 50, -20,
                    50, -20, 80, -20, 62, -5,
                    62, -5, 60, 0, 63, 5,
                    63, 5, 65, 15, 100, 0
                };

            for (var i = 0; i < curvyCoords.Length; i++)
            {
                curvyCoords[i] *= scale;
            }

            //t double[] coords = curvyCoords;

            var outerPathGeometry = GetPathGeometry(upperConnection, rightConnection, bottomConnection, leftConnection, curvyCoords, curvyCoords);

            m_path.Data = outerPathGeometry;
            m_shadowPath.Data = outerPathGeometry;

            var rt = new RotateTransform()
            {
                CenterX = 50,
                CenterY = 50,
                Angle = 0
            };

            m_tt1 = new TranslateTransform()
            {
                X = 0,
                Y = 0
            };


            Random rnd = new Random(DateTime.Now.Millisecond);

            TimeSpan beginTime = new TimeSpan(0, 0, 0, 0, rnd.Next(50, 200) * (int)(x + y));
            TimeSpan duration = new TimeSpan(0, 0, 0, 0, rnd.Next(50, 200) * (int)(x + y));

            m_tg1.Children.Add(m_tt1);
            m_tg1.Children.Add(rt);

            m_path.RenderTransform = m_tg1;

            m_tt2 = new TranslateTransform()
            {
                X = 1,
                Y = 1
            };

            m_tg2.Children.Add(m_tt2);
            m_tg2.Children.Add(rt);

            m_shadowPath.RenderTransform = m_tg2;

            var tt3 = new TranslateTransform()
            {
                X = x * 100 * scale + 0,
                Y = y * 100 * scale + 0
            };

            var st = new ScaleTransform() { ScaleX = 0.95, ScaleY = 0.95 };

            var tg = new TransformGroup();

            tg.Children.Add(st);
            tg.Children.Add(tt3);

            this.Width = 140 * scale;
            this.Height = 140 * scale;

            if (isShadow)
                this.Children.Add(m_shadowPath);
            else
                this.Children.Add(m_path);
        }

        #endregion constructor

        #region methods
        private static PathGeometry GetPathGeometry(int upperConnection, int rightConnection, int bottomConnection, int leftConnection, double[] blankCoords, double[] tabCoords)
        {
            var upperPoints = new List<Point>();
            var rightPoints = new List<Point>();
            var bottomPoints = new List<Point>();
            var leftPoints = new List<Point>();

            for (var i = 0; i < (tabCoords.Length / 2); i++)
            {
                double[] upperCoords = (upperConnection == (int)ConnectionType.Blank) ? blankCoords : tabCoords;
                double[] rightCoords = (rightConnection == (int)ConnectionType.Blank) ? blankCoords : tabCoords;
                double[] bottomCoords = (bottomConnection == (int)ConnectionType.Blank) ? blankCoords : tabCoords;
                double[] leftCoords = (leftConnection == (int)ConnectionType.Blank) ? blankCoords : tabCoords;

                upperPoints.Add(new Point(upperCoords[i * 2], 0 + upperCoords[i * 2 + 1] * upperConnection));
                rightPoints.Add(new Point(100 - rightCoords[i * 2 + 1] * rightConnection, rightCoords[i * 2]));
                bottomPoints.Add(new Point(100 - bottomCoords[i * 2], 100 - bottomCoords[i * 2 + 1] * bottomConnection));
                leftPoints.Add(new Point(0 + leftCoords[i * 2 + 1] * leftConnection, 100 - leftCoords[i * 2]));
            }

            var upperSegment = new PolyBezierSegment(upperPoints, true);
            var rightSegment = new PolyBezierSegment(rightPoints, true);
            var bottomSegment = new PolyBezierSegment(bottomPoints, true);
            var leftSegment = new PolyBezierSegment(leftPoints, true);

            var pathFigure = new PathFigure()
            {
                IsClosed = false,
                StartPoint = new Point(0, 0)
            };
            pathFigure.Segments.Add(upperSegment);
            pathFigure.Segments.Add(rightSegment);
            pathFigure.Segments.Add(bottomSegment);
            pathFigure.Segments.Add(leftSegment);

            var pathGeometry = new PathGeometry();
            pathGeometry.Figures.Add(pathFigure);
            return pathGeometry;
        }

        public int UpperConnection
        {
            get
            {
                var connection = 0;

                int a = (int)m_angle;
                switch (a)
                {
                    case 0:
                        connection = m_initialUpperConnection;
                        break;
                    case 90:
                        connection = m_initialLeftConnection;
                        break;
                    case 180:
                        connection = m_initialBottomConnection;
                        break;
                    case 270:
                        connection = m_initialRightConnection;
                        break;
                }
                return connection;
            }
        }

        public int LeftConnection
        {
            get
            {
                var connection = 0;
                int a = (int)m_angle;
                switch (a)
                {
                    case 0:
                        connection = m_initialLeftConnection;
                        break;
                    case 90:
                        connection = m_initialBottomConnection;
                        break;
                    case 180:
                        connection = m_initialRightConnection;
                        break;
                    case 270:
                        connection = m_initialUpperConnection;
                        break;
                }
                return connection;
            }
        }

        public int BottomConnection
        {
            get
            {
                var connection = 0;
                int a = (int)m_angle;
                switch (a)
                {
                    case 0:
                        connection = m_initialBottomConnection;
                        break;
                    case 90:
                        connection = m_initialRightConnection;
                        break;
                    case 180:
                        connection = m_initialUpperConnection;
                        break;
                    case 270:
                        connection = m_initialLeftConnection;
                        break;
                }
                return connection;
            }
        }

        public int RightConnection
        {
            get
            {
                var connection = 0;
                int a = (int)m_angle;
                switch (a)
                {
                    case 0:
                        connection = m_initialRightConnection;
                        break;
                    case 90:
                        connection = m_initialUpperConnection;
                        break;
                    case 180:
                        connection = m_initialLeftConnection;
                        break;
                    case 270:
                        connection = m_initialBottomConnection;
                        break;
                }
                return connection;
            }

        }

        public void Rotate(Piece axisPiece, double rotationAngle)
        {
            var deltaCellX = this.X - axisPiece.X;
            var deltaCellY = this.Y - axisPiece.Y;

            double rotatedCellX = 0;
            double rotatedCellY = 0;

            int a = (int)rotationAngle;
            switch (a)
            {
                case 0:
                    rotatedCellX = deltaCellX;
                    rotatedCellY = deltaCellY;
                    break;
                case 90:
                    rotatedCellX = -deltaCellY;
                    rotatedCellY = deltaCellX;
                    break;
                case 180:
                    rotatedCellX = -deltaCellX;
                    rotatedCellY = -deltaCellY;
                    break;
                case 270:
                    rotatedCellX = deltaCellY;
                    rotatedCellY = -deltaCellX;
                    break;
            }

            this.X = axisPiece.X + rotatedCellX;
            this.Y = axisPiece.Y + rotatedCellY;

            var rt1 = (RotateTransform)m_tg1.Children[1];
            var rt2 = (RotateTransform)m_tg2.Children[1];

            m_angle += rotationAngle;

            if (m_angle == -90)
                m_angle = 270;

            if (m_angle == 360)
                m_angle = 0;

            rt1.Angle =
            rt2.Angle = m_angle;
            
            this.SetValue(Canvas.LeftProperty, this.X * 100);
            this.SetValue(Canvas.TopProperty, this.Y * 100);
        }

        public void ClearImage()
        {
            m_path.Fill = null;
        }

        #endregion methods

        #region properties
        public string ImageUri { get { return m_imageUri; } set { m_imageUri = value; } }
        public double X { get { return m_x; } set { m_x = value; } }
        public double Y { get { return m_y; } set { m_y = value; } }
        public double InitialX { get { return m_initialX; } set { m_initialX = value; } }
        public double InitialY { get { return m_initialY; } set { m_initialY = value; } }
        public double XPercent { get { return m_xPercent; } set { m_xPercent = value; } }
        public double YPercent { get { return m_yPercent; } set { m_yPercent = value; } }
        public double Angle { get { return m_angle; } set { m_angle = value; } }
        public int Index { get { return m_index; } set { m_index = value; } }
        public bool IsSelected { get; set; }
        public ScaleTransform ScaleTransform { get; set; }
        #endregion properties

        #region events
        #endregion events
    }

    public enum ConnectionType
    {
        None = 0,
        Tab = 1,
        Blank = -1
    }
}
