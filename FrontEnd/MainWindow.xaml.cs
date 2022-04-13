using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
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

		bool render_init = false;


		public MainWindow()
		{
			InitializeComponent();

			BackEnd.initPixDebugger();
		}

		private void initRender(object sender, RoutedEventArgs e)
		{
			if (render_init == false) {
				BackEnd.initRender();
			}
		}
	}
}
