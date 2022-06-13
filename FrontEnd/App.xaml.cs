using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace FrontEnd {
	/// <summary>
	/// Interaction logic for App.xaml
	/// </summary>
	public partial class App : Application {

		App()
		{
			// Backend
			{
				BackEnd.init(true);
			}

			// Browser
			{
				var settings = new CefSharp.Wpf.CefSettings();

				// Increase the log severity so CEF outputs detailed information, useful for debugging
				settings.LogSeverity = CefSharp.LogSeverity.Verbose;

				CefSharp.Cef.Initialize(settings);
			}
		}
	}
}
