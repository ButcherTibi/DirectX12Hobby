using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
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


namespace FrontEnd {

	class F
	{
		public void create()
		{

		}
	}

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		WriteableBitmap viewport_bitmap;
		Thread thread;


		public MainWindow()
		{
			InitializeComponent();

			viewport_bitmap = new WriteableBitmap(250, 250, 0, 0, PixelFormats.Bgra32, null);
			viewport_img.Source = viewport_bitmap;

			BackEnd.initPixDebugger();
			BackEnd.init();

			thread = new Thread(backEndLoop);
			thread.Start();
		}

		void backEndLoop()
		{
			int min_frame_duration = 16;
			var stopwatch = new Stopwatch();

			while (true) {

				stopwatch.Restart();
				{
					BackEnd.phase_1_runCPU();
					BackEnd.phase_2_waitForRendering();

					// Set Viewport Image
					Dispatcher.Invoke(() => {

						viewport_bitmap.Lock();

						unsafe {
							BackEnd.phase_2X_tryDownloadRender(
								(uint)viewport_bitmap.PixelWidth,
								(uint)viewport_bitmap.PixelHeight,
								(byte*)viewport_bitmap.BackBuffer.ToPointer()
							);
						}

						viewport_bitmap.AddDirtyRect(new Int32Rect(0, 0, viewport_bitmap.PixelWidth, viewport_bitmap.PixelHeight));
						viewport_bitmap.Unlock();
					});

					BackEnd.phase_3_render();
				}
				stopwatch.Stop();

				// execution finished early so sleep
				if (stopwatch.ElapsedMilliseconds < min_frame_duration) {
					Thread.Sleep(min_frame_duration - (int)stopwatch.ElapsedMilliseconds);
				}
			}
		}

		void createControls(object sender, RoutedEventArgs e)
		{
			//var menu = new Menu();

			//var menu_item = new MenuItem();
			//menu_item.ToolTip

			//menu.Items.Add
		}

		void capture(object sender, RoutedEventArgs e)
		{
			BackEnd.captureFrame();
		}
	}
}
