using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
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
		FileSystemWatcher file_watch;

		public MainWindow()
		{
			this.InitializeComponent();

#if DEBUG
			file_watch = new FileSystemWatcher();
			file_watch.BeginInit();

			file_watch.Path = DevGlobals.client_side_src;
			file_watch.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.CreationTime;
			file_watch.IncludeSubdirectories = true;
			file_watch.EnableRaisingEvents = true;

			var copy_over_file = (object sender, FileSystemEventArgs e) => {

				string relative_path = e.FullPath.Split(DevGlobals.client_side_src)[1];
				string dest_filepath = Path.Combine(DevGlobals.client_side_dest, relative_path);

				if (File.Exists(dest_filepath) == false) {
					System.Diagnostics.Debugger.Break();
				}

				File.Copy(e.FullPath, dest_filepath, true);

				DispatcherQueue.TryEnqueue(() => {
					webview.Reload();
				});
			};

			file_watch.Changed += new FileSystemEventHandler(copy_over_file);
			file_watch.Created += new FileSystemEventHandler(copy_over_file);

			file_watch.EndInit();
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

		private void myButton_Click(object sender, RoutedEventArgs e)
		{
			myButton.Content = "Clicked";
		}
	}
}
