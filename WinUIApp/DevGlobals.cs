using System;
using System.IO;
using WebView2 = Microsoft.UI.Xaml.Controls.WebView2;


public static class DevGlobals
{
	static string project_dir;
	static string output_dir;
		  
	static string client_side_src;
	static string client_side_dest;

	static WinUIApp.MainWindow main_window;
	static WebView2 main_webview;
	static FileSystemWatcher client_side_watcher;


	static DevGlobals() {
		output_dir = AppDomain.CurrentDomain.BaseDirectory;

		project_dir = Path.GetFullPath(Path.Combine(
			AppDomain.CurrentDomain.BaseDirectory,
			@"..\..\..\..\..\..\"
		));

		string client_dir_name = "ClientSide";
		client_side_src = Path.Combine(project_dir, client_dir_name);
		client_side_dest = Path.Combine(output_dir, client_dir_name);
	}

	public static void initHotReload(WinUIApp.MainWindow new_window, WebView2 new_webview)
	{
		main_window = new_window;
		main_webview = new_webview;
		client_side_watcher = new FileSystemWatcher();

		var watcher = client_side_watcher;

		watcher.BeginInit();

		watcher.Path = client_side_src;
		watcher.NotifyFilter = NotifyFilters.LastWrite;
		watcher.IncludeSubdirectories = true;
		watcher.EnableRaisingEvents = true;

		var copy_over_file = (object sender, FileSystemEventArgs e) => {

			string relative_path = e.FullPath.Split(client_side_src)[1];
			string dest_filepath = Path.Join(client_side_dest, relative_path);

			switch (Path.GetExtension(e.FullPath)) {
			case ".ts": {

				var process = new System.Diagnostics.Process();
				process.StartInfo.FileName = "CMD.exe";
				process.StartInfo.Arguments = $"/c tsc --outFile \"bundle.js\"";
				process.StartInfo.CreateNoWindow = true;
				process.StartInfo.WorkingDirectory = client_side_src;
				process.StartInfo.RedirectStandardOutput = true;
				process.OutputDataReceived += (sender, e) =>
				{
					if (e.Data != null) {
						System.Diagnostics.Debug.WriteLine(e.Data);
					}
				};

				process.Start();
				process.BeginOutputReadLine();

				process.WaitForExit();

				string src = Path.Join(client_side_src, "bundle.js");
				string dest = Path.Join(client_side_dest, "bundle.js");
				File.Copy(src, dest, true);
				break;
			}
			case ".html":
			case ".css": {
				File.Copy(e.FullPath, dest_filepath, true);
				break;
			}
			case ".js": break;
			default: {
				System.Diagnostics.Debugger.Break();
				break;
			}
			}

			main_window.DispatcherQueue.TryEnqueue(() => {
				main_webview.Reload();
			});
		};

		watcher.Changed += new FileSystemEventHandler(copy_over_file);

		watcher.EndInit();
	}
}