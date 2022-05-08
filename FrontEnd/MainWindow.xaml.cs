using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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

using Timer = System.Threading.Timer;


namespace FrontEnd {
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		WriteableBitmap viewport_bitmap;

		public MainWindow()
		{
			InitializeComponent();

			BackEnd.initPixDebugger();
			BackEnd.init();

			viewport_bitmap = new WriteableBitmap(250, 250, 0, 0, PixelFormats.Bgra32, null);
			viewport_img.Source = viewport_bitmap;
		}

		//private void changeColor(object? obj)
		//{
		//	Dispatcher.Invoke(() => {

		//		Random random = new Random();
		//		int red = random.Next(0, 255);
		//		int blue = random.Next(0, 255);

		//		int width = 250;
		//		int height = 250;

		//		if (viewport_bitmap.TryLock(Duration.Forever)) {

		//			unsafe {
		//				byte* mem = (byte*)viewport_bitmap.BackBuffer.ToPointer();

		//				for (int row = 0; row < height; row += 2) {
		//					for (int col = 0; col < width; col++) {
		//						mem[row * (width * 4) + col * 4 + 0] = 0;
		//						mem[row * (width * 4) + col * 4 + 1] = 0;
		//						mem[row * (width * 4) + col * 4 + 2] = (byte)red;
		//						mem[row * (width * 4) + col * 4 + 3] = 0xFF;
		//					}
		//				}

		//				for (int row = 1; row < height; row += 2) {
		//					for (int col = 0; col < width; col++) {
		//						mem[row * (width * 4) + col * 4 + 0] = (byte)blue;
		//						mem[row * (width * 4) + col * 4 + 1] = 0;
		//						mem[row * (width * 4) + col * 4 + 2] = 0;
		//						mem[row * (width * 4) + col * 4 + 3] = 0xFF;
		//					}
		//				}
		//			}
		//			viewport_bitmap.AddDirtyRect(new Int32Rect(0, 0, width, height));
		//			viewport_bitmap.Unlock();
		//		}
		//	});
		//}

		private void render(object sender, RoutedEventArgs e)
		{
			// BackEnd.captureFrame();

			viewport_bitmap.Lock();

			unsafe {
				BackEnd.tryCopyLastRender(
					(uint)viewport_bitmap.PixelWidth,
					(uint)viewport_bitmap.PixelHeight,
					(byte*)viewport_bitmap.BackBuffer.ToPointer()
				);
			}
		
			viewport_bitmap.AddDirtyRect(new Int32Rect(0, 0, viewport_bitmap.PixelWidth, viewport_bitmap.PixelHeight));
			viewport_bitmap.Unlock();
		}
	}
}
