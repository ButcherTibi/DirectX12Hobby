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


namespace FrontEnd {
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {
		public MainWindow()
		{
			InitializeComponent();

			//var process = new System.Diagnostics.Process();
			//process.StartInfo.FileName = "cmd.exe";
			//process.StartInfo.Arguments = "/c " + "mkdir foo";
			//process.StartInfo.WorkingDirectory = @"F:\";
			//process.Start();
			//process.WaitForExit();
			BackEnd.initPixDebugger();
		}

		private void render(object sender, RoutedEventArgs e)
		{
			BackEnd.init();
		}

		private void setImage(object sender, RoutedEventArgs e)
		{
			PixelFormat pf = PixelFormats.Rgb24;
			int width = 250;
			int height = 250;
			byte[] rawImage = new byte[width * height * 3];

			for (uint row = 0; row < height; row += 2) {
				for (uint col = 0; col < width; col++) {
					rawImage[row * (width * 3) + col * 3] = 0xFF;
				}
			}

			for (uint row = 1; row < height; row += 2) {
				for (uint col = 0; col < width; col++) {
					rawImage[row * (width * 3) + col * 3 + 2] = 0xFF;
				}
			}

			// Create a BitmapSource.
			BitmapSource bitmap = BitmapSource.Create(
				width, height,
				0, 0, pf, null,
				rawImage, width * 3
			);

			test_image.Source = bitmap;
		}
	}
}
