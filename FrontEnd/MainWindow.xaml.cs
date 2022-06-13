using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.ComponentModel;

using CefSharp;


namespace FrontEnd {

	public class MousePan
	{
		public Cursor cursor = Cursors.Arrow;
	}

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		WriteableBitmap viewport_bitmap;

		bool mouse_pannig_started;
		MousePan mouse_pan;


		public MainWindow()
		{
			InitializeComponent();

			// Viewport
			{
				viewport_bitmap = new WriteableBitmap(800, 600, 0, 0, PixelFormats.Bgra32, null);
				viewport_img.Source = viewport_bitmap;

				CompositionTarget.Rendering += (object? sender, EventArgs e) =>
				{
					//BackEnd.phase_1_runCPU();
					//BackEnd.phase_2_waitForRendering();

					//// Set Viewport Image
					//viewport_bitmap.Lock();

					//unsafe {
					//	BackEnd.phase_2X_tryDownloadRender(
					//		(uint)viewport_bitmap.PixelWidth,
					//		(uint)viewport_bitmap.PixelHeight,
					//		(byte*)viewport_bitmap.BackBuffer.ToPointer()
					//	);
					//}

					//viewport_bitmap.AddDirtyRect(new Int32Rect(0, 0, viewport_bitmap.PixelWidth, viewport_bitmap.PixelHeight));
					//viewport_bitmap.Unlock();

					//BackEnd.phase_3_render();
				};
			}

			// Camera
			{
				mouse_pannig_started = false;
				mouse_pan = new MousePan();

				// Pan
				viewport_img.MouseDown += beginCameraPan;
				viewport_img.MouseUp += endCameraPan;
			}

			main_canvas.SizeChanged += (object? sender, SizeChangedEventArgs e) =>
			{
				browser.Width = e.NewSize.Width;
				browser.Height = e.NewSize.Height;
			};

			KeyDown += (object sender, KeyEventArgs e) =>
			{
				if (e.Key == Key.F11) {
					browser.ShowDevTools();
				}
			};
		}

		public void setMousePosition(Point new_mouse_pos)
		{
			var screen_space_pos = PointToScreen(new_mouse_pos);
			Win32.SetCursorPos((int)screen_space_pos.X, (int)screen_space_pos.Y);
		}

		public void trapMouse(Point point)
		{
			unsafe {
				var screen_space_pos = PointToScreen(point);

				var rect = new Win32.Rect();
				rect.left = (int)screen_space_pos.X;
				rect.top = (int)screen_space_pos.Y;
				rect.right = rect.left + 1;
				rect.bottom = rect.top + 1;

				Win32.ClipCursor(&rect);
			}
		}

		public static void untrapMouse()
		{
			unsafe {
				Win32.ClipCursor(null);
			}
		}

		private void beginCameraPan(object sender, MouseButtonEventArgs e)
		{
			if (mouse_pannig_started == false && e.MiddleButton == MouseButtonState.Pressed) {

				mouse_pannig_started = true;
				mouse_pan.cursor = Cursor;

				trapMouse(e.GetPosition(this));
				Cursor = Cursors.None;
			}
		}

		private void endCameraPan(object sender, MouseButtonEventArgs e)
		{
			if (mouse_pannig_started && e.MiddleButton == MouseButtonState.Released) {

				untrapMouse();
				Cursor = mouse_pan.cursor;

				mouse_pannig_started = false;
			}
		}

		void captureFrame(object sender, RoutedEventArgs e)
		{
			BackEnd.captureFrame();
		}
	}


	//public class ControlHost : HwndHost
	//{
	//	IntPtr hwnd;
	//	int window_width;
	//	int window_height;

	//	public ControlHost(double width, double height)
	//	{
	//		window_width = (int)width;
	//		window_height = (int)height;
	//	}

	//	protected override HandleRef BuildWindowCore(HandleRef hwnd_parent)
	//	{
	//		int ws_child = 0x40000000;
	//		int ws_visible = 0x10000000;

	//		hwnd = Win32.CreateWindowEx(
	//			0,
	//			"static",
	//			"",
	//			ws_child | ws_visible | 0x00000008 | 0x00000020,
	//			0, 0,
	//			window_width, window_height,
	//			hwnd_parent.Handle,
	//			IntPtr.Zero,
	//			IntPtr.Zero,
	//			IntPtr.Zero
	//		);

	//		if (hwnd == IntPtr.Zero) {
	//			throw new Win32Exception(Marshal.GetLastWin32Error());
	//		}

	//		return new HandleRef(this, hwnd);
	//	}

	//	protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
	//	{
	//		handled = false;
	//		return IntPtr.Zero;
	//	}

	//	protected override void DestroyWindowCore(HandleRef hwnd)
	//	{
	//		Win32.DestroyWindow(hwnd.Handle);
	//	}
	//}
}
