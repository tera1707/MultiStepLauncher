using System;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace MultiStepLauncher;

public partial class MainWindow : Window
{
    private const int WM_SYSCOMMAND = 0x0112;
    private const int SC_MINIMIZE = 0xF020;
    private const int SC_RESTORE = 0xF120;

    DispatcherTimer dt = new();

    private Point? _launcherFirstDragStart;
    private bool _launcherFirstWasDragged;
    private const double DragThreshold = 4;

    public MainWindow()
    {
        InitializeComponent();

        this.MouseLeftButtonDown += (sender, e) => { this.DragMove(); };

        dt.Interval = TimeSpan.FromSeconds(3);
        dt.Tick += ((s, e) =>
        {
            dt.Stop();
            HideAllLauncherButtons();
        });
    }

    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
        HwndSource? source = PresentationSource.FromVisual(this) as HwndSource;
        source?.AddHook(WndProc);

        Button_LauncherSecond.Visibility = Visibility.Collapsed;
        Button_Launcher1.Visibility = Visibility.Collapsed;
        Button_Launcher2.Visibility = Visibility.Collapsed;
        Button_Launcher3.Visibility = Visibility.Collapsed;
        Button_Launcher4.Visibility = Visibility.Collapsed;
        Button_Launcher5.Visibility = Visibility.Collapsed;
    }

    private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
    {
        if (msg == WM_SYSCOMMAND)
        {
            // 下位 4 ビットはシステムが予約しているため無視してマスクします。
            int command = wParam.ToInt32() & 0xFFF0;
            if (command == SC_MINIMIZE || command == SC_RESTORE)
            {
                // Win+↓ や最小化操作によるメッセージを握りつぶします。
                handled = true;
                return IntPtr.Zero;
            }
        }

        return IntPtr.Zero;
    }

    private void Button_Click(object sender, RoutedEventArgs e)
    {
        if (_launcherFirstWasDragged)
        {
            _launcherFirstWasDragged = false;
            return;
        }

        Button_LauncherFirst.Visibility = Visibility.Collapsed;
        Button_LauncherSecond.Visibility = Visibility.Visible;
        dt.Start();
    }

    private void Button_LauncherFirst_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        _launcherFirstDragStart = e.GetPosition(this);
        _launcherFirstWasDragged = false;
    }

    private void Button_LauncherFirst_PreviewMouseMove(object sender, MouseEventArgs e)
    {
        if (_launcherFirstDragStart is null || e.LeftButton != MouseButtonState.Pressed)
        {
            return;
        }

        Point current = e.GetPosition(this);
        double deltaX = current.X - _launcherFirstDragStart.Value.X;
        double deltaY = current.Y - _launcherFirstDragStart.Value.Y;

        if (!_launcherFirstWasDragged && (Math.Abs(deltaX) < DragThreshold && Math.Abs(deltaY) < DragThreshold))
        {
            return;
        }

        if (!_launcherFirstWasDragged)
        {
            _launcherFirstWasDragged = true;
            Button_LauncherFirst.CaptureMouse();
        }

        Button_LauncherFirstTranslate.X += deltaX;
        Button_LauncherFirstTranslate.Y += deltaY;

        _launcherFirstDragStart = current;
    }

    private void Button_LauncherFirst_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        _launcherFirstDragStart = null;

        if (_launcherFirstWasDragged)
        {
            Button_LauncherFirst.ReleaseMouseCapture();
            e.Handled = true;
        }
    }

    private void Button_LauncherFirst_LostMouseCapture(object sender, MouseEventArgs e)
    {
        _launcherFirstDragStart = null;
    }

    private void Button_Click_1(object sender, RoutedEventArgs e)
    {
        dt.Stop();
        Button_LauncherSecond.Visibility = Visibility.Collapsed;
        Button_Launcher1.Visibility = Visibility.Visible;
        Button_Launcher2.Visibility = Visibility.Visible;
        Button_Launcher3.Visibility = Visibility.Visible;
        Button_Launcher4.Visibility = Visibility.Visible;
        Button_Launcher5.Visibility = Visibility.Visible;
        dt.Start();
    }

    private void Button_Launcher1_Click(object sender, RoutedEventArgs e)
    {
        // メモ帳起動処理
        System.Diagnostics.Process.Start("notepad.exe");

        HideAllLauncherButtons();
    }

    private void Button_Launcher2_Click(object sender, RoutedEventArgs e)
    {
        // 電卓起動処理
        System.Diagnostics.Process.Start("calc.exe");

        HideAllLauncherButtons();
    }

    private void Button_Launcher3_Click(object sender, RoutedEventArgs e)
    {
        // ペイント起動処理
        System.Diagnostics.Process.Start("mspaint.exe");

        HideAllLauncherButtons();
    }

    private void Button_Launcher4_Click(object sender, RoutedEventArgs e)
    {
        // 設定画面起動処理
        System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("ms-settings:") { UseShellExecute = true });

        HideAllLauncherButtons();
    }

    private void Button_Launcher5_Click(object sender, RoutedEventArgs e)
    {
        HideAllLauncherButtons();
    }

    private void HideAllLauncherButtons()
    {
        dt.Stop();
        Button_LauncherFirst.Visibility = Visibility.Visible;
        Button_LauncherSecond.Visibility = Visibility.Collapsed;
        Button_Launcher1.Visibility = Visibility.Collapsed;
        Button_Launcher2.Visibility = Visibility.Collapsed;
        Button_Launcher3.Visibility = Visibility.Collapsed;
        Button_Launcher4.Visibility = Visibility.Collapsed;
        Button_Launcher5.Visibility = Visibility.Collapsed;
    }
}