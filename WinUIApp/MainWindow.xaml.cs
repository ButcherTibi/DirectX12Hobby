using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.


namespace WinUIApp
{
	/// <summary>
	/// An empty window that can be used on its own or navigated to within a Frame.
	/// </summary>
	public sealed partial class MainWindow : Window
	{
		bool is_webview_init = false;

		public MainWindow()
		{
			this.InitializeComponent();

#if DEBUG
			DevGlobals.initHotReload(this, webview);
#endif
		}

		async void windowSizeChanged(object sender, WindowSizeChangedEventArgs e)
		{
			webview.Width = e.Size.Width;
			webview.Height = e.Size.Height;

			if (is_webview_init == false) {

				await webview.EnsureCoreWebView2Async();
				webViewInit();
				is_webview_init = true;
			}
		}

		private void webViewInit()
		{
			webview.CoreWebView2.SetVirtualHostNameToFolderMapping(
				"app.invalid", "./ClientSide/", Microsoft.Web.WebView2.Core.CoreWebView2HostResourceAccessKind.Allow
			);
			// webview.CoreWebView2.Settings

			// source.CoreWebView2.AddWebResourceRequestedFilter();
			// source.CoreWebView2.WebResourceRequested

			webview.Source = new Uri("http://app.invalid/index.html");
		}

		//private void myButton_Click(object sender, RoutedEventArgs e)
		//{
		//	myButton.Content = "Clicked";
		//}
	}
}
